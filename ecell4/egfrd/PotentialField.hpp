#ifndef POTENTIAL_FIELD_HPP
#define POTENTIAL_FIELD_HPP

#include <memory>
#include <ecell4/core/types.hpp>
#include <ecell4/core/Shape.hpp>
#include <ecell4/core/RandomNumberGenerator.hpp>

namespace ecell4
{
namespace egfrd
{

template <typename Tcontainer>
class PotentialField
{
public:

    typedef Tcontainer container_type;
    typedef RandomNumberGenerator rng_type;
    typedef std::pair<ParticleID, Particle> pid_particle_pair;

public:

    virtual ~PotentialField()
    {
        ;
    }

    virtual bool try_move(rng_type& rng, const pid_particle_pair& p, const Real3& newpos, const container_type& space)
    {
        return true;
    }
};

template <typename Tcontainer>
class ShapedHardbodyPotentialField
    : public PotentialField<Tcontainer>
{
public:

    typedef PotentialField<Tcontainer> base_type;
    typedef typename base_type::container_type container_type;
    typedef typename base_type::pid_particle_pair pid_particle_pair;
    typedef typename base_type::rng_type rng_type;

public:

    ShapedHardbodyPotentialField(const std::shared_ptr<Shape>& shape)
        : shape_(shape)
    {
        ;
    }

    bool try_move(rng_type& rng, const pid_particle_pair& p, const Real3& newpos, const container_type& space)
    {
        return (shape_->is_inside(newpos) <= 0.0);
    }

protected:

    std::shared_ptr<Shape> shape_;
};

template <typename Tcontainer>
class ShapedDiscretePotentialField
    : public PotentialField<Tcontainer>
{
public:

    typedef PotentialField<Tcontainer> base_type;
    typedef typename base_type::container_type container_type;
    typedef typename base_type::pid_particle_pair pid_particle_pair;
    typedef typename base_type::rng_type rng_type;

public:

    ShapedDiscretePotentialField(const std::shared_ptr<Shape>& shape, const Real& threshold)
        : shape_(shape), threshold_(threshold)
    {
        ;
    }

    bool try_move(rng_type& rng, const pid_particle_pair& p, const Real3& newpos, const container_type& space)
    {
        const Real3& oldpos(p.second.position());
        if (shape_->is_inside(newpos) <= 0.0 || shape_->is_inside(oldpos) > 0.0)
        {
            return true;
        }

        const Real rnd = rng.uniform(0.0, 1.0);
        return (rnd < threshold_);
    }

protected:

    std::shared_ptr<Shape> shape_;
    Real threshold_;
};

template <typename Tcontainer>
class LeashPotentialField
    : public PotentialField<Tcontainer>
{
public:

    typedef PotentialField<Tcontainer> base_type;
    typedef typename base_type::container_type container_type;
    typedef typename base_type::pid_particle_pair pid_particle_pair;
    typedef typename base_type::rng_type rng_type;

    typedef std::unordered_map<ParticleID, Real3> particle_id_position_map_type;

public:

    LeashPotentialField(const Real radius)
        : radius_(radius)
    {
        ;
    }

    bool try_move(rng_type& rng, const pid_particle_pair& p, const Real3& newpos, const container_type& space)
    {
        particle_id_position_map_type::const_iterator it = centers_.find(p.first);
        if (it == centers_.end())
        {
            centers_.insert(particle_id_position_map_type::value_type(p.first, p.second.position()));
            return (space.distance(p.second.position(), newpos) <= radius_);
        }
        return (space.distance((*it).second, newpos) <= radius_);
    }

protected:

    Real radius_;
    particle_id_position_map_type centers_;
};

} // egfrd
} // ecell4
#endif /* POTENTIAL_FIELD_HPP */
