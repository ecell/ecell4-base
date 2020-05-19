#ifndef ECELL4_EGFRD_SHAPED_DOMAIN_HPP
#define ECELL4_EGFRD_SHAPED_DOMAIN_HPP

#include "Domain.hpp"

namespace ecell4
{
namespace egfrd
{
template<typename Ttraits_>
class ShapedDomain: public Domain<Ttraits_>
{
public:
    typedef Ttraits_ traits_type;
    typedef typename traits_type::domain_id_type identifier_type;
    typedef typename traits_type::world_type::length_type length_type;
    typedef typename traits_type::world_type::traits_type::position_type position_type;
    typedef Domain<Ttraits_> base_type;

public:
    virtual ~ShapedDomain() {}

    virtual position_type const& position() const = 0;

    virtual position_type& position() = 0;

    virtual length_type const& size() const = 0;

    virtual length_type& size() = 0;

    ShapedDomain(identifier_type const& id)
        : base_type(id) {}
};

} // egfrd
} // ecell4
#endif /* SHAPED_DOMAIN_HPP */
