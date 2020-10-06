#ifndef ECELL4_NGFRD_DOMAIN_HPP
#define ECELL4_NGFRD_DOMAIN_HPP
#include <ecell4/ngfrd/MultiDomain.hpp>

namespace ecell4
{
namespace ngfrd
{

struct Domain
{
public:
    enum class DomainKind : int // boost::variant::which returns an int.
    {
        Uninitialized = 0,
        Multi = 1,
    };

    // to contain this in a map, we need to make it default constructible...
    using storage_type = boost::variant<boost::blank, MultiDomain>;

private:

    struct multiplicity_visitor : boost::static_visitor<std::size_t>
    {
        std::size_t operator()(const boost::blank&) const
        {
            throw_exception<IllegalState>("Domain is not initialized");
        }
        std::size_t operator()(const MultiDomain& dom) const noexcept
        {
            return dom.multiplicity();
        }
    };

public:

    Domain() noexcept: storage_(boost::blank{}) {}

    template<typename D>
    explicit Domain(D&& d): storage_(std::forward<D>(d)) {}

    DomainKind kind() const noexcept {return DomainKind(storage_.which());}

    bool is_multi() const noexcept {return this->kind() == DomainKind::Multi;}

    MultiDomain const& as_multi() const noexcept {return boost::get<MultiDomain>(storage_);}
    MultiDomain&       as_multi()       noexcept {return boost::get<MultiDomain>(storage_);}

    storage_type const& as_variant() const noexcept {return storage_;}
    storage_type&       as_variant()       noexcept {return storage_;}

    std::size_t multiplicity() const
    {
        return boost::apply_visitor(multiplicity_visitor(), storage_);
    }

private:

    storage_type storage_;
};

} //ngfrd
} //ecell4
#endif// ECELL4_NGFRD_DOMAIN_HPP