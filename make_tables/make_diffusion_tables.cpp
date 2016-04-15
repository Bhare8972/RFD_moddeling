

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <ctime>
#include <list>
#include <thread>
#include <mutex>
#include <sstream>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "gen_ex.hpp"

using namespace std;

mutex cout_mutex;

class diff_cross_section : public functor_1D
{
public:
	double momentum_sq;
	double beta;
	double prefactor;
	double p_factor;
	diff_cross_section(double timestep, double energy_kev=0)
	{
		prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926);
		p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3);

		set_energy(energy_kev);
	}

	void set_energy(double energy_kev)
	{
		double energy=energy_kev*1000.0*elementary_charge/electron_rest_energy;
		momentum_sq=(energy+1.0)*(energy+1.0)-1;
		beta=sqrt(momentum_sq/(1+momentum_sq));
	}

	double cross_section(double angle)
	{
		double S=sin(angle/2.0);
		double numerator=1.0-beta*beta*S*S;
		double denom=S*S+p_factor/momentum_sq;
		return numerator*prefactor/(denom*denom*beta*momentum_sq);
	}

	double integrand(double angle)
	{
		return cross_section(angle)*sin(angle);
	}

	double call(double angle)
	{
		return integrand(angle);
	}
};

class workspace
{
public:
	diff_cross_section cross_section;
	double energy_;

	shared_ptr<poly_quad_spline> spline_sampler;
    mutex sampler_mutex;
	double num_interactions;


	list<thread> threads;

	gsl_rng* rand;
    mutex rand_mutex;

	gsl::vector final_distribution;
    mutex dist_mutex;


	workspace(double timestep, double energy, bool rnd_seed=false) : cross_section(timestep)
	{
        rand=gsl_rng_alloc(gsl_rng_mt19937);
		if(rnd_seed)
		{
		    unsigned int seed_val( time(NULL) );
		    gsl_rng_set(rand, seed_val);
		}
		else
        {
            gsl_rng_set(rand, 0);
        }

		set_energy(energy);
	}

    ~workspace()
	{
		gsl_rng_free(rand);
	}

    double sample_uniform()
	//gives a uniform random sample between 0 and 1
	{
        lock_guard<mutex> lock(rand_mutex);
		return gsl_rng_uniform(rand);
	}

    unsigned int sample_num_interactions()
	//samples poisson distribtion for number of diffusion interactions
	{
        lock_guard<mutex> lock(rand_mutex);
        return gsl_ran_poisson (rand, num_interactions);
	}

	void set_energy(double energy)
	{
        energy_=energy;
		cross_section.set_energy(energy);
		cum_adap_simps integrator(&cross_section, 0, 3.1415926, 1E4);
		gsl::vector points=integrator.points();
		gsl::vector cum_quads=integrator.cum_quads();
		num_interactions=cum_quads[cum_quads.size()-1]*2*3.1415926;
		cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

		gsl::vector quad_X;
		gsl::vector quad_Y;
        make_fix_spline(cum_quads, points, quad_X, quad_Y);// hope this function will fix the function if the points are singular!
		spline_sampler=make_shared<poly_quad_spline>(quad_X, quad_Y);
		spline_sampler->set_lower_fill(quad_X[0]);
		spline_sampler->set_upper_fill(quad_X[quad_X.size()-1]);


        //shared_ptr<doubles_output> cum_quad_table=make_shared<doubles_output>(cum_quads);
        //shared_ptr<doubles_output> point_table=make_shared<doubles_output>(points);
        //shared_ptr<doubles_output> value_table=make_shared<doubles_output>(integrator.values());
        //shared_ptr<doubles_output> quad_X_table=make_shared<doubles_output>(quad_X);
        //shared_ptr<doubles_output> quad_Y_table=make_shared<doubles_output>(quad_Y);

        //arrays_output array_out;
        //array_out.add_array(cum_quad_table);
        //array_out.add_array(point_table);
        //array_out.add_array(value_table);
        //array_out.add_array(quad_X_table);
        //array_out.add_array(quad_Y_table);

        //binary_output fout("./quadrature_tst");
        //array_out.write_out( &fout);
    }

	double sample_timestep()
	{
	    //get our random samples
        unsigned int num_samples=sample_num_interactions();

        gsl::vector T({0,0,1});

        for(size_t i=0; i<num_samples; i++)
        {
            double inclination_scattering;
            double azimuth_scattering;
            {
                lock_guard<mutex> lock(sampler_mutex);
                inclination_scattering=spline_sampler->call( sample_uniform() ); //transform the sample
                azimuth_scattering=sample_uniform()*2*3.1415926;
            }

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
            gsl::vector Cv=cross(Bv, T); //Bv and momentum are garenteed to be perpindicular.

            //give Bv correct magnitude
            //Bv*=sqrt(momentum_squared); //correct magnitude is 1

            //find new vector
            T=A*T + B*Bv + C*Cv;
        }

        return acos(T[2]);
	}

	void multi_samples(size_t N, size_t lowest_index=0)
	{
	    for(size_t i=0; i<N; i++)
	    {
            if((i%100)==0)
            {
                lock_guard<mutex> lock(cout_mutex);
                print(energy_,":", lowest_index, ":", i);
            }

            double sample=sample_timestep();

            {
                lock_guard<mutex> lock(dist_mutex);
                //print("A:", i+lowest_index, final_distribution.size());
                final_distribution[i+lowest_index]=sample;
                //print("B");
            }
	    }
	}

	void start_thread(size_t N, size_t N_threads)
	{
        if(threads.size() !=0 ) throw gen_exception("must join before starting more threads");
        final_distribution=gsl::vector(N);
        size_t samples_per_thread=int(N/N_threads);

        for(size_t i=0; i<N_threads; i++)
        {
            threads.push_back( thread(&workspace::multi_samples, this, samples_per_thread, i*samples_per_thread) );
            //multi_samples(samples_per_thread, i*samples_per_thread);
        }
	}

	void join_thread()
    {
        for(auto& T : threads)
        {
            T.join();
        }
        threads.clear();
    }

};

int main()
{
	double time_step=0.01;
	double min_energy=0.02; //keV
	double max_energy=30000; //kev
	int num_energies=10; //????
	size_t num_samples=10000;
	size_t threads_per_energy=16; //num threads per energy
	bool rnd_seed=false;

	gsl::vector energy_vector=linspace(min_energy, max_energy, num_energies);


    //start procesing for each energy
    std::list<workspace> samplers;
    for(double energy : energy_vector)
    {
        samplers.emplace_back(time_step, energy, rnd_seed);
        samplers.back().start_thread(num_samples, threads_per_energy);
        //samplers.back().join_thread();
    }

    //join threads
    for(workspace& W : samplers)
    {
        W.join_thread();
    }
    print("writing to file");

    //write to file
    arrays_output tables_out;
    shared_ptr<doubles_output> energies_table=make_shared<doubles_output>(energy_vector);
    tables_out.add_array(energies_table);

    for(workspace& W : samplers)
    {
        shared_ptr<doubles_output> sample_table=make_shared<doubles_output>(W.final_distribution);
        tables_out.add_array(sample_table);
    }

    stringstream fname;
    fname<<"../tables/diffusion"<<time_step<<endl;
	binary_output fout(fname.str());
	tables_out.write_out( &fout);

}
