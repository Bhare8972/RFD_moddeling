

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>
#include <list>
#include <thread>
#include <mutex>

#include <gsl/gsl_sort_vector_long.h>

#include "vector.hpp"
#include "vector_long.hpp"
#include "permutation.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"
#include "span_tree.hpp"

#include "../physics/shielded_coulomb_diffusion.hpp"



class energy_level_workspace
{
public:
	diff_cross_section cross_section;//this is thread safe
	rand_threadsafe rand;
	double energy;
	long N_samples;

    gsl::vector timesteps;
    gsl::vector num_interactions_per_timestep;

    std::vector< std::list<double> > distributions;
    std::mutex distributions_mutex;

	energy_level_workspace(gsl::vector timesteps_, double energy_) : cross_section(energy_)
	{
	    energy=energy_;
	    timesteps=timesteps_;

	    num_interactions_per_timestep=timesteps*cross_section.num_interactions_per_tau;

        distributions.reserve(timesteps.size());
        for(auto TS : timesteps)
        {
            (void)TS;
            distributions.push_back(  std::list<double>() );
        }
	}

	void sample_timesteps()
	{
	    //get our random samples
	    gsl::vector_long num_samples(timesteps.size());
	    LOOP(num_samples, A, num_interactions_per_timestep, rand.poisson(A) );

	    gsl::permutation num_samples_sorter(num_samples.size(), true);
	    gsl_sort_vector_long_index(num_samples_sorter.get(), num_samples.get()); //store sorted values in num_samples_sorter

	    long current_num_sample_index=0;
        long max_num_samples=num_samples[ num_samples_sorter[ num_samples.size()-1 ] ];

        gsl::vector T({0,0,1});

        size_t current_num_interactions=0;
        for(; current_num_interactions<=max_num_samples; current_num_interactions++)
        {
            while( current_num_interactions== num_samples[ num_samples_sorter[current_num_sample_index] ] ) //do this fist, becouse there could be a timestep with 0 interactions
            {
                double Z=acos(T[2]);
                std::lock_guard<std::mutex> lock(distributions_mutex);
                distributions[ num_samples_sorter[current_num_sample_index] ].push_back(Z);
                current_num_sample_index++;
                if(current_num_sample_index==num_samples.size()) { break; }
            }

            double inclination_scattering;
            double azimuth_scattering;
            {
                inclination_scattering=cross_section.sample( rand.uniform() ); //transform the sample
            }
            azimuth_scattering=rand.uniform()*2*PI;

            //calculate the three vector magnitudes
            double A=cos(inclination_scattering); //basis vector is original vector
            double B=sin(inclination_scattering)*cos(azimuth_scattering); //basis vector will be vector Bv below
            double C=-sin(inclination_scattering)*sin(azimuth_scattering); //basis vector will be vector Cv below

            //find vector Bv, perpinduclar to momentum
            gsl::vector init({1,0,0});
            gsl::vector Bv=cross(init, T);
            if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
            {
                init=gsl::vector({0,1,0}); //so we try a different init. momentum cannot be parrellel to both inits
                Bv=cross(init, T);
            }

            //normalize Bv
            Bv/=sqrt(Bv.sum_of_squares());

            //now we find Cv
            gsl::vector Cv=cross(Bv, T); //Bv and momentum are garenteed to be perpindicular,therefor Cv should have unit magnitude

            //find new vector
            T=A*T + B*Bv + C*Cv;
        }
        //print( "tst:", max_num_samples, T.sum_of_squares() );
	}

	inline void multi_samples(size_t N)
	{
	    for(size_t i=0; i<N; i++)
	    {
            sample_timesteps();
	    }
	}

	void thread_sample(size_t samples_perThread_perRun, size_t N_threads)
	{
    //// run for some initial number of runs
        std::list<std::thread> threads;
        for(size_t i=0; i<N_threads; i++)
        {
            threads.push_back( std::thread(&energy_level_workspace::multi_samples, this, samples_perThread_perRun) );
            //multi_samples(samples_per_thread, i*samples_per_thread);
        }
        for(auto& T : threads)
        {
            T.join();
        }
        threads.clear();
	}


    void output(arrays_output& tables_out)
    {
        for(auto& dist_list : distributions)
        {
            dist_list.sort();
            auto samples=make_vector(dist_list);

            auto samples_table=std::make_shared<doubles_output>(samples);
            tables_out.add_array(samples_table);
        }
    }

};


int main()
{
	double min_energy=lowest_physical_energy;  //100.0/energy_units_kev;
	double max_energy=100000.0/energy_units_kev; //100000/energy_units_kev;
	int num_energies=50;

    double min_timestep=2e-7;
    double max_timestep=0.01;
    size_t num_timesteps=60;

	size_t threads_per_energy=3; //num threads per energy
	size_t num_samples_per_energy_per_thread_per_run=1333;

	gsl::vector energy_vector=logspace(log10(min_energy), log10(max_energy), num_energies);
    gsl::vector timesteps=logspace(log10(min_timestep), log10(max_timestep), num_timesteps);

    //file IO
    arrays_output tables_out;
    auto energies_table=std::make_shared<doubles_output>(energy_vector);
    tables_out.add_array(energies_table);
    auto timesteps_table=std::make_shared<doubles_output>(timesteps);
    tables_out.add_array(timesteps_table);


    //start procesing for each energy
    for(int energy_i=0; energy_i<energy_vector.size(); energy_i++)
    {
        print("starting energy:", energy_vector[energy_i], "(", energy_i+1, "/", num_energies, ")");

        energy_level_workspace workspace(timesteps, energy_vector[energy_i]);
        print(" sampling");
        workspace.thread_sample(num_samples_per_energy_per_thread_per_run, threads_per_energy);
        print(" saving");
        workspace.output(tables_out);
    }
    //write to file

	binary_output fout("./shielded_coulomb_diffusion");
	tables_out.write_out( &fout);
}

