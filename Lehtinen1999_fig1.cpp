
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"
#include "rand.hpp"
#include "time_tree.hpp"
#include "arrays_IO.hpp"

#include "read_tables/diffusion_table.hpp"
#include "read_tables/bremsstrahlung_table.hpp"

#include "physics/quasi_static_fields.hpp"
#include "physics/relativistic_formulas.hpp"
#include "physics/particles.hpp"
#include "physics/bethe_eq.hpp"
#include "physics/apply_force.hpp"
#include "physics/moller_scattering.hpp"
#include "physics/interaction_chooser.hpp"

class electron_data
{
public:
    double time;

    electron_data(electron_T* electron)
    {
        time=electron->current_time;
    }

};
#include "physics/particles_source.hpp"

using namespace gsl;
using namespace std;


class particle_time_histogram
{
public:
    gsl::vector_long n_particles;
    gsl::vector bin_edges;

    particle_time_histogram(double max_t, int N_bins)
    {
        bin_edges=linspace(0, max_t, N_bins+1);
        n_particles=gsl::vector_long(size_t(N_bins));
        LOOP(n_particles, X, n_particles, 0); //fill n_particles with zero
    }

    void add_electron(electron_T* electron)
    {
        double end_time=electron->current_time;
        double start_time=electron->itter_history_begin()->time;

        int start_time_index=search_sorted_d(bin_edges, start_time);

        int end_time_index;
        if(end_time>=bin_edges[bin_edges.size()-1])
        {
            end_time_index=bin_edges.size();
        }
        else
        {
            end_time_index=search_sorted_d(bin_edges, end_time);
        }

        for(int i=start_time_index; i<end_time_index; i++)
        {
            n_particles[i]++;
        }
    }

    gsl::vector norm_particles(double n_seeds)
    {
        gsl::vector ret(n_particles.size());
        for(int i=0; i<n_particles.size(); i++)
        {
            ret[i]=n_particles[i]/n_seeds;
        }
        return ret;
    }
};

class sim_cls
{
public:
    ////constants////  (What will happen to results if we vary these?)
    const double pos_tol=0.0001;
    const double mom_tol=0.0001;
    const double initial_energy=1000.0/energy_units_kev; //lehtininin is 1 Gev. How does this affect results?
    double max_t;

    const double particle_removal_energy=2.0/energy_units_kev;

    ////fields///
	uniform_field E_field;
	uniform_field B_field;

	////physics engines////
    moller_table moller_engine; //moller scattering
    diffusion_table coulomb_scattering_engine;  //elastic scattering off air mollecules
    interaction_chooser_linear<1> interaction_engine; //interaction chooser (only one potential interaction at the moment
    apply_charged_force force_engine; //apply classical forces

    ////particles////
	time_tree<electron_T> electrons;


    sim_cls(double _max_t, double E_delta, double B_tsi) :

    moller_engine(particle_removal_energy, 200000/energy_units_kev, 400, true),
    interaction_engine(moller_engine),
    force_engine(particle_removal_energy, E_field.pntr(), B_field.pntr() )

    {
        max_t=_max_t;

        ////setup fields////
        //fields fill all space
        E_field.set_minimum(-INFINITY, -INFINITY, -INFINITY);
        E_field.set_maximum(INFINITY, INFINITY, INFINITY);
        B_field.set_minimum(-INFINITY, -INFINITY, -INFINITY);
        B_field.set_maximum(INFINITY, INFINITY, INFINITY);
        //set magnitudes
        E_field.set_value(0, 0, -E_delta*21.7);
        E_field.set_value(B_tsi*21.7, 0, 0);

        ////force engine setup////
        force_engine.set_max_timestep( coulomb_scattering_engine.max_timestep() );
        force_engine.set_errorTol(pos_tol, mom_tol);


    }

    void reset(double _max_t, double E_delta, double B_tsi)
    {
        max_t=_max_t;
        E_field.set_value(0, 0, -E_delta*21.7);
        E_field.set_value(B_tsi*21.7, 0, 0);
    }

    void setup(int n_seeds)
    {
        electrons.clear();
        ////seed electrons////
        for(int i=0; i<n_seeds; i++)
        {
            electron_T* new_electron= electrons.emplace(0);
            new_electron->set_position(0,0,0);
            new_electron->set_momentum(0,0, KE_to_mom( initial_energy ) );
            new_electron->update_energy();
        }
    }

    void run(particle_time_histogram& data_histogram)
    {
        int i=-1;
        while(true)
        {
            i++;

            auto current_electron=electrons.pop_first();
            if( (not current_electron) )
            {
                print("no electrons. Ending at", i);
                break;
            }
            if(current_electron->current_time>max_t)
            {
                print("no more time. Ending at", i);
                break;
            } //if no more electrons, or out of time

        /////solve equations of motion////
            double old_energy=current_electron->energy;
            auto old_position=current_electron->position;
            auto old_momentum=current_electron->momentum;
            force_engine.charged_particle_RungeKuttaCK(current_electron);
            current_electron->update_energy();

            //remove particle if necisary
            if(current_electron->energy < particle_removal_energy)
            {
                data_histogram.add_electron(current_electron);
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
                current_electron->next_timestep*=0.5;
            }
            else if(error_code==2) //this timestep was too large
            {
                //reset electron
                current_electron->current_time-=current_electron->timestep;
                current_electron->next_timestep=current_electron->timestep*0.5;
                current_electron->position=old_position;
                current_electron->momentum=old_momentum;
                current_electron->energy=old_energy;

                //place back in pool
                electrons.insert(current_electron->current_time, current_electron);

                //carry on as if nothing ever happened
                continue; //probably wind up with same electron again
            }

            //do the scattering
            if( (time_to_scatter <= current_electron->timestep) and interaction != -1)
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

                    //do interaction
                    auto new_electron=moller_engine.single_interaction(energy_before_scattering, current_electron);

                    if(new_electron)
                    {
                        new_electron->save_history();
                        electrons.insert(new_electron->current_time, new_electron);
                    }
                }
            }

            //remove particle if necessary
            if(current_electron->energy < particle_removal_energy)
            {
                data_histogram.add_electron(current_electron);
                delete current_electron;
                continue;
            }

    //// shielded coulomb scattering ////
            coulomb_scattering_engine.scatter(energy_before_scattering, current_electron);

    //// update electron ////
            current_electron->save_history();
            electrons.insert(current_electron->current_time, current_electron);
        }

        //add all remaining electrons
        auto current_electron=electrons.pop_first();
        while(current_electron)
        {
            data_histogram.add_electron(current_electron);
        }
    }
};

int main()
{
    double max_t=5.5;
    int n_seeds=10;

    sim_cls simulation(max_t, 2.0, 0.0);
    simulation.setup(n_seeds);

    particle_time_histogram data_hist(max_t, 10);

    simulation.run(data_hist);


    arrays_output out;
    out.add_doubles( data_hist.bin_edges );
    out.add_doubles( data_hist.norm_particles(n_seeds) );
    out.to_file("./Lehtinen1999_out");
}














