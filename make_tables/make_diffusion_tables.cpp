

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


class sampling_tree
{
    public:

    span_tree<int> all_data;
    span_tree<int> new_data;
    std::mutex tree_mutex;

    sampling_tree(){}

    void add_data(double D)
    {
        std::lock_guard<std::mutex> lock(tree_mutex);
        new_data.emplace(D, 0);
    }

    bool compare()
    //returns true if the new and old distributions are the same, false otherwise
    {
        std::lock_guard<std::mutex> lock(tree_mutex);

        long current_old_i=0;
        long current_new_i=0;
        long N_old_items=all_data.size();
        long N_new_items=all_data.size() + new_data.size();


        double dist_threshold=1.95*sqrt( (N_old_items+N_new_items)/double(N_old_items*N_new_items) );

        span_tree<int>::iterator old_iterator=all_data.begin();
        auto new_iterator=new_data.begin();
        auto end_iter_all=all_data.end();
        auto end_iter_new=new_data.end();
        while( (old_iterator!=end_iter_all) and (new_iterator!=end_iter_new) )
        {

            if( old_iterator.get_key() <  new_iterator.get_key())
            {
                current_old_i+=1;
                current_new_i+=1;

                float D=abs( (current_old_i/float(N_old_items))  -   (current_new_i/float(N_new_items)) );
                if( D>dist_threshold ){ return false; }

                ++old_iterator;
            }
            else
            {
                current_new_i+=1;

                float D=abs( (current_old_i/float(N_old_items))  -   (current_new_i/float(N_new_items)) );
                if( D>dist_threshold ){ return false; }

                ++new_iterator;
            }
        }
        return true;
    }

    void combine()
    {
        std::lock_guard<std::mutex> lock(tree_mutex);

        for(auto iter=new_data.begin(), end=new_data.end(); iter!=end; ++iter)
        {

            all_data.emplace(iter.get_key(), 0);
        }
        new_data.clear();
    }

    gsl::vector get_data()
    {
        std::lock_guard<std::mutex> lock(tree_mutex);

        for(auto iter=new_data.begin(), end=new_data.end(); iter!=end; ++iter)
        {
            print(iter.get_key());

            all_data.emplace(iter.get_key(), 0);
        }
        new_data.clear();

        return all_data.get_keys();
    }
};

typedef std::shared_ptr<sampling_tree> S_tree_ptr;


class energy_level_workspace
{
public:
	diff_cross_section cross_section;//this is thread safe
	rand_threadsafe rand;
	double energy;

    gsl::vector timesteps;
    gsl::vector num_interactions_per_timestep;

    std::vector<S_tree_ptr> distributions;//hist tool should be thread safe

	energy_level_workspace(gsl::vector timesteps_, double energy_) : cross_section(energy_)
	{
	    energy=energy_;
	    timesteps=timesteps_;

	    num_interactions_per_timestep=timesteps*cross_section.num_interactions_per_tau;

        distributions.reserve(timesteps.size());
        for(auto TS : timesteps)
        {
            (void)TS;
            distributions.push_back(  std::make_shared< sampling_tree >() );
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
                distributions[ num_samples_sorter[current_num_sample_index] ]->add_data(Z);
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

	void start_thread(size_t samples_perThread_perRun, size_t N_threads)
	{
    //// run for some initial number of runs
        long N_samples=0;

        print(" sample");
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
        N_samples+=samples_perThread_perRun*N_threads;

        print(" combine");
        for(auto& TS_tree : distributions)
        {
            TS_tree->combine();
        }


    ////run until all CDFS are equivelent////
        while(true)
        {
            print("  sample");
            //collect more samples
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
            N_samples+=samples_perThread_perRun*N_threads;


            //check if each CDF is equivalent
            print("  test and combine");
            bool all_equivalent=true;
            for(auto& TS_tree : distributions)
            {
                if(all_equivalent and (not TS_tree->compare()))
                {
                    all_equivalent=false;
                }
                TS_tree->combine();
            }

            if( all_equivalent )
            {
                print("done sampling energy:", energy);
                break;
            }
            else
            {
                print(" sampled:", N_samples, "samples. Energy:", energy);
            }
        }
	}


    void output(arrays_output& tables_out)
    {
        for(auto& TS_tree : distributions)
        {
            auto samples=TS_tree->get_data();

            auto samples_table=std::make_shared<doubles_output>(samples);
            tables_out.add_array(samples_table);
        }
    }

};


int main()
{
	double min_energy=lowest_physical_energy;  //100.0/energy_units_kev;
	double max_energy=100000.0/energy_units_kev; //100000/energy_units_kev;
	int num_energies=5; //200;

    double min_timestep=6e-7;
    double max_timestep=0.01;
    size_t num_timesteps=5;//1000, 20

	size_t threads_per_energy=1; //num threads per energy
	size_t num_samples_per_energy_per_thread_per_run=200;

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
        print("starting energy:", energy_vector[energy_i], "(", energy_i, "/", num_energies, ")");

        energy_level_workspace workspace(timesteps, energy_vector[energy_i]);
        workspace.start_thread(num_samples_per_energy_per_thread_per_run, threads_per_energy);
        workspace.output(tables_out);
    }
    //write to file

	binary_output fout("./diffusion_table_MONTECARLO");
	tables_out.write_out( &fout);
}

