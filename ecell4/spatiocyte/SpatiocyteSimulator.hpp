#ifndef ECELL4_LATTICE_LATTICE_SIMULATOR_HPP
#define ECELL4_LATTICE_LATTICE_SIMULATOR_HPP

#include <boost/lexical_cast.hpp>
#include <memory>
#include <numeric>

#include <ecell4/core/EventScheduler.hpp>
#include <ecell4/core/Model.hpp>
#include <ecell4/core/RandomNumberGenerator.hpp>
#include <ecell4/core/ReactionRule.hpp>
#include <ecell4/core/SimulatorBase.hpp>
#include <ecell4/core/VoxelPool.hpp>

#include "SpatiocyteEvent.hpp"
#include "SpatiocyteWorld.hpp"

namespace ecell4
{

namespace spatiocyte
{

class SpatiocyteSimulator : public SimulatorBase<SpatiocyteWorld> {
public:
    typedef SimulatorBase<SpatiocyteWorld> base_type;
    typedef SpatiocyteEvent::reaction_type reaction_type;
    typedef EventSchedulerBase<SpatiocyteEvent> scheduler_type;
    typedef std::unordered_map<Species, Real> alpha_map_type;

public:
    SpatiocyteSimulator(std::shared_ptr<SpatiocyteWorld> world,
                        std::shared_ptr<Model> model)
        : base_type(world, model)
    {
        initialize();
    }

    SpatiocyteSimulator(std::shared_ptr<SpatiocyteWorld> world)
        : base_type(world)
    {
        initialize();
    }

    virtual Real dt() const { return dt_; }

    void initialize();
    void finalize();
    void step();
    bool step(const Real &upto);

    virtual bool check_reaction() const { return last_reactions().size() > 0; }

    const std::vector<SpatiocyteEvent::reaction_type> &last_reactions() const
    {
        // return last_event_->reactions();
        return last_reactions_;
    }

protected:
    std::shared_ptr<SpatiocyteEvent>
    create_step_event(const Species &species, const Real &t, const Real &alpha);
    std::shared_ptr<SpatiocyteEvent>
    create_zeroth_order_reaction_event(const ReactionRule &reaction_rule,
                                       const Real &t);
    std::shared_ptr<SpatiocyteEvent>
    create_first_order_reaction_event(const ReactionRule &reaction_rule,
                                      const Real &t);

    void step_();
    void register_events(const Species &species);
    void update_alpha_map();

    void set_last_event_(std::shared_ptr<const SpatiocyteEvent> event)
    {
        last_event_ = event;
    }

protected:
    scheduler_type scheduler_;
    std::shared_ptr<const SpatiocyteEvent> last_event_;
    alpha_map_type alpha_map_;

    std::vector<reaction_type> last_reactions_;

    std::vector<Species> species_list_;

    Real dt_;
};

} // namespace spatiocyte

} // namespace ecell4

#endif /* ECELL4_LATTICE_LATTICE_SIMULATOR_HPP */
