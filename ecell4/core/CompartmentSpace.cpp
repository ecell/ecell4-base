#include <stdexcept>
#include <numeric>

#include "exceptions.hpp"
#include "functions.hpp"
#include "Context.hpp"
#include "CompartmentSpace.hpp"


namespace ecell4
{

const Real CompartmentSpaceVectorImpl::volume() const
{
    return volume_;
}

std::vector<Species> CompartmentSpaceVectorImpl::list_species() const
{
    return species_; // return a copy
}

bool CompartmentSpaceVectorImpl::has_species(const Species& sp) const
{
    return std::find(species_.begin(), species_.end(), sp) != species_.end();
}

void CompartmentSpaceVectorImpl::set_volume(const Real& volume)
{
    if (volume <= 0)
    {
        throw std::invalid_argument("The volume must be positive.");
    }

    volume_ = volume;
    const Real L(cbrt(volume));
    edge_lengths_ = Real3(L, L, L);
}

void CompartmentSpaceVectorImpl::reserve_species(const Species& sp)
{
    species_map_type::const_iterator i(index_map_.find(sp));
    if (i != index_map_.end())
    {
        throw AlreadyExists("Species already exists");
    }

    index_map_.insert(std::make_pair(sp, num_molecules_.size()));
    species_.push_back(sp);
    num_molecules_.push_back(0);
}

void CompartmentSpaceVectorImpl::release_species(const Species& sp)
{
    species_map_type::iterator i(index_map_.find(sp));
    if (i == index_map_.end())
    {
        std::ostringstream message;
        message << "Speices [" << sp.serial() << "] not found";
        throw NotFound(message.str()); // use boost::format if it's allowed
    }

    species_map_type::mapped_type
        idx((*i).second), last_idx(num_molecules_.size() - 1);
    if (idx != last_idx)
    {
        species_container_type::size_type const
            idx_(static_cast<species_container_type::size_type>(idx)),
            last_idx_(static_cast<species_container_type::size_type>(last_idx));
        const Species& last_sp(species_[last_idx_]);
        species_[idx_] = last_sp;
        num_molecules_[idx] = num_molecules_[last_idx];
        index_map_[last_sp] = idx;
    }

    species_.pop_back();
    num_molecules_.pop_back();
    index_map_.erase(sp);
}

Integer CompartmentSpaceVectorImpl::num_molecules(const Species& sp) const
{
    SpeciesExpressionMatcher sexp(sp);
    Integer retval(0);
    for (species_map_type::const_iterator i(index_map_.begin());
        i != index_map_.end(); ++i)
    {
        if (sexp.match((*i).first))
        {
            do
            {
                retval += num_molecules_[(*i).second];
            } while (sexp.next());
        }
    }
    return retval;
}

Integer CompartmentSpaceVectorImpl::num_molecules_exact(const Species& sp) const
{
    species_map_type::const_iterator i(index_map_.find(sp));
    if (i == index_map_.end())
    {
        // throw NotFound("Species not found");
        return 0;
    }
    return num_molecules_[(*i).second];
}

void CompartmentSpaceVectorImpl::add_molecules(
    const Species& sp, const Integer& num)
{
    if (num < 0)
    {
        std::ostringstream message;
        message << "The number of molecules must be positive. [" << sp.serial() << "]";
        throw std::invalid_argument(message.str());
    }

    species_map_type::const_iterator i(index_map_.find(sp));
    if (i == index_map_.end())
    {
        // throw NotFound("Species not found");
        reserve_species(sp);
        i = index_map_.find(sp);
    }

    num_molecules_[(*i).second] += num;
}

void CompartmentSpaceVectorImpl::remove_molecules(
    const Species& sp, const Integer& num)
{
    if (num < 0)
    {
        std::ostringstream message;
        message << "The number of molecules must be positive. [" << sp.serial() << "]";
        throw std::invalid_argument(message.str());
    }

    species_map_type::const_iterator i(index_map_.find(sp));
    if (i == index_map_.end())
    {
        std::ostringstream message;
        message << "Speices [" << sp.serial() << "] not found";
        throw NotFound(message.str()); // use boost::format if it's allowed
    }

    if (num_molecules_[(*i).second] < num)
    {
        std::ostringstream message;
        message << "The number of molecules cannot be negative. [" << sp.serial() << "]";
        throw std::invalid_argument(message.str());
    }

    num_molecules_[(*i).second] -= num;
}

} // ecell4
