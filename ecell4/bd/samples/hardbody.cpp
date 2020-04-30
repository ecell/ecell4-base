#include <string>

#include <ecell4/core/types.hpp>
#include <ecell4/core/Species.hpp>
#include <ecell4/core/Real3.hpp>
#include <ecell4/core/NetworkModel.hpp>

#include <ecell4/bd/BDSimulator.hpp>

using namespace ecell4;
using namespace ecell4::bd;

/**
 * a simple function to dump particle position(s)
 */
void print_particle_position(const BDWorld& world, const ParticleID& pid)
{
    const Real3 pos(world.get_particle(pid).second.position());
    std::cout << std::setprecision(12) << world.t() << " : " << pos << std::endl;
}

/**
 * main function
 */
int main(int argc, char** argv)
{
    /// simulation parameters
    const Real L(1e-6);
    std::string D("5e-12"), radius("5e-9");
    const Real3 edge_lengths(L, L, L);
    const Integer3 matrix_sizes(3, 3, 3);

    /// instantiate NetworkModel
    std::shared_ptr<NetworkModel> model(new NetworkModel());

    /// create a Species, and set its attributes
    Species sp1("A");
    sp1.set_attribute("D", D);
    sp1.set_attribute("radius", radius);
    (*model).add_species_attribute(sp1);

    std::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator());

    /// instantiate BDWorld
    std::shared_ptr<BDWorld> world(new BDWorld(edge_lengths, matrix_sizes, rng));
    world->bind_to(model);

    /// create a Particle, and inject it into BDWorld
    BDWorld::molecule_info_type info1((*world).get_molecule_info(Species("A")));
    const Particle p1(
        sp1, Real3(0, 0, 0), info1.radius, info1.D);
    const ParticleID pid1((*world).new_particle(p1).first.first);
    world->save("test_bd.h5");

    /// instatiate BDSimulator
    BDSimulator sim(world, model);
    sim.set_dt(1e-6);

    /// run and log by the millisecond
    for (unsigned int i(0); i <= 10; ++i)
    {
        while (sim.step(1e-3 * i))
        {
            ; // do nothing
        }
        print_particle_position(*world, pid1);
    }
}
