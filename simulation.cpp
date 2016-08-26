
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"
#include "rand.hpp"
#include "time_tree.hpp"

#include "read_tables/diffusion_table.hpp"

#include "physics/quasi_static_fields.hpp"
#include "physics/relativistic_formulas.hpp"
#include "physics/particles.hpp"
#include "physics/bethe_eq.hpp"
#include "physics/apply_force.hpp"
#include "physics/moller_scattering.hpp"
#include "physics/bremsstrahlung_scattering.hpp"
#include "physics/interaction_chooser.hpp"

using namespace gsl;
using namespace std;


int main()
{
    string output_fname="output";
    size_t nseeds=10;


	int number_itterations=40000*nseeds;
    double particle_removal_energy=2.0/energy_units_kev; //remove particles that have energy less than this
    double minimum_photon_energy=5.1/energy_units_kev; // will not produce discrete brem photons with energy less than this

    double pos_tol=0.0001;
    double mom_tol=0.0001;
    
////// initialize fields //////

	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -Kilo/distance_units);
	E_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 300/distance_units);
	E_field.set_value(0, 0, -3.75E5/E_field_units);
	//E_field.set_value(0, 0, 0/E_field_units);

	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -1/distance_units);
	B_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 20*Kilo/distance_units);
	//B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	B_field.set_value(0, 0, 0);


///////  initialize physics engines ////////
	rand_gen rand;

    //moller scattering
    moller_table moller_engine(particle_removal_energy, 200000/energy_units_kev, 400, true);

    //shielded coulomb diffusion
    diffusion_table coulomb_scattering_engine;

    //bremsstrahlung
    bremsstrahlung_scattering brem_engine( minimum_photon_energy );

    //interaction chooser
    interaction_chooser_linear<1> interaction_engine(moller_engine);//, brem_engine);

	//force
	apply_charged_force force_engine(particle_removal_energy, E_field.pntr(), B_field.pntr() );
    force_engine.set_max_timestep( coulomb_scattering_engine.max_timestep() );
    force_engine.set_errorTol(pos_tol, mom_tol);


	////output file////
    particle_history_out save_data(output_fname);

	////initial particle////
	time_tree<electron_T> electrons;
	electron_T* new_electron;

    for(int i=0; i<nseeds; i++)
    {
        new_electron= electrons.emplace(0);
        new_electron->set_position(0,0,0);
        new_electron->set_momentum(0,0, KE_to_mom( 1000.0/energy_units_kev ) );
        new_electron->update_energy();
        save_data.new_electron(new_electron);
    }

///// BEGIN SIMULATION //////
    int timestep_trims=0;
    int timestep_redone=0;
    int i=0;
	for(; i<=number_itterations; i++)
	{

        auto current_electron=electrons.pop_first();
        if(not current_electron) {break; } //if current_electron is null, then tree is empty

    /////solve equations of motion////
        double old_energy=current_electron->energy;
        auto old_position=current_electron->position;
        auto old_momentum=current_electron->momentum;
        force_engine.charged_particle_RungeKuttaCK(current_electron);
        current_electron->update_energy();

        //remove particle if necisary
        if(current_electron->energy < particle_removal_energy)
        {
            save_data.remove_electron(0, current_electron);
            delete current_electron;
            continue;
        }

        //for linear interpolation of position and momentum
        auto position_rate_of_change=current_electron->position-old_position;
        auto momentum_rate_of_change=current_electron->momentum-old_momentum;
        position_rate_of_change/=current_electron->timestep;
        momentum_rate_of_change/=current_electron->timestep;

        double energy_before_scattering=current_electron->energy;

    //// scattering (moller only presently) ////
        int interaction=-1;
        double time_to_scatter=interaction_engine.sample(old_energy, current_electron->energy, current_electron->timestep, interaction);

        //check error code
        auto error_code=interaction_engine.get_error_flag();
        if(error_code==1) //need to reduce the timestep size
        {
            //print("trim timestep");
            timestep_trims+=1;
            current_electron->next_timestep*=0.5;
        }
        else if(error_code==2) //this timestep was too large
        {
            //print("timestep too large!");
            timestep_redone+=1;
            i-=1; //this itteration doesn't count
            //reset electron
            current_electron->current_time-=current_electron->timestep;
            current_electron->next_timestep=current_electron->timestep *=0.5;
            current_electron->position=old_position;
            current_electron->momentum=old_momentum;
            current_electron->energy=old_energy;

            //place back in pool
            electrons.insert(current_electron->current_time, current_electron);

            //carry on as if nothing ever happend
            continue; //probably wind up with same electron again
        }

        //do the scattering
        if(time_to_scatter <= current_electron->timestep and interaction != -1)
        {


            if(interaction==0) //moller scattering
            {
                //set electron values to time of interaction
                current_electron->current_time += time_to_scatter - current_electron->timestep;
                current_electron->timestep=time_to_scatter;
                current_electron->position = old_position + position_rate_of_change*time_to_scatter;
                current_electron->momentum = old_momentum + momentum_rate_of_change*time_to_scatter;
                current_electron->update_energy();
                energy_before_scattering=current_electron->energy;

                new_electron=moller_engine.single_interaction(energy_before_scattering, current_electron);

                if(new_electron)
                {
                    save_data.new_electron(new_electron);
                    electrons.insert(new_electron->current_time, new_electron);
                }
            }
        }

        //remove particle if necisary
        if(current_electron->energy < particle_removal_energy)
        {
            save_data.remove_electron(0, current_electron); //need to fix removal reasons to be obvious
            delete current_electron;
            continue;
        }

//// shielded coulomb scattering ////
        coulomb_scattering_engine.scatter(energy_before_scattering, current_electron);

//// update electron ////
        save_data.update_electron(current_electron);
        electrons.insert(current_electron->current_time, current_electron);

	}

	print(i, "iterations of:", number_itterations);
	print(timestep_trims, "trims");
	print(timestep_redone, "re-does");
}














