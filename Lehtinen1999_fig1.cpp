
#include <iostream>
#include <cmath>
#include <fstream>
#include <map>

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

using namespace gsl;
using namespace std;

#define pass_planes
//#define histogram


class analyzer
{
public:
    int N_bins;
    double max_t;

    gsl::vector_long n_particles;
    gsl::vector bin_edges;
    map<size_t, double> particle_start_times;


    analyzer(double _max_t, int _N_bins)
    {
        max_t=_max_t;
        N_bins=_N_bins;
#ifdef pass_planes
        bin_edges=linspace(0, max_t, N_bins);
#elif defined(histogram)
        bin_edges=linspace(0, max_t, N_bins+1);
#endif // histogram vs pass_planes

        n_particles=gsl::vector_long(size_t(N_bins));
        LOOP(n_particles, X, n_particles, X*0); //fill n_particles with zero.
    }

    void reset()
    {
        particle_start_times.clear();
        LOOP(n_particles, X, n_particles, X*0); //fill n_particles with zero
    }

    void add_electron(electron_T* new_electron)
    {
        particle_start_times[new_electron->ID]=new_electron->current_time;
    }

    void remove_electron(electron_T* new_electron)
    {
        double end_time=new_electron->current_time;
        double start_time=particle_start_times[new_electron->ID];

        if(start_time>=max_t)
        {
            return;
        }

        int start_time_index;
        int end_time_index;

#ifdef pass_planes
        if(start_time<bin_edges[0])
        {
            start_time_index=0;
        }
        else
        {
            start_time_index=search_sorted_d(bin_edges, start_time)+1;
        }
        //print(start_time_index, new_electron->ID);

        if(end_time>=bin_edges[N_bins-1])
        {
            end_time_index=N_bins;
        }
        else
        {
            end_time_index=search_sorted_d(bin_edges, end_time)+1;
        }

#elif defined(histogram)
        if(start_time<bin_edges[1])
        {
            start_time_index=0;
        }
        else
        {
            start_time_index=search_sorted_d(bin_edges, start_time);
        }

        if(end_time>=bin_edges[N_bins-1])
        {
            end_time_index=N_bins;
        }
        else
        {
            end_time_index=search_sorted_d(bin_edges, end_time)+1;
        }

#endif // histogram vs pass_planes


        for(int i=start_time_index; i<end_time_index; i++)
        {
            n_particles[i]++;
        }
    }

    gsl::vector normalize(double n_seeds)
    {
        gsl::vector ret(N_bins);
        LOOP(ret, X, n_particles, X/n_seeds); //fill n_particles with zero
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
	particle_history_out save_data;
	analyzer histogramer;


    sim_cls(double _max_t, double E_delta, double B_tsi) :

	save_data("./output"),
    moller_engine(particle_removal_energy, 200000/energy_units_kev, 400, true),
	histogramer(_max_t, 1000),
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
        B_field.set_value(B_tsi*21.7, 0, 0);

        ////force engine setup////
        force_engine.set_max_timestep( coulomb_scattering_engine.max_timestep() );
        force_engine.set_errorTol(pos_tol, mom_tol);


    }

    void reset(double _max_t, double E_delta, double B_tsi)
    {
        max_t=_max_t;
        E_field.set_value(0, 0, -E_delta*21.7);
        B_field.set_value(B_tsi*21.7, 0, 0);
        histogramer.reset();
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
            save_data.new_electron(new_electron);
            histogramer.add_electron(new_electron);
        }
    }

    void run()
    {
        int i=0;
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

            if((i%20000)==0){ print("  ",i, current_electron->current_time); }


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
                histogramer.remove_electron(current_electron);
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
                        save_data.new_electron(new_electron);
                        histogramer.add_electron(new_electron);
                        electrons.insert(new_electron->current_time, new_electron);
                    }
                }
            }

            //remove particle if necessary
            if(current_electron->energy < particle_removal_energy)
            {
                save_data.remove_electron(0, current_electron);
                histogramer.remove_electron(current_electron);
                delete current_electron;
                continue;
            }

    //// shielded coulomb scattering ////
            coulomb_scattering_engine.scatter(energy_before_scattering, current_electron);


            save_data.update_electron(current_electron);
            electrons.insert(current_electron->current_time, current_electron);
        }

        //add all remaining electrons
        auto current_electron=electrons.pop_first();
        while(current_electron)
        {
            histogramer.remove_electron(current_electron);
            delete current_electron;
            current_electron=electrons.pop_first();
        }
    }
};

int main()
{
    double max_t=0.5;
    int n_seeds=10;
    double E_field=8.0;
    double B_field=0.0;
    int N_runs=1;

    sim_cls simulation(max_t, E_field, B_field);
    arrays_output out;

    for(int run_i=0; run_i<N_runs; run_i++)
    {
        print("Run:", run_i+1);
        simulation.reset(max_t, E_field, B_field);
        simulation.setup(n_seeds);
        simulation.run();

        if(run_i==0)
        {
            out.add_doubles(simulation.histogramer.bin_edges);
        }
        out.add_doubles(simulation.histogramer.normalize(n_seeds));
    }
    out.to_file("./Lehtinen1999_out");

}














