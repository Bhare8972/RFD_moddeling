
#ifndef MOLLER_SCATTEING
#define MOLLER_SCATTEING

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <ctime>

class electron;

class moller_scattering
{
private:
	gsl_rng* rand;
	double time_step;
	double E_min;

public:
    moller_scattering(double time_step_, double minimum_energy, bool rnd_seed=false)
    {
        time_step=time_step_;
        E_min=minimum_energy;

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
    }

    bool does_scatter(electron& particle)
    {
        double gamma=particle.gamma();
        double beta=std::sqrt(particle.momentum.sum_of_squares())/gamma;

        double p=( 1.0/E_min  -  1.0/(gamma-1.0-E_min)  +  (gamma-1.0-2*E_min)/(2.0*gamma*gamma)  +
                  std::log(E_min/(gamma-1.0-E_min)) *(2*gamma-1.0)/((gamma-1.0)*gamma*gamma)   )*time_step/beta;
        if( gsl_rng_uniform(rand) < p)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    electron preform_scatter(electron& primary)
    {
        double primary_energy=primary.gamma()-1.0;
        double secondary_energy=primary_energy*E_min/( E_min - gsl_rng_uniform(rand)*(primary_energy-2*secondary_energy));
        //note: line above is an approxamation, accurate to 5 %. See lehtinen99.

        I AM HERE
    }

};

#endif
