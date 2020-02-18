// vim: foldmethod=marker
// Sakamoto EPDP Sample

#include <stdexcept>
#include <vector>
#include <string>
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <cstdlib>
#include <gsl/gsl_roots.h>

#include <boost/format.hpp>

// // epdp headers
// #include <ecell4/epdp/config.h>
// #include <ecell4/epdp/utils/range.hpp>
// #include <ecell4/epdp/World.hpp>
// //#include <ecell4/epdp/ParticleModel.hpp>
// //#include <ecell4/epdp/SpeciesType.hpp>
// //#include <ecell4/epdp/SpeciesTypeID.hpp>
// //#include <ecell4/epdp/CuboidalRegion.hpp>
// //#include <ecell4/epdp/NetworkRules.hpp>
// //#include <ecell4/epdp/ReactionRule.hpp>
// #include <ecell4/epdp/EGFRDSimulator.hpp>
// //#include <ecell4/epdp/NetworkRulesAdapter.hpp>
// //#include <ecell4/epdp/GSLRandomNumberGenerator.hpp>

#include <ecell4/core/Model.hpp>
#include <ecell4/core/RandomNumberGenerator.hpp>
#include <ecell4/core/NetworkModel.hpp>
#include <ecell4/core/Species.hpp>
#include <ecell4/core/ReactionRule.hpp>

#include <ecell4/egfrd/egfrd.hpp>


// typedef double Real;

