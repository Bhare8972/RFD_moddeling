
#include <iostream>
#include <string>
#include <cmath>
#include "vector_float.hpp"
#include "constants.hpp"
#include "arrays_IO.hpp"
using namespace std;
//TODO: change all to doubles?

// this file writes the values found in the ICRU report 37 into a format that can be easily read by the program.
// this writes the table in dimensionless units, where energy is converted to momentum^2
// that can be used in the simulation with no conversion
// note that the values given in ICRU 37 go to MUCH higher energy then needed for our simulation (up to 1000 Mev).
// In these tables I have only recorded up to 35 MeV. The tables below DO include the low-energy stopping powers
// found in table 8.1, for electrons only.



//extra constants


int main()
{
    bool remove_moller_losses=true;

	//convert energies to dimensionless units
	double kev_to_dimensionless=Kilo*elementary_charge/electron_rest_energy;
	electron_energy*=kev_to_dimensionless;
	positron_energy*=kev_to_dimensionless;

	//convert energies to momentum squared
	electron_energy+=1.0; //now gamma
	gsl::vector_float electron_mom_sq=(electron_energy*electron_energy) - 1.0;

	positron_energy+=1.0; //now gamma
	gsl::vector_float positron_mom_sq=(positron_energy*positron_energy) - 1.0;

	//convert stopping power MeV cm^2 g^-1 to dimensionless force
	double conversion_factor=elementary_charge*1.0E8*density/(2.0*3.1415926*average_air_atomic_number*electron_classical_radius*electron_rest_energy*electron_classical_radius*air_molecular_density);
	electron_SP*=conversion_factor;
	positron_SP*=conversion_factor;

    //remove energy losses due to moller scattering, maybe?
    if(remove_moller_losses)
    {
        for(size_t i=0; i<electron_energy.size(); i++)
        {
            if(electron_energy[i]>(minimum_energy*2.0))
            {
                electron_SP[i]-=moller_losses(electron_energy[i]);
            }
        }
    }

	//save into a table!
	shared_ptr<floats_output> electron_energy_table(new floats_output(electron_mom_sq));
	shared_ptr<doubles_output> electron_SP_table(new doubles_output(electron_SP));
	shared_ptr<floats_output> positron_energy_table(new floats_output(positron_mom_sq));
	shared_ptr<doubles_output> positron_SP_table(new doubles_output(positron_SP));

	arrays_output array_out;
	array_out.add_array(electron_energy_table);
	array_out.add_array(electron_SP_table);
	array_out.add_array(positron_energy_table);
	array_out.add_array(positron_SP_table);

    string fname="../tables/ionization_losses";
    if(remove_moller_losses)
    {
        fname="../tables/ionization_losses_RML";
    }

	binary_output fout(fname);
	array_out.write_out( &fout);
}
