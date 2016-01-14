#ifndef __ECELL4_LATTICE_LATTICE_SIMULATOR_HPP
#define __ECELL4_LATTICE_LATTICE_SIMULATOR_HPP

#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <ecell4/core/Model.hpp>
#include <ecell4/core/ReactionRule.hpp>
#include <ecell4/core/Reaction.hpp>
#include <ecell4/core/MolecularTypeBase.hpp>
#include <ecell4/core/SimulatorBase.hpp>
#include <ecell4/core/RandomNumberGenerator.hpp>
#include <ecell4/core/EventScheduler.hpp>

#include "SpatiocyteWorld.hpp"

namespace ecell4
{

namespace spatiocyte
{

class ReactionInfo
{
public:

    typedef std::pair<ParticleID, Voxel> particle_id_pair_type;
    typedef std::vector<particle_id_pair_type> container_type;

public:

    ReactionInfo(
        const Real t,
        const container_type& reactants,
        const container_type& products)
        : t_(t), reactants_(reactants), products_(products)
    {}

    ReactionInfo(const ReactionInfo& another)
        : t_(another.t()), reactants_(another.reactants()), products_(another.products())
    {}

    Real t() const
    {
        return t_;
    }

    const container_type& reactants() const
    {
        return reactants_;
    }

    void add_reactant(const particle_id_pair_type& pid_pair)
    {
        reactants_.push_back(pid_pair);
    }

    const container_type& products() const
    {
        return products_;
    }

    void add_product(const particle_id_pair_type& pid_pair)
    {
        products_.push_back(pid_pair);
    }

protected:

