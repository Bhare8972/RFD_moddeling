﻿#ifndef SHIELDED_COULOMB_DIFFUSION_HPP
#define SHIELDED_COULOMB_DIFFUSION_HPP


#include <mutex>
#include <cmath>

#include "vector.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "functor.hpp"
//#include "integrate.hpp"
#include "gen_ex.hpp"
//#include "rand.hpp"
#include "chebyshev.hpp"

class diff_cross_section : public functor_1D
{
public:
	double energy;

	double momentum_sq;
	double beta_sq;

	double nitrogen_prefactor;
	double nitrogen_p_factor;
	double oxygen_prefactor;
	double oxygen_p_factor;
	double argon_prefactor;
	double argon_p_factor;

	double CDF_offset;

	double num_interactions_per_tau;
	std::mutex sampler_mutex; //do we really need this?
	CDF_sampler theta_sampler;

	diff_cross_section(double energy_=lowest_physical_energy)
	{
	    if(energy_<lowest_physical_energy)
	    {
	        print("warning in shielded coulomb cross section: energy is below lowest physical energy");
	    }
		nitrogen_prefactor=0.784*7.0*7.0/(2.0*average_air_atomic_number);
		nitrogen_p_factor=std::pow(7.0, 2.0/3.0)/(4*183.3*183.3);
		oxygen_prefactor=0.211*8.0*8.0/(2.0*average_air_atomic_number);
		oxygen_p_factor=std::pow(8.0, 2.0/3.0)/(4*183.3*183.3);
		argon_prefactor=0.005*18.0*18.0/(2.0*average_air_atomic_number);
		argon_p_factor=std::pow(18.0, 2.0/3.0)/(4*183.3*183.3);

        energy=energy_;
		momentum_sq=(energy+1.0)*(energy+1.0)-1;
		beta_sq=momentum_sq/(1+momentum_sq);
		double beta=sqrt(beta_sq);

		nitrogen_p_factor/=momentum_sq;
		oxygen_p_factor/=momentum_sq;
		argon_p_factor/=momentum_sq;

        nitrogen_prefactor/=(beta*momentum_sq);
        oxygen_prefactor/=(beta*momentum_sq);
        argon_prefactor/=(beta*momentum_sq);

        CDF_offset=0.0;
        CDF_offset=call(0.0);

        AdaptiveSpline_Cheby_O3 cheby_sampler(*this, 10E3, 0, PI);
        auto CDF_spline=cheby_sampler.get_spline();
        num_interactions_per_tau=CDF_spline->call(3.1415926);
        theta_sampler=CDF_sampler(CDF_spline);
	}

	double call(double angle)
	//return the cross section, in terms of a rate, integrated from 0 to angle
	{
        double S_2=std::sin(angle*0.5);
		S_2*=S_2;

		double nitrogen_denom=nitrogen_p_factor + S_2;
		double oxygen_denom=oxygen_p_factor + S_2;
		double argon_denom=argon_p_factor + S_2;

		double nitrogen_num=1.0+beta_sq*nitrogen_p_factor;
		double oxygen_num=1.0+beta_sq*oxygen_p_factor;
		double argon_num=1.0+beta_sq*argon_p_factor;

		return nitrogen_prefactor*(std::log(nitrogen_denom) - nitrogen_num/nitrogen_denom) + oxygen_prefactor*(std::log(oxygen_denom) - oxygen_num/oxygen_denom) + argon_prefactor*(std::log(argon_denom)-argon_num/argon_denom) - CDF_offset;
	}

	double sample(double U)
	{
	    std::lock_guard<std::mutex> lock(sampler_mutex);
        return theta_sampler.sample(U);
	}
};



/*
	inline double cross_section(double angle)
	// return differential number of interactions per differential tau and solid angle
	{
		double S_2=std::sin(angle*0.5);
		S_2*=S_2;
		double argon_denom=S_2+argon_p_factor;
		double oxygen_denom=S_2+oxygen_p_factor;
		double nitrogen_denom=S_2+nitrogen_p_factor;

		return cross_section_prefactor*(1.0-beta_sq*S_2)*( nitrogen_prefactor/(nitrogen_denom*nitrogen_denom) + oxygen_prefactor/(oxygen_denom*oxygen_denom) + argon_prefactor/(argon_denom*argon_denom) );
	}

//	double dp_dOmega(double angle)
	 //return differential probability per differential tau and solid angle, assuming that there is one interaction
//	{
//		double S_2=std::sin(angle/2.0);
//		S_2*=S_2;
//		double denom=S_2+p_factor;
//		return dp_dOmega_prefector*(1.0-beta_sq*S_2)/(denom*denom);
//       return cross_section(angle)/num_interactions_per_tau;
//	}

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
            azimuth_scattering=rand.uniform()*2*PI;

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
	*/

#endif
