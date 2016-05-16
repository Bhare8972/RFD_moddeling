
#include "GSL_utils.hpp"
#include "vector.hpp"
#include <time.h>

#include "../physics/bethe_eq.hpp"
#include "rand.hpp"


using namespace std;

size_t search_sorted_exponential_test(gsl::vector A, double v)
{
    print("exponential test. find:", v);
	if(v<A[0] or v>=A[A.size()-1]) throw gen_exception("value out of range");
	size_t lower=0;
	size_t upper=A.size()-1;
	while(true)
	{
		//check if found solution
		if( (upper-1)==lower ) return lower;
		//guess new solution
		size_t guess=size_t( std::log(v/A[lower])*(upper-lower)/std::log(A[upper]/A[lower]) ) + lower;

		print("lower:", lower, "upper:", upper, "guess:", guess);
		print(A[lower], A[upper], A[guess]);

		if(guess==lower){guess++;}
		else if(guess==upper){guess--;}

		if(A[guess]>v) upper=guess;
		else if(A[guess+1]<=v) lower=guess+1;
		else return guess;
		print("not found, repeat");
	}
}

int main()
{
    int num_tests=10000000;

    //// test test which lookup is faster for a known logrithmic scale
    gsl::vector scale=logspace(-1, 4, 100000);


    double lower_val=scale[0];
    double upper_val=scale[scale.size()-1];
    rand_gen gen(true);


	time_t start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(scale, V);
    }
	int stop=time(NULL);
	cout<< "standard lookup on exponential scale: "<<difftime(stop, start)<<endl;


	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(scale, V);
    }
	stop=time(NULL);
	cout<< "exponential lookup on exponential scale: "<<difftime(stop, start)<<endl;


	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(scale, V);
    }
	stop=time(NULL);
	cout<< "linear lookup on exponential scale: "<<difftime(stop, start)<<endl;
	print();
	print();


	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        gen.uniform(lower_val, upper_val);
    }
	stop=time(NULL);
	cout<< "dry rand run: "<<difftime(stop, start)<<endl;


    gsl::vector linscale=linspace(lower_val, upper_val, 100000);


	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(linscale, V);
    }
	stop=time(NULL);
	cout<< "standard lookup on linear scale: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(linscale, V);
    }
	stop=time(NULL);
	cout<< "linear lookup on linear scale: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(linscale, V);
    }
	stop=time(NULL);
	cout<< "exponential lookup on linear scale: "<<difftime(stop, start)<<endl;
	print();
	print();




	//now test on beth lookup
	lower_val=bethe_table::electron_energy[0];
	upper_val=bethe_table::electron_energy[bethe_table::electron_energy.size()-1];

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(bethe_table::electron_energy, V);
    }
	stop=time(NULL);
	cout<< "exponential lookup on electron energy: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(bethe_table::electron_energy, V);
    }
	stop=time(NULL);
	cout<< "normal lookup on electron energy: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(bethe_table::electron_energy, V);
    }
	stop=time(NULL);
	cout<< "linear lookup on electron energy: "<<difftime(stop, start)<<endl;
	print();
	print();


    ////test the implemented beth table
    electron_ionization_table test_table(false);

    lower_val=test_table.electron_mom_sq[0];
	upper_val=test_table.electron_mom_sq[test_table.electron_mom_sq.size()-1];


	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential(test_table.electron_mom_sq, V);
    }
	stop=time(NULL);
	cout<< "exponential lookup on bethe interped: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_d(test_table.electron_mom_sq, V);
    }
	stop=time(NULL);
	cout<< "normal lookup on bethe interped: "<<difftime(stop, start)<<endl;

	start=time(NULL);
    for(int i=0; i<num_tests; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_linear(test_table.electron_mom_sq, V);
    }
	stop=time(NULL);
	cout<< "linear lookup on bethe interped: "<<difftime(stop, start)<<endl;


    for(int i=0; i<5; i++)
    {
        double V=gen.uniform(lower_val, upper_val);
        search_sorted_exponential_test(test_table.electron_mom_sq, V);
        print();
    }
    print();

}