int main(int argc, char **argv)
{
    // Traits typedefs
    // {{{
    // typedef ::World< ::CyclicWorldTraits<Real> > world_type;
    // typedef EGFRDSimulator< ::EGFRDSimulatorTraitsBase<world_type> >
    //     simulator_type;
    typedef ecell4::egfrd::EGFRDWorld world_type;
    typedef ecell4::egfrd::EGFRDSimulator simulator_type;
    typedef simulator_type::multi_type multi_type;
    // }}}

    // Constants
    // {{{
    const ecell4::Real L(1e-6);
    const ecell4::Real3 edge_lengths(L, L, L);
    const ecell4::Integer3 matrix_sizes(3, 3, 3);
    const ecell4::Real volume(L * L * L);
    const ecell4::Integer N(60);
    const ecell4::Real kd(0.1), U(0.5);
    const ecell4::Real ka(kd * volume * (1 - U) / (U * U * N));
    const ecell4::Real k2(ka), k1(kd);
    const ecell4::Integer dissociation_retry_moves(3);
    // }}}

    boost::shared_ptr<ecell4::NetworkModel>
        model(new ecell4::NetworkModel());

    // add ::SpeciesType to ::ParticleModel
    // {{{
    ecell4::Species sp1(
        std::string("A"), 2.5e-09, 1e-12);
    model->add_species_attribute(sp1);

    ecell4::Species sp2(
        std::string("B"), 2.5e-09, 1e-12);
    model->add_species_attribute(sp2);

    ecell4::Species sp3(
        std::string("C"), 2.5e-09, 1e-12);
    model->add_species_attribute(sp3);
    // }}}

    // ReactionRules
    // {{{
    // A -> B + C   k1
    // {{{
    ecell4::ReactionRule rr1(
        ecell4::create_unbinding_reaction_rule(sp1, sp2, sp3, k1));
    model->add_reaction_rule(rr1);
    // }}}

    // B + C -> A   k2
    // {{{
    ecell4::ReactionRule rr2(
        ecell4::create_binding_reaction_rule(sp2, sp3, sp1, k2));
    model->add_reaction_rule(rr2);
    // }}}
    // }}}

    // Random Number Generator (Instanciate and Initialize)
    // {{{
    // boost::shared_ptr<ecell4::GSLRandomNumberGenerator>
    boost::shared_ptr<ecell4::RandomNumberGenerator>
        rng(new ecell4::GSLRandomNumberGenerator());
    rng->seed((unsigned long int)0);
    // rng->seed(time(NULL));
    // }}}

    // World Definition
    // {{{
    boost::shared_ptr<world_type>
        world(new world_type(edge_lengths, matrix_sizes, rng));
    world->bind_to(model);
    // }}}

    // add ecell4::Species( ::SpeciesInfo) to ::World
    // {{{
    // world->add_species(ecell4::Species("A"));
    // world->add_species(ecell4::Species("B"));
    // world->add_species(ecell4::Species("C"));
    // }}}

    // Thorow particles into world at random
    // {{{
    world->add_molecules(ecell4::Species("A"), N);

    typedef std::vector<std::pair<ecell4::ParticleID, ecell4::Particle> >
        particle_id_pair_list;
    const particle_id_pair_list particles(world->list_particles());
    for (particle_id_pair_list::const_iterator i(particles.begin());
        i != particles.end(); ++i)
    {
        const ecell4::Real3 pos((*i).second.position());
        std::cout << "(" << pos[0] << pos[1] << pos[2] << ")" << std::endl;
    }
    // }}}

    // Logger Settings
    // {{{
    boost::shared_ptr< ::LoggerManager> logger_mng(
        new ::LoggerManager("dummy", ::Logger::L_WARNING));
    ::LoggerManager::register_logger_manager(
        "ecell.EGFRDSimulator", logger_mng);
    // }}}

    // EGFRDSimulator instance generated
    // {{{
    boost::shared_ptr<simulator_type> sim(
        new simulator_type(world, model, dissociation_retry_moves));
    // sim->paranoiac() = true;
    sim->initialize();
    // }}}

    // Simulation Executed
    // {{{
    ecell4::Real next_time(0.0), dt(0.02);
    std::cout << sim->t() << "\t"
        << world->num_molecules_exact(sp1) << "\t"
        << world->num_molecules_exact(sp2) << "\t"
        << world->num_molecules_exact(sp3) << "\t" << std::endl;
    // for (int i(0); i < 10; i++)
    // for (int i(0); i < 100; i++)
    for (int i(0); i < 100; i++)
    {
        next_time += dt;
        while (sim->step(next_time))
        {
            // if (sim->last_reactions().size() > 0)
            // {
            //     std::cout << sim->t() << "\t"
            //         << world->num_molecules_exact(sp1) << "\t"
            //         << world->num_molecules_exact(sp2) << "\t"
            //         << world->num_molecules_exact(sp3) << "\t" << std::endl;
            // }
        }

        std::cout << sim->t() << "\t"
            << world->num_molecules_exact(sp1) << "\t"
            << world->num_molecules_exact(sp2) << "\t"
            << world->num_molecules_exact(sp3) << "\t" << std::endl;
    }
    // }}}

    // world->save("test.h5");

    // Statistics
    // {{{
    int num_single_steps_per_type[simulator_type::NUM_SINGLE_EVENT_KINDS];
    num_single_steps_per_type[simulator_type::SINGLE_EVENT_REACTION]
        = sim->num_single_steps_per_type(simulator_type::SINGLE_EVENT_REACTION);
    num_single_steps_per_type[simulator_type::SINGLE_EVENT_ESCAPE]
        = sim->num_single_steps_per_type(simulator_type::SINGLE_EVENT_ESCAPE);

    std::cout << (boost::format("%1%: %2% \n")
        % "SINGLE_EVENT_REACTION"
        % num_single_steps_per_type[simulator_type::SINGLE_EVENT_REACTION]);
    std::cout << (boost::format("%1%: %2% \n")
        % "SINGLE_EVENT_ESCAPE"
        % num_single_steps_per_type[simulator_type::SINGLE_EVENT_ESCAPE]);

    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_SINGLE_REACTION_0"
        % sim->num_pair_steps_per_type(
            simulator_type::PAIR_EVENT_SINGLE_REACTION_0));
    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_SINGLE_REACTION_1"
        % sim->num_pair_steps_per_type(
            simulator_type::PAIR_EVENT_SINGLE_REACTION_1));
    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_COM_ESCAPE"
        % sim->num_pair_steps_per_type(simulator_type::PAIR_EVENT_COM_ESCAPE));
    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_IV_UNDETERMINED"
        % sim->num_pair_steps_per_type(
            simulator_type::PAIR_EVENT_IV_UNDETERMINED));
    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_IV_ESCAPE"
        % sim->num_pair_steps_per_type(simulator_type::PAIR_EVENT_IV_ESCAPE));
    std::cout << (boost::format("%1%: %2% \n")
        % "PAIR_EVENT_IV_REACTION"
        % sim->num_pair_steps_per_type(simulator_type::PAIR_EVENT_IV_REACTION));

    std::cout << (boost::format("%1%: %2% \n")
        % "NONE" % sim->num_multi_steps_per_type(multi_type::NONE));
    std::cout << (boost::format("%1%: %2% \n")
        % "ESCAPE" % sim->num_multi_steps_per_type(multi_type::ESCAPE));
    std::cout << (boost::format("%1%: %2% \n")
        % "REACTION" % sim->num_multi_steps_per_type(multi_type::REACTION));
    // }}}

    // {
    //     std::unique_ptr<world_type>
    //         world2(new world_type(ecell4::Real3(1, 2, 3), ecell4::Integer3(3, 6, 9)));
    //     std::cout << "edge_lengths:" << world2->edge_lengths()[0] << " " << world2->edge_lengths()[1] << " " << world2->edge_lengths()[2] << std::endl;
    //     std::cout << "matrix_sizes:" << world2->matrix_sizes()[0] << " " << world2->matrix_sizes()[1] << " " << world2->matrix_sizes()[2] << std::endl;
    //     std::cout << "num_particles: " << world2->num_particles() << std::endl;

    //     world2->load("test.h5");
    //     std::cout << "edge_lengths:" << world2->edge_lengths()[0] << " " << world2->edge_lengths()[1] << " " << world2->edge_lengths()[2] << std::endl;
    //     std::cout << "matrix_sizes:" << world2->matrix_sizes()[0] << " " << world2->matrix_sizes()[1] << " " << world2->matrix_sizes()[2] << std::endl;
    //     std::cout << "num_particles: " << world2->num_particles() << std::endl;
    // }

    return 0;
}
