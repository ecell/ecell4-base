#define BOOST_TEST_MODULE "SpatiocyteSimulator_test"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#   include <boost/test/unit_test.hpp>
#else
#   define BOOST_TEST_NO_LIB
#   include <boost/test/included/unit_test.hpp>
#endif

#include <boost/test/floating_point_comparison.hpp>

#include <ecell4/core/NetworkModel.hpp>
#include "../SpatiocyteSimulator.hpp"
#include <ecell4/core/Sphere.hpp>

using namespace ecell4;
using namespace ecell4::spatiocyte;

const Real DEFAULT_VOXEL_RADIUS = 1e-8;

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_constructor)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(DEFAULT_VOXEL_RADIUS);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp1("A", radius, D),
        sp2("B", radius, D),
        sp3("C", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp1);
    (*model).add_species_attribute(sp2);
    (*model).add_species_attribute(sp3);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_hdf5_save)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(DEFAULT_VOXEL_RADIUS);
    const Integer N(60);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    world->add_molecules(sp, N);
    BOOST_ASSERT(world->num_molecules(sp) == N);

    SpatiocyteSimulator sim(model, world);
#ifdef WITH_HDF5
    world->save("data.h5");
#endif
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_step_with_single_particle)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    BOOST_CHECK(world->new_voxel(sp, world->inner2coordinate(36)).second);

    SpatiocyteSimulator sim(model, world);

    const std::string hdf5path("/");

    for (int i(0); i < 50; ++i)
    {
        std::ostringstream oss;
        oss << "data_with_single_particle_";
        if (i < 10)
        {
            oss << "0" << i;
        }
        else
        {
            oss << i;
        }
        oss << ".h5";
        sim.step();
#ifdef WITH_HDF5
        world->save(oss.str());
#endif
    }
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_step_with_single_species)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const Integer N(60);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    world->add_molecules(sp, N / 2);

    BOOST_ASSERT(world->num_molecules(sp) == N / 2);

    SpatiocyteSimulator sim(model, world);

    world->add_molecules(sp, N / 2);
    BOOST_ASSERT(world->num_molecules(sp) == N);

    sim.initialize();
    sim.step();
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_save_step_with_single_species)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const Integer N(60);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    world->add_molecules(sp, N);
    sim.initialize();

    const std::string hdf5path("/");

    for (int i(0); i < 50; ++i)
    {
        std::ostringstream oss;
        oss << "data_with_single_species_";
        if (i < 10)
        {
            oss << "0" << i;
        }
        else
        {
            oss << i;
        }
        oss << ".h5";
        sim.step();
#ifdef WITH_HDF5
        world->save(oss.str());
#endif
    }
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_save_step_with_periodic)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const Integer N(60);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    world->add_molecules(sp, N);
    sim.initialize();

    const std::string hdf5path("/");

    for (int i(0); i < 50; ++i)
    {
        std::ostringstream oss;
        oss << "data_with_single_species_";
        if (i < 10)
        {
            oss << "0" << i;
        }
        else
        {
            oss << i;
        }
        oss << ".h5";
        sim.step();
#ifdef WITH_HDF5
        world->save(oss.str());
#endif
    }
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_unimolecular_reaction)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const std::string radius("1.25e-9");
    const ecell4::Species sp1("A", radius, "1.0e-12"),
          sp2("B", radius, "1.1e-12"),
          sp3("C", "2.5e-9", "1.2e-12");

    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    model->add_species_attribute(sp1);
    model->add_species_attribute(sp2);
    model->add_species_attribute(sp3);

    model->add_reaction_rule(create_unimolecular_reaction_rule(sp1,sp3,1e6));

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    BOOST_CHECK(world->add_molecules(sp1, 25));
    BOOST_CHECK(world->add_molecules(sp2, 25));
    sim.initialize();

#ifdef WITH_HDF5
    world->save("data_unimolecular_reaction_single0.h5");
#endif
    for (Integer i(0); i < 10; ++i)
    {
        sim.step();
    }
    BOOST_ASSERT(world->num_molecules(sp3) > 0);
    BOOST_ASSERT(25 - world->num_molecules(sp1) == world->num_molecules(sp3));
