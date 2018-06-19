#ifndef ECELL4_SGFRD_WORLD
#define ECELL4_SGFRD_WORLD
#include "StructureRegistrator.hpp"
#include "Informations.hpp"
#include <ecell4/core/ParticleSpaceCellListImpl.hpp>
#include <ecell4/core/SerialIDGenerator.hpp>
#include <ecell4/core/Polygon.hpp>
#include <ecell4/core/Context.hpp>
#include <ecell4/core/Segment.hpp>
#include <ecell4/core/Model.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace ecell4
{
namespace sgfrd
{

class SGFRDWorld : public ecell4::Space
{
  public:
    typedef ecell4::Polygon  polygon_type;
    typedef polygon_type::FaceID   FaceID;
    typedef polygon_type::EdgeID   EdgeID;
    typedef polygon_type::VertexID VertexID;
    typedef Barycentric barycentric_type;

    typedef ecell4::Model model_type;
    typedef ecell4::sgfrd::MoleculeInfo molecule_info_type;

    typedef ParticleSpaceCellListImpl default_particle_space_type;
    typedef ParticleSpace particle_space_type;
    typedef particle_space_type::particle_container_type
        particle_container_type;
    typedef ecell4::SerialIDGenerator<ParticleID> particle_id_generator_type;
    typedef StructureRegistrator<ParticleID, FaceID>
        structure_registrator_type;

  public:

    SGFRDWorld(const Real3& edge_lengths, const Integer3& matrix_sizes,
               const boost::shared_ptr<polygon_type>& polygon)
        : ps_(new default_particle_space_type(edge_lengths, matrix_sizes)),
          polygon_(polygon), registrator_(*polygon)
    {
        rng_ = boost::shared_ptr<RandomNumberGenerator>(
            new GSLRandomNumberGenerator());
        rng_->seed();

        this->prepair_barriers();
    }

    SGFRDWorld(const Real3& edge_lengths, const Integer3& matrix_sizes,
               const boost::shared_ptr<polygon_type>& polygon,
               boost::shared_ptr<RandomNumberGenerator> rng)
        : ps_(new default_particle_space_type(edge_lengths, matrix_sizes)),
          rng_(rng), polygon_(polygon), registrator_(*polygon)
    {this->prepair_barriers();}

    ~SGFRDWorld(){}

    boost::shared_ptr<RandomNumberGenerator> const& rng() {return this->rng_;}
    boost::shared_ptr<polygon_type> const& polygon() const {return polygon_;}

    const Real t()                  const {return ps_->t();}
    void       set_t(const Real& t)       {return ps_->set_t(t);}
    const Real volume()             const {return ps_->volume();}
    Real get_value(const Species& sp)       const {return ps_->get_value(sp);}
    Real get_value_exact(const Species& sp) const {return ps_->get_value_exact(sp);}

    Integer num_species() const {return ps_->num_species();}
    bool has_species(const Species& sp) const {return ps_->has_species(sp);}
    std::vector<Species> list_species() const {return ps_->list_species();}

// CellListImpl stuff
//     Real3 const& edge_lengths() const {return ps_->edge_lengths();}
//     Real3 const& cell_sizes()   const {return ps_->cell_sizes();}
//     Integer3     matrix_sizes() const {return ps_->matrix_sizes();}
//     void reset(const Real3& edge_lengths) {ps_->reset(edge_lengths);}

    std::pair<std::pair<ParticleID, Particle>, bool>
    new_particle(const Particle& p);
    std::pair<std::pair<ParticleID, Particle>, bool>
    new_particle(const Particle& p, const FaceID& fid);

    std::pair<std::pair<ParticleID, Particle>, bool>
    throw_in_particle(const Species& sp);

    void add_molecule(const Species& sp, const std::size_t N);

    bool update_particle(const ParticleID& pid, const Particle& p)
    {
        return ps_->update_particle(pid, p);
    }
    bool update_particle(const ParticleID& pid, const Particle& p,
                         const FaceID& fid)
    {
        if(registrator_.have(pid))
        {
            registrator_.update(pid, fid);
        }
        else
        {
            registrator_.emplace(pid, fid);
        }
        return ps_->update_particle(pid, p);
    }

    // this also removes particle if it is on surface
    void remove_particle(const ParticleID& pid)
    {
        if(registrator_.have(pid)) registrator_.remove(pid);
        return ps_->remove_particle(pid);
    }
    void remove_particle(const ParticleID& pid, const FaceID& fid)
    {
        registrator_.remove(pid, fid);
        return ps_->remove_particle(pid);
    }

    std::pair<ParticleID, Particle>
    get_particle(const ParticleID& pid) const {return ps_->get_particle(pid);}
    bool
    has_particle(const ParticleID& pid) const {return ps_->has_particle(pid);}
    bool
    has_particle(const FaceID& fid) const
    {
        return registrator_.elements_over(fid).size() > 0;
    }
    Integer
    num_particle(const FaceID& fid) const
    {
        return registrator_.elements_over(fid).size();
    }

    Integer
    num_particles()                        const {return ps_->num_particles();}
    Integer
    num_particles(const Species& sp)       const {return ps_->num_particles(sp);}
    Integer
    num_particles_exact(const Species& sp) const {return ps_->num_particles_exact(sp);}
    Integer
    num_molecules(const Species& sp)       const {return ps_->num_molecules(sp);}
    Integer
    num_molecules_exact(const Species& sp) const {return ps_->num_molecules_exact(sp);}

    std::vector<std::pair<ParticleID, Particle> >
    list_particles() const {return ps_->list_particles();}
    std::vector<std::pair<ParticleID, Particle> >
    list_particles(const Species& sp) const {return ps_->list_particles(sp);}
    std::vector<std::pair<ParticleID, Particle> >
    list_particles_exact(const Species& sp) const {return ps_->list_particles_exact(sp);}

    std::vector<std::pair<ParticleID, Particle> >
    list_particles(const FaceID& fid) const
    {
        std::vector<ParticleID> const& pids = registrator_.elements_over(fid);
        std::vector<std::pair<ParticleID, Particle> > retval(pids.size());
        std::transform(pids.begin(), pids.end(), retval.begin(),
                       boost::bind(&SGFRDWorld::get_particle, this, _1));
        return retval;
    }
    std::vector<ParticleID> const&
    list_particleIDs(const FaceID& fid) const
    {
        return registrator_.elements_over(fid);
    }

    bool is_on_face(const ParticleID& pid) const
    {
        return registrator_.have(pid);
    }

    FaceID get_face_id(const ParticleID& pid) const
    {
        return registrator_.structure_on(pid);
    }

    void save(const std::string& fname) const
    {
        throw NotImplemented("SGFRDWorld::save");
    }
    void load(const std::string& fname) const
    {
        throw NotImplemented("SGFRDWorld::load");
    }

#ifdef WITH_HDF5
    void save_hdf5(H5::Group* root) const
    {
        throw NotImplemented("SGFRDWorld::save_hdf5");
    }

    void load_hdf5(const H5::Group& root)
    {
        throw NotImplemented("SGFRDWorld::load_hdf5");
    }
#endif

    //TODO: consider periodic transpose in the same way as ParticleSpaceCellListImpl
    Real distance_sq(const Real3& lhs, const Real3& rhs)
    {
        return length_sq(lhs - rhs);
    }
    Real distance(const Real3& lhs, const Real3& rhs)
    {
        return length(lhs - rhs);
    }

    template<typename str1T, typename str2T>
    Real distance_sq(const std::pair<Real3, str1T>& lhs,
                     const std::pair<Real3, str2T>& rhs)
    {
        return ecell4::polygon::distance_sq(*polygon_, lhs, rhs);
    }
    template<typename str1T, typename str2T>
    Real distance(const std::pair<Real3, str1T>& lhs,
                  const std::pair<Real3, str2T>& rhs)
    {
        return ecell4::polygon::distance(*polygon_, lhs, rhs);
    }

    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const Real3& pos, const Real& radius) const
    {
        return ps_->list_particles_within_radius(pos, radius);
    }
    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const Real3& pos, const Real& radius,
            const ParticleID& ignore) const
    {
        return ps_->list_particles_within_radius(pos, radius, ignore);
    }
    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const Real3& pos, const Real& radius,
            const ParticleID& ignore1, const ParticleID& ignore2) const
    {
        return ps_->list_particles_within_radius(pos, radius, ignore1, ignore2);
    }

    // for 2D
    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const std::pair<Real3, FaceID>& pos, const Real& radius) const;
    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const std::pair<Real3, FaceID>& pos, const Real& radius,
            const ParticleID& ignore) const;
    std::vector<std::pair<std::pair<ParticleID, Particle>, Real> >
        list_particles_within_radius(
            const std::pair<Real3, FaceID>& pos, const Real& radius,
            const ParticleID& ignore1, const ParticleID& ignore2) const;

    std::vector<std::pair<VertexID, Real> > list_vertices_within_radius(
            const std::pair<Real3, FaceID>& pos, const Real& radius) const
    {
        const Real threshold_sq = radius * radius;
        std::vector<std::pair<VertexID, Real> > retval;
        const std::vector<VertexID> candidates =
            this->polygon_->neighbor_vertices_of(pos.second);
        for(std::vector<VertexID>::const_iterator
                i(candidates.begin()), e(candidates.end()); i!=e; ++i)
        {
            const VertexID vid = *i;
            const Real dist_sq = polygon_->distance_sq(pos,
                    std::make_pair(this->polygon_->position_at(vid), vid));
            if(dist_sq < threshold_sq)
            {
                retval.push_back(std::make_pair(vid, std::sqrt(dist_sq)));
            }
        }
        std::sort(retval.begin(), retval.end(),
              ecell4::utils::pair_second_element_comparator<VertexID, Real>());
        return retval;
    }

    // return false if overlap exists. for 3D. FIXME: speedup
    bool check_no_overlap(const Real3& pos, const Real& radius) const
    {
        return ps_->list_particles_within_radius(pos, radius).empty();
    }
    bool check_no_overlap(const Real3& pos, const Real& radius,
            const ParticleID& ignore) const
    {
        return ps_->list_particles_within_radius(pos, radius, ignore).empty();
    }
    bool check_no_overlap(const Real3& pos, const Real& radius,
            const ParticleID& ignore1, const ParticleID& ignore2) const
    {
        return ps_->list_particles_within_radius(pos, radius, ignore1, ignore2
                ).empty();
    }

    // return false if overlap exists.
    bool check_no_overlap(
            const std::pair<Real3, FaceID>& pos, const Real& radius) const;
    bool check_no_overlap(
            const std::pair<Real3, FaceID>& pos, const Real& radius,
            const ParticleID& ignore) const;
    bool check_no_overlap(
            const std::pair<Real3, FaceID>& pos, const Real& radius,
            const ParticleID& ignore1, const ParticleID& ignore2) const;

    particle_container_type const& particles() const {return ps_->particles();}


    void bind_to(boost::shared_ptr<model_type> model)
    {
        if (boost::shared_ptr<model_type> bound_model = lock_model())
        {
            if (bound_model.get() != model.get())
            {
                std::cerr << "Warning: Model already bound to BDWorld"
                    << std::endl;
            }
        }
        model_ = model;
    }

    boost::shared_ptr<model_type> lock_model() const
    {
        return model_.lock();
    }

    molecule_info_type get_molecule_info(const Species& sp) const
    {
        if(sp.has_attribute("radius") && sp.has_attribute("D"))
        {
            return MoleculeInfo(
                    boost::lexical_cast<Real>(sp.get_attribute("radius")),
                    boost::lexical_cast<Real>(sp.get_attribute("D")));
        }
        else if(boost::shared_ptr<Model> bound_model = lock_model())
        {
            Species attr(bound_model->apply_species_attributes(sp));
            if (attr.has_attribute("radius") && attr.has_attribute("D"))
            {
                return MoleculeInfo(
                        boost::lexical_cast<Real>(attr.get_attribute("radius")),
                        boost::lexical_cast<Real>(attr.get_attribute("D")));
            }
        }
        return MoleculeInfo(0., 0.);
    }

    boost::array<ecell4::Segment, 6> const& barrier_at(const FaceID& fid) const
    {
        return this->barriers_.at(fid);
    }

  private:

    void prepair_barriers()
    {
        // this contains the edges that correspond to the developed neighbor faces.
        //
        //        /\
        //     > /__\ <
        //      /\* /\
        //   > /__\/__\ < these edges
        //      ^    ^
        const std::vector<FaceID> faces = polygon_->list_face_ids();
        for(std::vector<FaceID>::const_iterator
                iter(faces.begin()), iend(faces.end()); iter!=iend; ++iter)
        {
            const FaceID fid = *iter;
            const Triangle& tri = polygon_->triangle_at(fid);

            boost::array<Segment, 6> segments;
            boost::array<EdgeID, 3> const& edges = polygon_->edges_of(fid);
            for(std::size_t i=0; i<3; ++i)
            {
                const EdgeID eid = edges[i];
                const Real3 orig = tri.vertex_at(i);
                const Real3  vtx = orig + rotate(
                    -1 * this->polygon_->tilt_angle_at(eid),
                    this->polygon_->direction_of(eid),
                    this->polygon_->direction_of(this->polygon_->next_of(
                            this->polygon_->opposite_of(eid))));

                const Real3  vtx_ = orig + rotate(
                    -1 * this->polygon_->tilt_angle_at(eid),
                    this->polygon_->direction_of(this->polygon_->opposite_of(
                            this->polygon_->next_of(eid))),
                    this->polygon_->direction_of(this->polygon_->next_of(
                            this->polygon_->opposite_of(eid))));

                const Real dist = length(vtx - vtx_);
                if(dist > 1e-12)
                {
                    std::cerr << "[World::prepair_barriers]: "
                              << "dist between first calculation " << vtx
                              << " and second calculation " << vtx_
                              << " is too large = " << dist << std::endl;
                    assert(false);
                }

                segments[i*2    ] = Segment(orig, vtx);
                segments[i*2 + 1] = Segment(this->polygon_->position_at(
                    this->polygon_->target_of(eid)), vtx);
            }
            this->barriers_[fid] = segments;

//             std::cerr << "[World::prepair_barriers]: barriers = {";
//             for(std::size_t i=0; i<6; ++i)
//             {
//                 std::cerr << this->barriers_[fid][i] << ", ";
//             }
//             std::cerr << std::endl;
        }
        return;
    }

  private:

    boost::scoped_ptr<particle_space_type>   ps_;
    boost::shared_ptr<RandomNumberGenerator> rng_;
    boost::weak_ptr<Model>                   model_;
    boost::shared_ptr<polygon_type>          polygon_;
    structure_registrator_type               registrator_;
    particle_id_generator_type               pidgen_;

    // XXX consider moving this to the other place
    // this contains the edges that correspond to the developed neighbor faces.
    //
    //        /\
    //     > /__\ <
    //      /\* /\
    //   > /__\/__\ < these edges
    //      ^    ^
    boost::container::flat_map<FaceID, boost::array<ecell4::Segment, 6> > barriers_;
};


}// sgfrd
}// ecell4
#endif // ECELL4_SGFRD_WORLD
