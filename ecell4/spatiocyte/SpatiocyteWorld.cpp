#include <stdexcept>
#include <fstream>

#include "SpatiocyteWorld.hpp"

namespace ecell4
{

namespace spatiocyte
{

SpatiocyteWorld* create_spatiocyte_world_cell_list_impl(
    const Real3& edge_lengths, const Real& voxel_radius,
    const Integer3& matrix_sizes,
    const boost::shared_ptr<RandomNumberGenerator>& rng)
{
    return new SpatiocyteWorld(
        new LatticeSpaceCellListImpl(edge_lengths, voxel_radius, matrix_sizes), rng);
}

SpatiocyteWorld* create_spatiocyte_world_vector_impl(
    const Real3& edge_lengths, const Real& voxel_radius,
    const boost::shared_ptr<RandomNumberGenerator>& rng)
{
    return new SpatiocyteWorld(
        new LatticeSpaceVectorImpl(edge_lengths, voxel_radius), rng);
}

const Real SpatiocyteWorld::t() const
{
    return (*space_).t();
}

void SpatiocyteWorld::set_t(const Real& t)
{
    (*space_).set_t(t);
}

const Real3& SpatiocyteWorld::edge_lengths() const
{
    return (*space_).edge_lengths();
}

const Real SpatiocyteWorld::volume() const
{
    return (*space_).volume();
}

Integer SpatiocyteWorld::num_species() const
{
    return (*space_).num_species();
}

void SpatiocyteWorld::set_value(const Species& sp, const Real value)
{
    const Integer num1 = static_cast<Integer>(value);
    const Integer num2 = num_molecules_exact(sp);
    if (num1 > num2)
    {
        add_molecules(sp, num1 - num2);
    }
    else if (num1 < num2)
    {
        remove_molecules(sp, num2 - num1);
    }
}

bool SpatiocyteWorld::has_species(const Species &sp) const
{
    return (*space_).has_species(sp);
}

// bool SpatiocyteWorld::has_species_exact(const Species &sp) const
// {
//     return (*space_).has_species_exact(sp);
// }

Integer SpatiocyteWorld::num_molecules(const Species& sp) const
{
    return (*space_).num_molecules(sp);
}

Integer SpatiocyteWorld::num_molecules_exact(const Species& sp) const
{
    return (*space_).num_molecules_exact(sp);
}

Integer SpatiocyteWorld::num_particles(const Species& sp) const
{
    return (*space_).num_particles(sp);
}

Integer SpatiocyteWorld::num_particles_exact(const Species& sp) const
{
    return (*space_).num_particles_exact(sp);
}

Integer SpatiocyteWorld::num_particles() const
{
    return (*space_).num_particles();
}

Integer SpatiocyteWorld::num_voxels() const
{
    return (*space_).num_voxels();
}

Integer SpatiocyteWorld::num_voxels(const Species& sp) const
{
    return (*space_).num_voxels(sp);
}

Integer SpatiocyteWorld::num_voxels_exact(const Species& sp) const
{
    return (*space_).num_voxels_exact(sp);
}

bool SpatiocyteWorld::has_particle(const ParticleID& pid) const
{
    return (*space_).has_particle(pid);
}

bool SpatiocyteWorld::has_voxel(const ParticleID& pid) const
{
    return (*space_).has_voxel(pid);
}

std::vector<std::pair<ParticleID, Particle> >
SpatiocyteWorld::list_particles() const
{
    return (*space_).list_particles();
}

std::vector<std::pair<ParticleID, Particle> >
SpatiocyteWorld::list_particles(const Species& sp) const
{
    return (*space_).list_particles(sp);
}

std::vector<std::pair<ParticleID, Particle> >
SpatiocyteWorld::list_particles_exact(const Species& sp) const
{
    return (*space_).list_particles_exact(sp);
}

std::vector<std::pair<ParticleID, Particle> >
SpatiocyteWorld::list_structure_particles() const
{
    const std::vector<Species> structure_species(list_structure_species());

    typedef std::vector<std::vector<std::pair<ParticleID, Particle> > > tmp_type;
    tmp_type tmp_vector(structure_species.size());
    Integer num_elements;

    for (std::vector<Species>::const_iterator itr(structure_species.begin());
            itr != structure_species.end(); ++itr)
    {
        std::vector<std::pair<ParticleID, Particle> > tmp(list_particles(*itr));
        tmp_vector.push_back(tmp);
        num_elements += tmp.size();
    }

    std::vector<std::pair<ParticleID, Particle> > retval;
    retval.reserve(num_elements);
    for (tmp_type::const_iterator itr(tmp_vector.begin());
            itr != tmp_vector.end(); ++itr)
    {
        retval.insert(retval.end(), (*itr).begin(), (*itr).end());
    }

    return retval;
}

std::vector<std::pair<ParticleID, Particle> >
SpatiocyteWorld::list_non_structure_particles() const
{
    const std::vector<Species> non_structure_species(list_non_structure_species());

    typedef std::vector<std::vector<std::pair<ParticleID, Particle> > > tmp_type;
    tmp_type tmp_vector(non_structure_species.size());
    Integer num_elements;

    for (std::vector<Species>::const_iterator itr(non_structure_species.begin());
            itr != non_structure_species.end(); ++itr)
    {
        std::vector<std::pair<ParticleID, Particle> > tmp(list_particles(*itr));
        tmp_vector.push_back(tmp);
        num_elements += tmp.size();
    }

    std::vector<std::pair<ParticleID, Particle> > retval;
    retval.reserve(num_elements);
    for (tmp_type::const_iterator itr(tmp_vector.begin());
            itr != tmp_vector.end(); ++itr)
    {
        retval.insert(retval.end(), (*itr).begin(), (*itr).end());
    }

    return retval;
}

std::vector<Species> SpatiocyteWorld::list_species() const
{
    return (*space_).list_species();
}

std::vector<Species> SpatiocyteWorld::list_non_structure_species() const
{
    const std::vector<Species> species(list_species());
    std::vector<Species> retval;
    for (std::vector<Species>::const_iterator itr(species.begin());
            itr != species.end(); ++itr)
    {
        if (!find_voxel_pool(*itr)->is_structure())
            retval.push_back(*itr);
    }
    return retval;
}

std::vector<Species> SpatiocyteWorld::list_structure_species() const
{
    const std::vector<Species> species(list_species());
    std::vector<Species> retval;
    for (std::vector<Species>::const_iterator itr(species.begin());
            itr != species.end(); ++itr)
    {
        if (find_voxel_pool(*itr)->is_structure())
            retval.push_back(*itr);
    }
    return retval;
}

std::vector<std::pair<ParticleID, Voxel> > SpatiocyteWorld::list_voxels() const
{
    return (*space_).list_voxels();
}

std::vector<std::pair<ParticleID, Voxel> >
    SpatiocyteWorld::list_voxels(const Species& sp) const
{
    return (*space_).list_voxels(sp);
}

std::vector<std::pair<ParticleID, Voxel> >
    SpatiocyteWorld::list_voxels_exact(const Species& sp) const
{
    return (*space_).list_voxels_exact(sp);
}

VoxelPool* SpatiocyteWorld::find_voxel_pool(const Species& species)
{
    return (*space_).find_voxel_pool(species);
}

const VoxelPool* SpatiocyteWorld::find_voxel_pool(const Species& species) const
{
    return (*space_).find_voxel_pool(species);
}

VoxelPool* SpatiocyteWorld::get_voxel_pool_at(const coordinate_type& coord) const
{
    return (*space_).get_voxel_pool_at(coord);
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel(
        const Species& sp, const coordinate_type& coord)
{
    const molecule_info_type minfo(get_molecule_info(sp));
    return new_voxel(
        Voxel(sp, coord, minfo.radius, minfo.D, minfo.loc));
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel(const Voxel& v)
{
    ParticleID pid(sidgen_());
    const bool is_succeeded(update_voxel(pid, v));
    return std::make_pair(std::make_pair(pid, v), is_succeeded);
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel_structure(const Species& sp, const coordinate_type& coord)
{
    const molecule_info_type minfo(get_molecule_info(sp));
    return new_voxel_structure(
        Voxel(sp, coord, minfo.radius, minfo.D, minfo.loc));
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel_structure(const Voxel& v)
{
    const bool is_succeeded(update_voxel(ParticleID(), v));
    return std::make_pair(std::make_pair(ParticleID(), v), is_succeeded);
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel_interface(const Species& sp, const coordinate_type& coord)
{
    const molecule_info_type minfo(get_molecule_info(sp));
    return new_voxel_interface(
        Voxel(sp, coord, minfo.radius, minfo.D, minfo.loc));
}

std::pair<std::pair<ParticleID, Voxel>, bool>
SpatiocyteWorld::new_voxel_interface(const Voxel& v)
{
    const bool is_succeeded(update_voxel(ParticleID(), v));
    return std::make_pair(std::make_pair(ParticleID(), v), is_succeeded);
}

bool SpatiocyteWorld::add_molecules(const Species& sp, const Integer& num)
{
    if (num < 0)
    {
        throw std::invalid_argument("The number of molecules must be positive.");
    }

    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));

    Integer count(0);
    while (count < num)
    {
        const coordinate_type coord(inner2coordinate(rng()->uniform_int(0, inner_size() - 1)));
        //XXX: just for consistency. rather use below
        // const coordinate_type coord(rng()->uniform_int(0, size() - 1));

        const Voxel v(sp, coord, info.radius, info.D, info.loc);

        if (on_structure(v))
        {
            continue;
        }
        else if (new_voxel(v).second)
        {
            ++count;
        }
    }
    return true;
}

bool SpatiocyteWorld::add_molecules(
    const Species& sp, const Integer& num, const boost::shared_ptr<const Shape> shape)
{
    if (num < 0)
    {
        throw std::invalid_argument("The number of molecules must be positive.");
    }

    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));

    Integer count(0);
    while (count < num)
    {
        const Real3 pos(shape->draw_position(rng_));
        const Voxel v(sp, position2coordinate(pos), info.radius, info.D, info.loc);

        if (on_structure(v))
        {
            continue;
        }
        else if (new_voxel(v).second)
        {
            ++count;
        }
    }
    return true;
}

Integer SpatiocyteWorld::add_structure(
    const Species& sp, const boost::shared_ptr<const Shape> shape)
{
    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    (*space_).make_structure_type(sp, shape->dimension(), info.loc);

    switch (shape->dimension())
    {
    case Shape::THREE:
        return add_structure3(sp, shape);
    case Shape::TWO:
        return add_structure2(sp, shape);
    case Shape::ONE:
    case Shape::UNDEF:
        break;
    }

    throw NotSupported("The dimension of a shape must be two or three.");
}

Integer SpatiocyteWorld::add_structure3(const Species& sp, const boost::shared_ptr<const Shape> shape)
{
    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    Integer count(0);
    for (coordinate_type inner(0); inner < inner_size(); ++inner) {
        const coordinate_type coord(inner2coordinate(inner));
        const Real L(shape->is_inside(coordinate2position(coord)));
        if (L > 0)
            continue;

        const Voxel v(sp, coord, info.radius, info.D, info.loc);
        if (new_voxel_structure(v).second)
            ++count;
    }
    return count;
}

Integer SpatiocyteWorld::add_structure2(const Species& sp, const boost::shared_ptr<const Shape> shape)
{
    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    Integer count(0);
    for (coordinate_type inner(0); inner < inner_size(); ++inner) {
        const coordinate_type coord(inner2coordinate(inner));
        if (!is_surface_voxel(coord, shape))
            continue;

        const Voxel v(sp, coord, info.radius, info.D, info.loc);
        if (new_voxel_structure(v).second)
            ++count;
    }
    return count;
}

Integer SpatiocyteWorld::add_interface(const Species& sp)
{
    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    (*space_).make_interface_type(sp, Shape::UNDEF, info.loc);  //XXX: set the dimension properly
}

bool SpatiocyteWorld::is_surface_voxel(
    const coordinate_type coord, const boost::shared_ptr<const Shape> shape) const
{
    const Real L(shape->is_inside(coordinate2position(coord)));
    if (L > 0 || L < -2 * voxel_radius())
        return false;

    for (Integer i(0); i < 12; ++i)
        if (shape->is_inside(coordinate2position(get_neighbor(coord, i))) > 0)
            return true;

    return false;
}

// TODO
Integer SpatiocyteWorld::add_neighbors(const Species& sp,
    const SpatiocyteWorld::coordinate_type center)
{
    Integer count(0);
    const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    for (Integer i(0); i < 12; ++i)
    {
        const coordinate_type n(get_neighbor(center, i));
        if (new_voxel(Voxel(sp, n, info.radius, info.D, info.loc)).second)
        {
            ++count;
        }
        else
        {
            throw "Error in add_neighbors()";
        }
    }
    return count;

    // Integer count(0);
    // const SpatiocyteWorld::molecule_info_type info(get_molecule_info(sp));
    // std::vector<SpatiocyteWorld::coordinate_type> neighbors(
    //         get_neighbors(center));
    // for (std::vector<SpatiocyteWorld::coordinate_type>::iterator itr(
    //             neighbors.begin()); itr != neighbors.end(); itr++)
    // {
    //     if (new_voxel(Voxel(sp, *itr, info.radius, info.D, info.loc)).second)
    //         ++count;
    //     else
    //         throw "Error in add_neighbors()";
    // }
    // return count;
}
// TODO

void SpatiocyteWorld::remove_molecules(const Species& sp, const Integer& num)
{
    if (num < 0)
    {
        throw std::invalid_argument("The number of molecules must be positive.");
    }

    const MoleculePool* mtype(find_molecule_pool(sp));
    if (mtype->size() < num)
    {
        throw std::invalid_argument(
            "The number of molecules cannot be negative.");
    }

    Integer count(0);
    while (count < num)
    {
        const Integer idx(rng_->uniform_int(0, mtype->size() - 1));
        if (remove_voxel(mtype->at(idx).coordinate))
        {
            ++count;
        }
    }
}

bool SpatiocyteWorld::remove_voxel(const coordinate_type coord)
{
    return (*space_).remove_voxel(coord);
}

bool SpatiocyteWorld::move(
    const coordinate_type& src, const coordinate_type& dest, const std::size_t candidate)
{
    return (*space_).move(src, dest, candidate);
}

bool SpatiocyteWorld::can_move(const coordinate_type& src,
        const coordinate_type& dest) const
{
    return (*space_).can_move(src, dest);
}

std::pair<SpatiocyteWorld::coordinate_type, bool>
SpatiocyteWorld::move_to_neighbor(
    VoxelPool* const& from_mt, VoxelPool* const& loc,
    coordinate_id_pair_type& info, const Integer nrand)
{
    return (*space_).move_to_neighbor(from_mt, loc, info, nrand);
}

std::pair<SpatiocyteWorld::coordinate_type, bool>
SpatiocyteWorld::check_neighbor(
    const coordinate_type coord, const std::string& loc)
{
    std::vector<coordinate_type> tmp;
    tmp.reserve(12);
    for (unsigned int rnd(0); rnd < 12; ++rnd)
    {
        const coordinate_type neighbor(get_neighbor(coord, rnd));
        const VoxelPool* mt(get_voxel_pool_at(neighbor));
        const std::string
            serial(mt->is_vacant() ? "" : mt->species().serial());
        if (serial == loc)
        {
            tmp.push_back(neighbor);
        }
    }

    if (tmp.size() == 0)
    {
        return std::make_pair(coord, false);
    }

    return std::make_pair(
        tmp[rng()->uniform_int(0, tmp.size() - 1)], true);

    // const Integer rnd(rng()->uniform_int(0, 11));
    // const coordinate_type neighbor(get_neighbor(coord, rnd));
    // bool flg = find_voxel_pool(neighbor)->is_vacant(); //XXX: loc
    // return std::make_pair(neighbor, flg);
}

} // spatiocyte

} // ecell4
