
#ifndef UTILS_SC_ELASTIC_SCATT
#define UTILS_SC_ELASTIC_SCATT
//this is for sampling from the shielded-Coulomb cross section

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "gen_ex.hpp"


class shielded_coulomb
{
private:
	double factor;
	gsl_rng* rand;
public:
	shielded_coulomb(double average_atomic_number)
	{
		factor=std::pow(average_atomic_number, 2.0/3.0)/(4.0*183.8*183.8);
		rand=gsl_rng_alloc(gsl_rng_mt19937);
		gsl_rng_set(rand, 0);
	}
	
	~shielded_coulomb()
	{
		gsl_rng_free(rand);
	}
	
	double sample_azimuth()
	//gives a uniform random sample between 0 and pi
	{
		return gsl_rng_uniform(rand)*3.1415926;
	}
	
	double diff_cross_section(double elivation, double beta, double momentum)
	//gives differential cross section. is in terms of beta, angle (radians), and momentum (in natural units of mc)
	{
		if(elivation>3.1415926 or elivation<-3.1415926){ return 0.0; }
		double S2=std::pow(sin(elivation/2.0), 2.0);
		double F=factor/(momentum*momentum);
		return F*F*(1.0-beta*beta*S2)/pow(S2 + F, 2.0 );
	}
	
	double sample_elivation(double beta, double momentum)
	{
		//step one: empirical formula to get cauchy dist. parameter
		double cauchy_param=(0.9e-2)*std::pow(beta, -1.09)*std::pow(1.0-beta, 0.3);
		double scale_factor=gsl_ran_cauchy_pdf(0.0, cauchy_param)/1.01;
		//use rejection sampling with cauchy distribution
		while(true)
		{
			//sample cauch distribution
			double test_eliv=gsl_ran_cauchy(rand, cauchy_param);
			double cauchy_PDF=gsl_ran_cauchy_pdf(test_eliv, cauchy_param);
			double diff_cross=diff_cross_section(test_eliv, beta, momentum)*scale_factor;
			//sanity check
			if(diff_cross>cauchy_PDF) throw gen_exception("error in elastic scattering cross section for beta: ", beta, " angle: ",test_eliv, " cauchy: ", cauchy_PDF, " scaled diff. cross: ", diff_cross);
			//sample uniform distribution
			double test=gsl_rng_uniform(rand)*cauchy_PDF;
			//test!
			if(test<diff_cross) return test_eliv;
		}
	}
};

#endif
