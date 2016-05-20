#ifndef SHIELDED_COULOMB_DIFFUSION_HPP
#define SHIELDED_COULOMB_DIFFUSION_HPP


#include <mutex>
#include <cmath>

#include "vector.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"

class diff_cross_section : public functor_1D
{
public:
	double energy;

	double momentum_sq;
	double beta_sq;
	double prefactor;
	double p_factor;

	double num_interactions_per_tau;
	std::mutex spline_sampler_mutex; //why do we need this??
	std::shared_ptr<poly_spline> spline_sampler;
	rand_threadsafe rand;


	double cross_section_prefactor;
	double dp_dOmega_prefector;

	diff_cross_section(double energy_=lowest_physical_energy)
	{
		prefactor=average_air_atomic_number*average_air_atomic_number/(8*3.1415926);
		p_factor=std::pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3);

        energy=energy_;
		momentum_sq=(energy+1.0)*(energy+1.0)-1;
		beta_sq=momentum_sq/(1+momentum_sq);
		double beta=sqrt(beta_sq);

		p_factor/=momentum_sq;
        cross_section_prefactor=prefactor/(beta*momentum_sq);

        cum_adap_simps integrator(this, 0, 3.1415926, 1E4);
		gsl::vector points=integrator.points();
		gsl::vector cum_quads=integrator.cum_quads();
		num_interactions_per_tau=cum_quads[cum_quads.size()-1]*2*3.1415926;
		cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

		dp_dOmega_prefector=cross_section_prefactor/num_interactions_per_tau;

		gsl::vector quad_X;
		gsl::vector quad_Y;
        make_fix_spline(cum_quads, points, quad_X, quad_Y);// hope this function will fix the function if the points are singular!

		spline_sampler=std::make_shared<poly_spline>(quad_X, quad_Y);

		spline_sampler->set_lower_fill(quad_X[0]);
		spline_sampler->set_upper_fill(quad_Y[quad_X.size()-1]);
	}

	double cross_section(double angle)
	// return differential number of interactions per differential tau and solid angle
	{
		double S_2=std::sin(angle*0.5);
		S_2*=S_2;
		double denom=S_2+p_factor;
		return cross_section_prefactor*(1.0-beta_sq*S_2)/(denom*denom);
	}

	double dp_dOmega(double angle)
	// return differential probability per differential tau and solid angle, assuming that there is one interaction
	{
		double S_2=std::sin(angle/2.0);
		S_2*=S_2;
		double denom=S_2+p_factor;
		return dp_dOmega_prefector*(1.0-beta_sq*S_2)/(denom*denom);
	}

	double call(double angle)
	//the intergrand to find thte total cross section
	{
		return cross_section(angle)*sin(angle);
	}

    double sample_timestep(double timestep)
	{
	    //get our random samples
        unsigned int num_samples=rand.poisson(num_interactions_per_tau*timestep);

        gsl::vector T({0,0,1});

        for(size_t i=0; i<num_samples; i++)
        {
            double inclination_scattering;
            double azimuth_scattering;
            {
                std::lock_guard<std::mutex> lock(spline_sampler_mutex);
                inclination_scattering=spline_sampler->call( rand.uniform() ); //transform the sample
            }
            azimuth_scattering=rand.uniform()*2*3.1415926;

            //calculate the three vector magnitudes
            double A=std::cos(inclination_scattering); //basis vector is original vector
            double B=std::sin(inclination_scattering)*std::cos(azimuth_scattering); //basis vector will be vector Bv below
            double C=-std::sin(inclination_scattering)*std::sin(azimuth_scattering); //basis vector will be vector Cv below

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
};

#endif
