#ifndef ECELL4_MOLECULAR_TYPE_BASE_HPP
#define ECELL4_MOLECULAR_TYPE_BASE_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "Species.hpp"
#include "Shape.hpp"
#include "Identifier.hpp"
#include "RandomNumberGenerator.hpp"
#include "ParticleVoxel.hpp"


namespace ecell4
{

class VoxelPool
{
public:

    typedef ParticleVoxel::coordinate_type coordinate_type;

    // typedef std::pair<coordinate_type, ParticleID> coordinate_id_pair_type;
    typedef struct coordinate_id_pair_type
        {
            coordinate_id_pair_type(ParticleID const& pid, coordinate_type const& coordinate)
                : coordinate(coordinate), pid(pid)
            {}

            coordinate_type coordinate;
            ParticleID pid;

            bool operator==(const coordinate_id_pair_type& rhs) const
            {
                return pid == rhs.pid &&
                    coordinate == rhs.coordinate;
            }

            bool operator!=(const coordinate_id_pair_type& rhs) const
            {
                return pid != rhs.pid
                    || coordinate != rhs.coordinate;
            }

            bool operator<(const coordinate_id_pair_type& rhs) const
            {
                return coordinate < rhs.coordinate
                    || (coordinate == rhs.coordinate &&
                        pid < rhs.pid);
            }

            bool operator>=(const coordinate_id_pair_type& rhs) const
            {
                return coordinate > rhs.coordinate
                    || (coordinate == rhs.coordinate &&
                        pid >= rhs.pid);
            }

            bool operator>(const coordinate_id_pair_type& rhs) const
            {
                return coordinate > rhs.coordinate
                    || (coordinate == rhs.coordinate &&
                        pid > rhs.pid);
            }

            bool operator<=(const coordinate_id_pair_type& rhs) const
            {
                return coordinate < rhs.coordinate
                    || (coordinate == rhs.coordinate &&
                        pid <= rhs.pid);
            }
        } coordinate_id_pair_type;

public:

    typedef enum
    {
        DEFAULT,
        VACANT,
        STRUCTURE,
        INTERFACE
    } voxel_type_type;

public:

    VoxelPool(
        const Species& species, boost::weak_ptr<VoxelPool> location,
        const Real& radius, const Real& D)
        : species_(species), location_(location),
        radius_(radius), D_(D)
    {
        ;
    }


    virtual ~VoxelPool()
    {
        ;
    }

    virtual voxel_type_type const voxel_type() const = 0;

    virtual const Shape::dimension_kind get_dimension() const
    {
        return Shape::UNDEF;
    }

public:

    bool is_vacant() const
    {
        return voxel_type() == VACANT;
    }

    bool is_structure() const
    {
        return voxel_type() == STRUCTURE;
    }

    bool is_interface() const
    {
        return voxel_type() == INTERFACE;
    }

    const Species& species() const
    {
        return species_;
    }

    boost::shared_ptr<VoxelPool> location() const
    {
        return location_.lock();
    }

    Real& radius()
    {
        return radius_;
    }

    const Real& radius() const
    {
        return radius_;
    }

    Real& D()
    {
        return D_;
    }

    const Real& D() const
    {
        return D_;
    }

public:

    virtual const Integer size() const = 0;

    virtual void add_voxel(const coordinate_id_pair_type& info) = 0;

    virtual void replace_voxel(
        const coordinate_type& from_coord, const coordinate_type& to_coord,
        const std::size_t candidate = 0)
    {
        ; // do nothing
    }

    virtual coordinate_id_pair_type pop(const coordinate_type& coord) = 0;

    virtual bool remove_voxel_if_exists(const coordinate_type& coord) = 0;

    virtual const ParticleID get_particle_id(const coordinate_type& coord) const
    {
        return ParticleID();
    }

protected:

    const Species species_;
    boost::weak_ptr<VoxelPool> location_;
    Real radius_, D_;
};

} // ecell4

#endif
