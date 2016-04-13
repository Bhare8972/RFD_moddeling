

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <ctime>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "gen_ex.hpp"

using namespace std;

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
private:
	diff_cross_section cross_section;

	shared_ptr<poly_quad_spline> spline_sampler;
	double num_interactions;

	gsl_rng* rand;

public:
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
		return gsl_rng_uniform(rand);
	}

    unsigned int sample_num_interactions()
	//samples poisson distribtion for number of diffusion interactions
	{
        return gsl_ran_poisson (rand, num_interactions);
	}

	void set_energy(double energy)
	{
		cross_section.set_energy(energy);
		cum_adap_simps integrator(&cross_section, 0, 3.1415926, 1E4);
		gsl::vector points=integrator.points();
		gsl::vector cum_quads=integrator.cum_quads();
		num_interactions=cum_quads[cum_quads.size()-1]*2*3.1415926;
		cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1
		spline_sampler=make_shared<poly_quad_spline>(cum_quads, points);// hope the inverse can be represented by spline
		//but don't worry. WIll throw if not!
	}

	double sample_timestep()
	{
	    //get our random samples
        unsigned int num_samples=sample_num_interactions();

        gsl::vector T({0,0,1});

        for(size_t i=0; i<num_samples; i++)
        {
            double inclination_scattering=spline_sampler->call( sample_uniform() ); //transform the sample
            double azimuth_scattering=sample_uniform()*2*3.1415926;

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

	gsl::vector sample_timestep(size_t N)
	{
	    gsl::vector out(N);
	    for(size_t i=0; i<N; i++)
	    {
            if((i%10)==0) print(i);
	        out[i]=sample_timestep();
	    }
	    return out;
	}

};

int main()
{
	double time_step=0.01;
	double min_energy=0.02; //keV
	double max_energy=30000; //kev
	int num_energies=10; //????
	size_t num_samples=1000;

	gsl::vector energy_vector=linspace(min_energy, min_energy, num_energies);

    workspace sampler(time_step, max_energy);
    gsl::vector samples=sampler.sample_timestep(num_samples);

    shared_ptr<doubles_output> samples_table=make_shared<doubles_output>(samples);

    arrays_output array_out;
	array_out.add_array(samples_table);

	binary_output fout("./tst");
	array_out.write_out( &fout);

	//diff_cross_section scatterer(time_step, min_energy);

    //cum_adap_simps integrator(&scatterer, 0, 3.1415926, 1E4);
    //print("total integrand:", integrator.quad());
    //print("state:", integrator.info());

    //auto points=integrator.points();
    //auto values=integrator.values();
    //auto cum_quads=integrator.cum_quads();

	//shared_ptr<doubles_output> points_table(new doubles_output(points));
	//shared_ptr<doubles_output> values_table(new doubles_output(values));
	//shared_ptr<doubles_output> cum_quads_table(new doubles_output(cum_quads));

	//arrays_output array_out;
	//array_out.add_array(points_table);
	//array_out.add_array(values_table);
	//array_out.add_array(cum_quads_table);

	//binary_output fout("./tst");
	//array_out.write_out( &fout);
}