#ifdef WITH_HDF5
    world->save("data_unimolecular_reaction_single1.h5");
#endif
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_binding_reaction)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const std::string radius("1.25e-9");
    const ecell4::Species sp1("A", radius, "1.0e-12"),
          sp2("B", radius, "1.1e-12"),
          sp3("C", "2.5e-9", "1.2e-12");

    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    model->add_species_attribute(sp1);
    model->add_species_attribute(sp2);
    model->add_species_attribute(sp3);

    model->add_reaction_rule(create_binding_reaction_rule(sp1,sp2,sp3,1e-20));

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    BOOST_CHECK(world->add_molecules(sp1, 25));
    BOOST_CHECK(world->add_molecules(sp2, 25));
    sim.initialize();

#ifdef WITH_HDF5
    world->save("data_binging_reaction0.h5");
#endif
    for (Integer i(0); i < 20; ++i)
    {
        sim.step();
    }
#ifdef WITH_HDF5
    world->save("data_binding_reaction1.h5");
#endif
    Integer num_sp3(world->num_molecules(sp3));
    BOOST_ASSERT(num_sp3 > 0);
    BOOST_CHECK_EQUAL(25 - world->num_molecules(sp1), num_sp3);
    BOOST_CHECK_EQUAL(25 - world->num_molecules(sp2), num_sp3);
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_unbinding_reaction)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const std::string radius("1.25e-9");
    const ecell4::Species sp1("A", radius, "1.0e-12"),
          sp2("B", radius, "1.1e-12"),
          sp3("C", "2.5e-9", "1.2e-12");

    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    model->add_species_attribute(sp1);
    model->add_species_attribute(sp2);
    model->add_species_attribute(sp3);

    model->add_reaction_rule(create_unbinding_reaction_rule(sp1,sp2,sp3,1e5));

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    BOOST_CHECK(world->add_molecules(sp1, 25));
    sim.initialize();

#ifdef WITH_HDF5
    world->save("data_unbinding_reaction0.h5");
#endif
    for (Integer i(0); i < 10; ++i)
    {
        sim.step();
    }
    const Integer num_sp1(world->num_molecules(sp1));
    BOOST_ASSERT(num_sp1 < 25);
    BOOST_CHECK_EQUAL(25 - num_sp1, world->num_molecules(sp2));
    BOOST_CHECK_EQUAL(25 - num_sp1, world->num_molecules(sp3));
#ifdef WITH_HDF5
    world->save("data_unbinding_reaction1.h5");
#endif
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_degradation_reaction)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const std::string radius("1.25e-9");
    const ecell4::Species sp1("A", radius, "1.0e-12");

    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    model->add_species_attribute(sp1);

    model->add_reaction_rule(create_degradation_reaction_rule(sp1,1e5));

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    BOOST_CHECK(world->add_molecules(sp1, 25));
    sim.initialize();

#ifdef WITH_HDF5
    world->save("data_degradation_reaction0.h5");
#endif
    for (Integer i(0); i < 10; ++i)
    {
        sim.step();
    }
    BOOST_ASSERT(world->num_molecules(sp1) < 25);
#ifdef WITH_HDF5
    world->save("data_degradation_reaction1.h5");
#endif
}

