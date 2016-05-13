
#include "GSL_utils.hpp"
#include "vector.hpp"
#include <time.h>

#include "../physics/bethe_eq.hpp"
#include "rand.hpp"


using namespace std;

int main()
{
    int num_tests=1000;

    //// test test which lookup is faster for a known logrithmic scale
    gsl::vector scale=logspace(-1, 4, 10000);
    double lower_val=scale[0];
    double upper_val=scale[scale.size()-1];
    rand_gen gen(true);


	int start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(scale, V);
    }
	int stop=clock();
	int standard_exp=(stop-start);
	cout<< "standard lookup on exponential scale: "<<standard_exp<<endl;


	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(scale, V);
    }
	stop=clock();
	int expL_exp=(stop-start);
	cout<< "exponential lookup on exponential scale: "<<expL_exp<<endl;


	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(scale, V);
    }
	stop=clock();
	int lin_exp=(stop-start);
	cout<< "linear lookup on exponential scale: "<<lin_exp<<endl;


	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        gen.uniform(lower_val, upper_val);
    }
	stop=clock();
	int dry=(stop-start);
	cout<< "dry rand run: "<<dry<<endl;


    gsl::vector linscale=linspace(lower_val, upper_val, 10000);


	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(linscale, V);
    }
	stop=clock();
	int std_lin=(stop-start);
	cout<< "standard lookup on linear scale: "<<std_lin<<endl;

	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(linscale, V);
    }
	stop=clock();
	int lin_lin=(stop-start);
	cout<< "linear lookup on linear scale: "<<lin_lin<<endl;

	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(linscale, V);
    }
	stop=clock();
	int exp_lin=(stop-start);
	cout<< "exponential lookup on linear scale: "<<exp_lin<<endl;



	//now test on beth lookup
	lower_val=bethe_table::electron_energy[0];
	upper_val=bethe_table::electron_energy[bethe_table::electron_energy.size()-1];

	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(bethe_table::electron_energy, V);
    }
	stop=clock();
	int expL_el=(stop-start);
	cout<< "exponential lookup on electron energy: "<<expL_el<<endl;

	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(bethe_table::electron_energy, V);
    }
	stop=clock();
	int standard_el=(stop-start);
	cout<< "normal lookup on electron energy: "<<standard_el<<endl;

	start=clock();
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(bethe_table::electron_energy, V);
    }
	stop=clock();
	int lin_el=(stop-start);
	cout<< "linear lookup on electron energy: "<<lin_el<<endl;

}