    Real t_;
    container_type reactants_, products_;
};

class SpatiocyteSimulator
    : public SimulatorBase<Model, SpatiocyteWorld>
{
public:

    typedef SimulatorBase<Model, SpatiocyteWorld> base_type;
    typedef Reaction<Voxel> reaction_type;

    typedef ReactionInfo reaction_info_type;

protected:

    struct StepEvent : EventScheduler::Event
    {
        StepEvent(
            SpatiocyteSimulator* sim, const Species& species, const Real& t,
            const Real alpha=1.0)
            : EventScheduler::Event(t), sim_(sim), species_(species), alpha_(alpha)
        {
            const SpatiocyteWorld::molecule_info_type
                minfo(sim_->world_->get_molecule_info(species));
            const Real R(minfo.radius);
            const Real D(minfo.D);
            const MolecularTypeBase* mtype(sim_->world_->find_molecular_type(species));
            // const Real R(sim_->world_->voxel_radius());
            // Real D = boost::lexical_cast<Real>(species.get_attribute("D"));
            if (D <= 0)
            {
                dt_ = inf;
            } else if(mtype->get_dimension() == Shape::THREE) {
                dt_ = 2 * R * R / 3 / D * alpha_;
            } else if(mtype->get_dimension() == Shape::TWO) {
                // TODO: Regular Lattice
                // dt_  = pow((2*sqrt(2.0)+4*sqrt(3.0)+3*sqrt(6.0)+sqrt(22.0))/
                //           (6*sqrt(2.0)+4*sqrt(3.0)+3*sqrt(6.0)), 2) * R * R / D * alpha_;
                dt_ = R * R / D * alpha_;
            } else if(mtype->get_dimension() == Shape::ONE) {
                dt_ = 2 * R * R / D * alpha_;
            }
            else
            {
                throw NotSupported(
                    "The dimension of a structure must be two or three.");
            }

            time_ = t + dt_;
            // time_ = t;
        }

        virtual ~StepEvent()
        {
            ;
        }

        virtual void fire()
        {
            sim_->walk(species_, alpha_);
            time_ += dt_;
        }

        Species const& species() const
        {
            return species_;
        }

        Real const& alpha() const
        {
            return alpha_;
        }

    protected:

        SpatiocyteSimulator* sim_;
        Species species_;
        MolecularTypeBase* mt_;
        const Real alpha_;
    };

    struct ZerothOrderReactionEvent : EventScheduler::Event
    {
        ZerothOrderReactionEvent(
            SpatiocyteSimulator* sim, const ReactionRule& rule, const Real& t)
            : EventScheduler::Event(t), sim_(sim), rule_(rule)
        {
            time_ = t + draw_dt();
        }

        virtual ~ZerothOrderReactionEvent()
        {
        }

        virtual void fire()
        {
            sim_->apply_zeroth_order_reaction_(rule_);
            time_ += draw_dt();
        }

        virtual void interrupt(Real const& t)
        {
            time_ = t + draw_dt();
        }

        Real draw_dt()
        {
            const Real k(rule_.k());
            const Real p = k * sim_->world()->volume();
            Real dt(inf);
            if (p != 0.)
            {
                const Real rnd(sim_->world_->rng()->uniform(0.,1.));
                dt = - log(1 - rnd) / p;
            }
            return dt;
        }

    protected:

        SpatiocyteSimulator* sim_;
        ReactionRule rule_;
    };

    struct FirstOrderReactionEvent : EventScheduler::Event
    {
        FirstOrderReactionEvent(
            SpatiocyteSimulator* sim, const ReactionRule& rule, const Real& t)
            : EventScheduler::Event(t), sim_(sim), rule_(rule)
        {
            //assert(rule_.reactants().size() == 1);
            time_ = t + draw_dt();
        }

        virtual ~FirstOrderReactionEvent()
        {
        }

        virtual void fire()
        {
            const Species reactant(*(rule_.reactants().begin()));
            sim_->apply_first_order_reaction_(rule_, sim_->world_->choice(reactant));
            time_ += draw_dt();
        }

        virtual void interrupt(Real const& t)
        {
            time_ = t + draw_dt();
        }

        Real draw_dt()
        {
            const Species reactant(*(rule_.reactants().begin()));
            const Integer num_r(sim_->world_->num_voxels_exact(reactant));
            const Real k(rule_.k());
            const Real p = k * num_r;
            Real dt(inf);
            if (p != 0.)
            {
                const Real rnd(sim_->world_->rng()->uniform(0.,1.));
                dt = - log(1 - rnd) / p;
            }
            return dt;
        }

    protected:

        SpatiocyteSimulator* sim_;
        ReactionRule rule_;
    };

public:

    SpatiocyteSimulator(
            boost::shared_ptr<Model> model,
            boost::shared_ptr<SpatiocyteWorld> world,
            const Real alpha = 1.0)
        : base_type(model, world), alpha_(alpha)
    {
        initialize();
    }

    SpatiocyteSimulator(
            boost::shared_ptr<SpatiocyteWorld> world,
            const Real alpha = 1.0)
        : base_type(world), alpha_(alpha)
    {
        initialize();
    }

    virtual Real dt() const
    {
        return dt_;
    }

    void initialize();
    void finalize();
    void step();
    bool step(const Real& upto);
    // void run(const Real& duration);
    void walk(const Species& species);
    void walk(const Species& species, const Real& alpha);
    Real calculate_alpha(const ReactionRule& rule) const;

    virtual bool check_reaction() const
    {
        return last_reactions_.size() > 0;
    }

    std::vector<std::pair<ReactionRule, reaction_info_type> > last_reactions() const
    {
        return last_reactions_;
    }

    void set_alpha(const Real alpha)
    {
        if (alpha < 0 || alpha > 1)
        {
            return;  // XXX: ValueError
        }

        alpha_ = alpha;
        initialize();
    }

    Real get_alpha() const
    {
        return alpha_;
    }

protected:

    boost::shared_ptr<EventScheduler::Event> create_step_event(
        const Species& species, const Real& t);
    boost::shared_ptr<EventScheduler::Event> create_zeroth_order_reaction_event(
        const ReactionRule& reaction_rule, const Real& t);
    boost::shared_ptr<EventScheduler::Event> create_first_order_reaction_event(
        const ReactionRule& reaction_rule, const Real& t);
    Real calculate_dimensional_factor(
        const MolecularTypeBase* mt0, const MolecularTypeBase* mt1) const;
    std::pair<bool, reaction_type> attempt_reaction_(
        const SpatiocyteWorld::particle_info_type info,
        SpatiocyteWorld::coordinate_type to_coord, const Real& alpha);
    std::pair<bool, reaction_type> apply_second_order_reaction_(
        const ReactionRule& reaction_rule,
        const reaction_type::particle_type& p0,
        const reaction_type::particle_type& p1);
    std::pair<bool, reaction_type> apply_first_order_reaction_(
        const ReactionRule& reaction_rule,
        const reaction_type::particle_type& p);
    void apply_vanishment(
        const SpatiocyteWorld::particle_info_type from_info,
        const SpatiocyteWorld::particle_info_type to_info,
        reaction_type& reaction);
    void apply_ab2c(
        const SpatiocyteWorld::particle_info_type from_info,
        const SpatiocyteWorld::particle_info_type to_info,
        const Species& product_species,
        reaction_type& reaction);
    void apply_ab2cd(
        const SpatiocyteWorld::particle_info_type from_info,
        const SpatiocyteWorld::particle_info_type to_info,
        const Species& product_species0,
        const Species& product_species1,
        reaction_type& reaction);
    void apply_ab2cd_in_order(
        const SpatiocyteWorld::private_coordinate_type coord0,
        const Species& product_species0,
        const SpatiocyteWorld::private_coordinate_type coord1,
        const Species& product_species1,
        reaction_type& reaction);
    void apply_a2b(
        const SpatiocyteWorld::particle_info_type pinfo,
        const Species& product_species,
        reaction_type& reaction);
    bool apply_a2bc(
        const SpatiocyteWorld::particle_info_type pinfo,
        const Species& product_species0,
        const Species& product_species1,
        reaction_type& reaction);
    std::pair<bool, reaction_type> apply_zeroth_order_reaction_(
        const ReactionRule& reaction_rule);

    void register_product_species(const Species& product_species);
    void register_reactant_species(
        const SpatiocyteWorld::particle_info_type pinfo, reaction_type& reaction) const;

    void step_();
    void register_events(const Species& species);
    // void register_step_event(const Species& species);

    void walk_in_space_(const MolecularTypeBase* mtype, const Real& alpha);
    void walk_on_surface_(const MolecularTypeBase* mtype, const Real& alpha);

    inline Voxel private_voxel2voxel(const Voxel& v) const
    {
        const SpatiocyteWorld::coordinate_type
            coord(world_->private2coord(v.coordinate()));
        return Voxel(v.species(), coord, v.radius(), v.D(), v.loc());
    }

    const std::string get_serial(
        const SpatiocyteWorld::private_coordinate_type coord) const
    {
        const MolecularTypeBase* mtype(world_->get_molecular_type_private(coord));
        return mtype->is_vacant() ? "" : mtype->species().serial();
    }

    const std::string get_location(
        const SpatiocyteWorld::private_coordinate_type coord) const
    {
        const MolecularTypeBase* mtype(world_->get_molecular_type_private(coord));
        if (mtype->is_vacant())
        {
            return "";
        }
        const MolecularTypeBase* ltype(mtype->location());
        return ltype->is_vacant() ? "" : ltype->species().serial();
    }

protected:

    EventScheduler scheduler_;
    std::vector<std::pair<ReactionRule, reaction_info_type> > last_reactions_;
    std::vector<Species> new_species_;
    std::vector<unsigned int> nids_; // neighbor indexes

    Real dt_;
    Real alpha_;
};

} // spatiocyte

} // ecell4

#endif /* __ECELL4_LATTICE_LATTICE_SIMULATOR_HPP */
