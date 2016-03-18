
#include <iostream>
#include <fstream>
#include <cmath>
#include "SC_elastic_scatt.hpp"

using namespace std;

int num_test=5000;
double beta=0.5;
double air_ave_num=14.5;

int main()
{
	double mom=beta/sqrt(1-beta*beta);
	shielded_coulomb scatt_sampler(air_ave_num);
	
	
	ofstream out("scattering_test.txt");
	for(int i=0; i<num_test; i++)
	{
		double elivation=scatt_sampler.sample_elivation(beta, mom);
		out<<elivation<<endl;
	}
}
