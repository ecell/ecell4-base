#ifndef ECELL4_SPECIES_HPP
#define ECELL4_SPECIES_HPP

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/variant.hpp>
#include <boost/container/flat_map.hpp>
// #include <boost/container/small_vector.hpp>

#include <ecell4/core/config.h>

#include "hash.hpp"
#include "get_mapper_mf.hpp"
#include "types.hpp"
#include "exceptions.hpp"
#include "UnitSpecies.hpp"
// #include "Context.hpp"
#include "Quantity.hpp"


namespace ecell4
{

class Species
{
public:

    typedef UnitSpecies::serial_type serial_type; //XXX: std::string
    typedef std::vector<UnitSpecies> container_type;

    typedef boost::variant<std::string, Quantity<Real>, Quantity<Integer>, bool> attribute_type;

protected:

    typedef boost::container::flat_map<std::string, attribute_type>
        attributes_container_type;

    // typedef boost::container::small_vector<
    //     std::pair<std::string, attribute_type>, 3
    //         > flat_map_backend_type;
    // typedef boost::container::flat_map<
    //     std::string, attribute_type, std::less<std::string>, flat_map_backend_type
    //         > attributes_container_type;

public:

    Species();
    explicit Species(const serial_type& name);
    Species(const Species& another);
    Species& operator=(const Species& another);
    Species(const serial_type& name, const Real& radius, const Real& D,
            const std::string location = "", const Integer& dimension = 0);
    Species(const serial_type& name, const Quantity<Real>& radius, const Quantity<Real>& D,
            const std::string location = "", const Integer& dimension = 0);

    const serial_type serial() const;

    void add_unit(const UnitSpecies& usp);
    const std::vector<UnitSpecies> units() const;

    const attributes_container_type& attributes() const;
    std::vector<std::pair<std::string, attribute_type> > list_attributes() const;
    attribute_type get_attribute(const std::string& name_attr) const;

    template <typename T_>
    T_ get_attribute_as(const std::string& name_attr) const
    {
        attribute_type val = get_attribute(name_attr);
        if (T_* x = boost::get<T_>(&val))
        {
            return (*x);
        }
        throw NotSupported("An attribute has incorrect type.");
    }

    template <typename T_>
    void set_attribute(const std::string& name_attr, T_ value)
    {
        attributes_[name_attr] = value;
    }

    void set_attributes(const Species& sp);
    void remove_attribute(const std::string& name_attr);
    bool has_attribute(const std::string& name_attr) const;
    void overwrite_attributes(const Species& sp);

    bool operator==(const Species& rhs) const;
    bool operator!=(const Species& rhs) const;
    bool operator<(const Species& rhs) const;
    bool operator>(const Species& rhs) const;

    Integer count(const Species& sp) const;

    /** Method chaining
     */

    Species& D(const std::string& value);
    Species* D_ptr(const std::string& value);
    Species& radius(const std::string& value);
    Species* radius_ptr(const std::string& value);
    Species& location(const std::string& value);
    Species* location_ptr(const std::string& value);
    Species& dimension(const std::string& value);
    Species* dimension_ptr(const std::string& value);

    /** for epdp
     */
    serial_type name() const;

protected:

    serial_type serial_;
    attributes_container_type attributes_;
};

template <> Real Species::get_attribute_as<Real>(const std::string& name_attr) const;
template <> Integer Species::get_attribute_as<Integer>(const std::string& name_attr) const;
template <> void Species::set_attribute<const char*>(const std::string& name_attr, const char* value);
template <> void Species::set_attribute<Real>(const std::string& name_attr, const Real value);
template <> void Species::set_attribute<Integer>(const std::string& name_attr, const Integer value);

template<typename Tstrm_, typename Ttraits_>
inline std::basic_ostream<Tstrm_, Ttraits_>& operator<<(
    std::basic_ostream<Tstrm_, Ttraits_>& strm,
    const ecell4::Species& sp)
{
    strm << sp.serial();
    return strm;
}

} // ecell4

ECELL4_DEFINE_HASH_BEGIN()

template<>
struct hash<ecell4::Species>
{
    std::size_t operator()(const ecell4::Species& val) const
    {
        return hash<ecell4::Species::serial_type>()(val.serial());
    }
};

ECELL4_DEFINE_HASH_END()

#endif /* ECELL4_SPECIES_HPP */
