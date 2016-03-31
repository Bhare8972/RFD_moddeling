
#include <iostream>
#include <time.h>
#include <cmath>
#include <ctime>
#include <gsl/gsl_rng.h>
#include "constants.hpp"
#include "ionization_table.hpp"

using namespace std;
using namespace gsl_utils;

//this file tests the relative speed of lookup values of energy loss due to ionization vs using the beth formula

double lower_test_momentum_squared=10E-5; 
double upper_test_momentum_squared=4827; 
gsl_rng* _rand;

double beth_force(double momentum_squared)
{
	double gamma_squared=1+momentum_squared;
	double gamma_=sqrt(gamma_squared);
	double inv_beta_squared=gamma_squared/momentum_squared;

	double term1=log(inv_I_sq*momentum_squared*(gamma_-1.0));
	double term2=(1+(2.0/gamma_)-1.0/gamma_squared)*log(2.0);
	double term3=(1.0-2.0/gamma_+1.0/gamma_squared)/8.0;
	double term4=1.0/gamma_squared;

	if(std::isnan(term1)) return 0.0;

	return inv_beta_squared*(term1 - term2 + term3 + term4);
}

ionization_table ionization;

int main()
{
	int tests=50000;
	
	//make random number generator
	_rand=gsl_rng_alloc(gsl_rng_mt19937);
	unsigned int seed_val( time(NULL) );
	gsl_rng_set(_rand, seed_val);

	double test_momentum_squared;
	int start=clock();
	for(int x=0; x<tests; x++)
	{
		test_momentum_squared=lower_test_momentum_squared+gsl_rng_uniform(_rand)*(upper_test_momentum_squared-lower_test_momentum_squared);
	}	
	int stop=clock();
	int dry_speed=(stop-start);
	cout<< "dry-run: "<<dry_speed<<endl;
	
	start=clock();
	for(int x=0; x<tests; x++)
	{
		test_momentum_squared=lower_test_momentum_squared+gsl_rng_uniform(_rand)*(upper_test_momentum_squared-lower_test_momentum_squared);
		beth_force(test_momentum_squared);
	}	
	stop=clock();
	cout<< "beth: "<<(stop-start)-dry_speed<<endl;
	
	start=clock();
	for(int x=0; x<tests; x++)
	{
		test_momentum_squared=lower_test_momentum_squared+gsl_rng_uniform(_rand)*(upper_test_momentum_squared-lower_test_momentum_squared);
		ionization.electron_lookup(test_momentum_squared);
	}	
	stop=clock();
	cout<< "lookup: "<<(stop-start)-dry_speed<<endl;
	
	gsl_rng_free(_rand);
	//memory man, memory man, make me a leak as fast as you can!
	//hint: press ctrl+c
}
