#include <cmath>
#include "vector_float.hpp"
#include "constants.hpp"
#include "arrays_IO.hpp"
#include "iostream"
using namespace std;
using namespace gsl_utils;
// this file reads the table written by make_ionization_tables. It is meant as a test of the IO code

//extra constants
double density=1.205E-3; // g cm^-3
double air_molecular_density=2.688E25; // m^-3
double electron_classical_radius=2.8179E15;

int main()
{
	// read the results
	binary_input fin("./ionization_losses");
	array_input table_in(fin);
	
	array_input electron_mom_sq_table=table_in.get_array();
	gsl::vector_float electron_mom_sq=electron_mom_sq_table.read_floats();
	
	array_input electron_SP_table=table_in.get_array();
	gsl::vector_float electron_SP=electron_SP_table.read_floats();
	
	array_input positron_mom_sq_table=table_in.get_array();
	gsl::vector_float positron_mom_sq=positron_mom_sq_table.read_floats();
	
	array_input positron_SP_table=table_in.get_array();
	gsl::vector_float positron_SP=positron_SP_table.read_floats();
	
	//converion factors
	double SP_conversion_factor=density/(2.0*3.1415926*average_air_atomic_number*electron_classical_radius*electron_rest_energy*electron_classical_radius*air_molecular_density);
	double kev_to_dimensionless=kilo*elementary_charge/electron_rest_energy;
	//electron_SP/=conversion_factor;
	//positron_SP/=conversion_factor;
	
	//print!
	cout<<"electron SP:"<<endl;
	for(size_t i=0; i<electron_mom_sq.size(); i++)
	{
		double mom_sq=electron_mom_sq[i];
		double energy= sqrt(mom_sq+1.0)-1.0;
		double SP=electron_SP[i];
		cout<<(energy/kev_to_dimensionless)<<"   "<<(SP/SP_conversion_factor)<<endl;
	}
	cout<<endl<<"positron SP:"<<endl;
	for(size_t i=0; i<positron_mom_sq.size(); i++)
	{
		double mom_sq=positron_mom_sq[i];
		double energy= sqrt(mom_sq+1.0)-1.0;
		double SP=positron_SP[i];
		cout<<(energy/kev_to_dimensionless)<<"   "<<(SP/SP_conversion_factor)<<endl;
	}
}
