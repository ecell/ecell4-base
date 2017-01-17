#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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
    const Real3 pos(world.get_2D_particle(pid).second.position());
    std::cout << std::setprecision(12) << world.t() << " "
        << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
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
    boost::shared_ptr<NetworkModel> model(new NetworkModel());

    /// create a Species, and set its attributes
    Species sp1("A");
    sp1.set_attribute("D", D);
    sp1.set_attribute("radius", radius);
    (*model).add_species_attribute(sp1);

    boost::shared_ptr<RandomNumberGenerator> rng(new GSLRandomNumberGenerator());

    /// instantiate BDWorld
    boost::shared_ptr<BDWorld> world(new BDWorld(edge_lengths, matrix_sizes, rng));
    world->bind_to(model);

    // create polygon
    std::ifstream poly("polygon.xyz");
    if(!poly.good())
    {
        std::cout << "file open error" << std::endl;
        return 1;
    }

    std::size_t num_faces=0;
    while(!poly.eof())
    {
        std::string line;
        Real3 v1, v2, v3;
        {
            Real x, y, z;
            std::getline(poly, line);
            std::istringstream iss(line);
            iss >> x >> y >> z;
            v1 = Real3(x, y, z);
        }
        {
            Real x, y, z;
            std::getline(poly, line);
            std::istringstream iss(line);
            iss >> x >> y >> z;
            v2 = Real3(x, y, z);
        }
        {
            Real x, y, z;
            std::getline(poly, line);
            std::istringstream iss(line);
            iss >> x >> y >> z;
            v3 = Real3(x, y, z);
        }

        Triangle f(v1, v2, v3);
        world->new_face(f);
        poly.peek();
    }
    world->initialize_polygon();
    std::cerr << "polygon initialized" << std::endl;

    const std::size_t fid=0;
    Triangle f = world->face_at(fid);
    const Real3 initial = (f.vertex_at(0) + f.vertex_at(1) + f.vertex_at(2)) / 3.;
    std::cerr << "initial position = " << initial << std::endl;

    /// create a Particle, and inject it into BDWorld
    BDWorld::molecule_info_type info1((*world).get_molecule_info(Species("A")));
    const Particle p1(sp1, initial, info1.radius, info1.D);
    const ParticleID pid1((*world).new_2D_particle(p1, fid).first.first);
    world->save("test_bd.h5");


    /// instatiate BDSimulator
    BDSimulator sim(model, world);
    sim.set_dt(1e-6);

    /// run and log by the millisecond
    for(unsigned int i(0); i <= 100; ++i)
    {
        while(sim.step(1e-3*i))
        {
            // do nothing
        }
        print_particle_position(*world, pid1);
    }
    return 0;
}