BOOST_AUTO_TEST_CASE(LattiecSimulator_test_scheduler)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);

    const std::string D1("1.0e-12"),
          D2("1.1e-12"),
          D3("1.2e-12"),
          radius("2.5e-9");

    const ecell4::Species sp1("A", radius, D1),
        sp2("B", radius, D2),
        sp3("C", radius, D3);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp1);
    (*model).add_species_attribute(sp2);
    (*model).add_species_attribute(sp3);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteWorld::coordinate_type
        c1(world->inner2coordinate(41*400*400+35*400+56)),
        c2(world->inner2coordinate(33*400*400+51*400+25)),
        c3(world->inner2coordinate(61*400*400+37*400+90));
    BOOST_CHECK(world->new_voxel(sp1, c1).second);
    BOOST_CHECK(world->new_voxel(sp2, c2).second);
    BOOST_CHECK(world->new_voxel(sp3, c3).second);

    SpatiocyteSimulator sim(model, world);

    sim.initialize();

    BOOST_ASSERT(world->has_molecule_pool(sp1));
    BOOST_ASSERT(world->has_molecule_pool(sp2));
    BOOST_ASSERT(world->has_molecule_pool(sp3));

    const MoleculePool
        *mt1(world->find_molecule_pool(sp1)),
        *mt2(world->find_molecule_pool(sp2)),
        *mt3(world->find_molecule_pool(sp3));

    MoleculePool::const_iterator
        itr1(mt1->begin()),
        itr2(mt2->begin()),
        itr3(mt3->begin());

    BOOST_ASSERT(itr1 != mt1->end());
    BOOST_ASSERT(itr2 != mt2->end());
    BOOST_ASSERT(itr3 != mt3->end());

    c1 = (*itr1).coordinate;
    c2 = (*itr2).coordinate;
    c3 = (*itr3).coordinate;

    sim.step();
    itr1 = mt1->begin();
    itr2 = mt2->begin();
    itr3 = mt3->begin();
    BOOST_ASSERT((*itr1).coordinate == c1);
    BOOST_ASSERT((*itr2).coordinate == c2);
    BOOST_ASSERT((*itr3).coordinate != c3);
    c3 = (*itr3).coordinate;

    sim.step();
    itr1 = mt1->begin();
    itr2 = mt2->begin();
    itr3 = mt3->begin();
    BOOST_ASSERT((*itr1).coordinate == c1);
    BOOST_ASSERT((*itr2).coordinate != c2);
    BOOST_ASSERT((*itr3).coordinate == c3);
    c2 = (*itr2).coordinate;

    sim.step();
    itr1 = mt1->begin();
    itr2 = mt2->begin();
    itr3 = mt3->begin();
    BOOST_ASSERT((*itr1).coordinate != c1);
    BOOST_ASSERT((*itr2).coordinate == c2);
    BOOST_ASSERT((*itr3).coordinate == c3);
    c1 = (*itr1).coordinate;

}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_finalize)
{
    const Real L(1e-6);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(2.5e-9);
    const Integer N(60);

    const std::string D("1e-12"), radius("2.5e-9");

    ecell4::Species sp("A", radius, D);
    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);

    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    world->add_molecules(sp, N);
    sim.initialize();

    while(sim.step(0.311111111))
        ;

#ifdef WITH_HDF5
    world->save("data_finalize_before.h5");
#endif
    sim.finalize();
#ifdef WITH_HDF5
    world->save("data_finalize_after.h5");
#endif
}

BOOST_AUTO_TEST_CASE(SpatiocyteSimulator_test_shape)
{
    const Real L(2.5e-8);
    const Real3 edge_lengths(L, L, L);
    const Real voxel_radius(1e-9);
    const std::string D("1e-12"), radius("2.5e-9");
    Species membrane("Membrane", "2.5e-9", "0");

    Species sp("SpeciesA", "2.5e-9", "1e-12");
    sp.set_attribute("location", "Membrane");

    boost::shared_ptr<NetworkModel> model(new NetworkModel());
    (*model).add_species_attribute(sp);
    boost::shared_ptr<GSLRandomNumberGenerator>
        rng(new GSLRandomNumberGenerator());
    boost::shared_ptr<SpatiocyteWorld> world(
            new SpatiocyteWorld(edge_lengths, voxel_radius, rng));

    SpatiocyteSimulator sim(model, world);

    boost::shared_ptr<const Sphere> sphere(new Sphere(Real3(L/2, L/2, L/2), L*1/3));

    BOOST_CHECK(world->add_structure(membrane, sphere) > 0);
    BOOST_CHECK(!world->new_particle(Particle(sp, Real3(L/2, L/2, L*5/6),
                    2.5e-9, 1e-12)).second);  // This should fail
    BOOST_CHECK(world->new_particle(Particle(sp, Real3(L/2, L/2, L*5/6 - voxel_radius),
                    2.5e-9, 1e-12)).second);

    sim.initialize();
#ifdef WITH_HDF5
    world->save("structure_before.h5");
#endif

    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();
    sim.step();

#ifdef WITH_HDF5
    world->save("structure_after.h5");
#endif
}
