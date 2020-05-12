#define BOOST_TEST_MODULE "BDSimulator_test"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#   include <boost/test/unit_test.hpp>
#else
#   define BOOST_TEST_NO_LIB
#   include <boost/test/included/unit_test.hpp>
#endif

#include <ecell4/core/NetworkModel.hpp>
#include "../BDSimulator.hpp"

using namespace ecell4;
using namespace ecell4::bd;


BOOST_AUTO_TEST_CASE(BDSimulator_test_constructor)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Integer3 matrix_sizes(3, 3, 3);
    std::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator());

    std::shared_ptr<NetworkModel> model(new NetworkModel());
    std::shared_ptr<BDWorld> world(new BDWorld(edge_lengths, matrix_sizes, rng));

    BDSimulator target(world, model);
}

BOOST_AUTO_TEST_CASE(BDSimulator_test_step1)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Integer3 matrix_sizes(3, 3, 3);
    std::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator());

    std::shared_ptr<NetworkModel> model(new NetworkModel());
    std::shared_ptr<BDWorld> world(new BDWorld(edge_lengths, matrix_sizes, rng));

    BDSimulator target(world, model);
    target.step();
}

BOOST_AUTO_TEST_CASE(BDSimulator_test_step2)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Integer3 matrix_sizes(3, 3, 3);
    std::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator());

    std::shared_ptr<NetworkModel> model(new NetworkModel());
    Species sp1("A", 2.5e-9, 1e-12);
    model->add_species_attribute(sp1);

    std::shared_ptr<BDWorld> world(new BDWorld(edge_lengths, matrix_sizes, rng));
    world->new_particle(Particle(sp1, Real3(0, 0, 0), 2.5e-9, 1e-12));
    world->add_molecules(sp1, 10);

    BDSimulator target(world, model);
    target.step();
}
