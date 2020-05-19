#define BOOST_TEST_MODULE "CompartmentSpace_test"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#   include <boost/test/unit_test.hpp>
#else
#   define BOOST_TEST_NO_LIB
#   include <boost/test/included/unit_test.hpp>
#endif

// #include "../types.hpp"
// #include "../CompartmentSpace.hpp"
#include <ecell4/core/types.hpp>
#include <ecell4/core/CompartmentSpace.hpp>

using namespace ecell4;

template<typename Timpl_>
void CompartmentSpace_test_volume_template()
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    Timpl_ target(edge_lengths);
    const Real new_volume(2 * target.volume());
    target.set_volume(new_volume);
    BOOST_CHECK_EQUAL(target.volume(), new_volume);
}

BOOST_AUTO_TEST_CASE(CompartmentSpace_test_volume)
{
    CompartmentSpace_test_volume_template<CompartmentSpaceVectorImpl>();
}

template<typename Timpl_>
void CompartmentSpace_test_species_template()
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    Timpl_ target(edge_lengths);

    Species sp1("A"), sp2("B"), sp3("C");
    // target.add_species(sp1);
    // target.add_species(sp2);
    // BOOST_CHECK(target.has_species(sp1));
    // BOOST_CHECK(target.has_species(sp2));
    // BOOST_CHECK(!target.has_species(sp3));
    // target.add_species(sp3);
    // BOOST_CHECK(target.has_species(sp3));
    // BOOST_CHECK(target.num_species() == 3);

    // target.remove_species(sp2);
    // BOOST_CHECK(!target.has_species(sp2));
    // BOOST_CHECK_THROW(target.remove_species(sp2), NotFound);
    // BOOST_CHECK(target.has_species(sp1));
    // BOOST_CHECK(target.has_species(sp3));

    BOOST_CHECK(target.num_molecules(sp1) == 0);
    target.add_molecules(sp1, 30);
    BOOST_CHECK(target.num_molecules(sp1) == 30);
    target.remove_molecules(sp1, 10);
    BOOST_CHECK(target.num_molecules(sp1) == 20);
}

BOOST_AUTO_TEST_CASE(CompartmentSpace_test_species)
{
    CompartmentSpace_test_species_template<CompartmentSpaceVectorImpl>();
}
