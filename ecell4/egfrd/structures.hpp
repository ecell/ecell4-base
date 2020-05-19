#ifndef ECELL4_EGFRD_STRUCTURES_HPP
#define ECELL4_EGFRD_STRUCTURES_HPP

#include <ecell4/core/RandomNumberGenerator.hpp>
#include <ecell4/core/AABB.hpp>

namespace ecell4
{
namespace egfrd
{

template<typename Ttraits_>
struct ImmutativeStructureVisitor
{
    typedef Ttraits_ traits_type;
    // typedef typename traits_type::spherical_surface_type spherical_surface_type;
    // typedef typename traits_type::cylindrical_surface_type cylindrical_surface_type;
    // typedef typename traits_type::planar_surface_type planar_surface_type;
    typedef typename traits_type::cuboidal_region_type cuboidal_region_type;

    virtual ~ImmutativeStructureVisitor() {}

    // virtual void operator()(spherical_surface_type const&) const = 0;
    // virtual void operator()(cylindrical_surface_type const&) const = 0;
    // virtual void operator()(planar_surface_type const&) const = 0;
    virtual void operator()(cuboidal_region_type const&) const = 0;
};

template<typename Ttraits_>
struct MutativeStructureVisitor
{
    typedef Ttraits_ traits_type;
    // typedef typename traits_type::spherical_surface_type spherical_surface_type;
    // typedef typename traits_type::cylindrical_surface_type cylindrical_surface_type;
    // typedef typename traits_type::planar_surface_type planar_surface_type;
    typedef typename traits_type::cuboidal_region_type cuboidal_region_type;

    virtual ~MutativeStructureVisitor() {}

    // virtual void operator()(spherical_surface_type&) const = 0;
    // virtual void operator()(cylindrical_surface_type&) const = 0;
    // virtual void operator()(planar_surface_type&) const = 0;
    virtual void operator()(cuboidal_region_type&) const = 0;
};

template<typename Ttraits_>
class Structure
{
public:

    typedef Ttraits_ traits_type;
    typedef typename traits_type::structure_id_type identifier_type;
    typedef typename traits_type::length_type length_type;
    typedef typename traits_type::position_type position_type;
    typedef RandomNumberGenerator rng_type;

public:

    Structure(const identifier_type& id)
        : id_(id)
    {
        ;
    }

    virtual ~Structure()
    {
        ; // do nothing
    }

    const identifier_type& id() const
    {
        return id_;
    }

    virtual position_type random_vector(length_type const& r, rng_type& rng) const = 0;
    virtual position_type bd_displacement(length_type const& r, rng_type& rng) const = 0;

    virtual void accept(ImmutativeStructureVisitor<traits_type> const& visitor) const = 0;
    // virtual void accept(MutativeStructureVisitor<traits_type> const& visitor) = 0;

protected:

    identifier_type id_;
};

template<typename Ttraits_, typename Tshape_>
class ShapedStructure
    : public Structure<Ttraits_>
{
public:

    typedef Structure<Ttraits_> base_type;
    typedef Tshape_ shape_type;

    typedef typename base_type::traits_type traits_type;
    typedef typename base_type::identifier_type identifier_type;
    typedef typename base_type::length_type length_type;
    typedef typename base_type::position_type position_type;
    typedef typename base_type::rng_type rng_type;

public:

    ShapedStructure(const identifier_type& id, const shape_type& shape)
        : base_type(id), shape_(shape)
    {
        ;
    }

    shape_type& shape()
    {
        return shape_;
    }

    const shape_type& shape() const
    {
        return shape_;
    }

protected:

    shape_type shape_;
};

template<typename Ttraits_>
class AABBRegion
    : public ShapedStructure<Ttraits_, AABB> //XXX
{
public:

    typedef ShapedStructure<Ttraits_, AABB> base_type;

    typedef typename base_type::traits_type traits_type;
    typedef typename base_type::shape_type shape_type;
    typedef typename base_type::identifier_type identifier_type;
    typedef typename base_type::length_type length_type;
    typedef typename base_type::position_type position_type;
    typedef typename base_type::rng_type rng_type;

public:

    AABBRegion(const identifier_type& id, const shape_type& shape)
        : base_type(id, shape)
    {
        ;
    }

    virtual ~AABBRegion()
    {
        ;
    }

    virtual position_type random_vector(length_type const& r, rng_type& rng) const
    {
        // return rng.direction3d(r);
        return normalize(
            create_vector<position_type>(
                rng.uniform(-1., 1.), rng.uniform(-1., 1.), rng.uniform(-1., 1.)),
            r);
    }

    virtual position_type bd_displacement(length_type const& r, rng_type& rng) const
    {
        return create_vector<position_type>(
            rng.gaussian(r), rng.gaussian(r), rng.gaussian(r));
    }

    virtual void accept(ImmutativeStructureVisitor<traits_type> const& visitor) const
    {
        visitor(*this);
    }

    // virtual void accept(MutativeStructureVisitor<traits_type> const& visitor)
    // {
    //     visitor(*this);
    // }
};

} // egfrd
} // ecell4

#endif /* ECELL4_STRUCTURES_HPP */
