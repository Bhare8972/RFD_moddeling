
#include <iostream>
#include "vector_float.hpp"
#include "constants.hpp"
#include "arrays_IO.hpp"
using namespace std;
// this file writes the values found in the ICRU report 37 into a format that can be easily read by the program.
// this writes the table in dimensionless units, where energy is converted to momentum^2
// that can be used in the simulation with no conversion
// note that the values given in ICRU 37 go to MUCH higher energy then needed for our simulation (up to 1000 Mev).
// In these tables I have only recorded up to 35 MeV. The tables below DO include the low-energy stopping powers
// found in table 8.1, for electrons only. The lowest stopping power for electrons (0,0) was added by myself

//energy of electrons in keV
gsl::vector_float electron_energy({ 0, 0.02, 0.04, 0.06, 0.1, 0.2, 0.4, 0.6, 1, 2,
4, 6, 10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0,
60.0, 70.0, 80.0, 90.0, 100, 125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
550, 600, 700, 800, 900, 1000, 1250, 1500, 1750, 2000, 2500, 3000, 3500,
4000, 4500, 5000, 5500, 6000, 7000, 8000, 9000, 10000, 12500, 15000, 17500,
20000, 25000, 30000, 35000});

//stopping power of electrons for energies in electron_energy, in units of MeV cm^2 g^-1
gsl::vector electron_SP({0, 26.0, 104, 170, 218, 216, 170, 139, 97.9, 65.8,
40.5, 30.1, 20.4, 16.63, 14.45, 12.83, 11.57, 9.753, 8.492, 7.563, 6.848,
6.281, 5.819, 5.435, 5.111, 4.593, 4.198, 3.886, 3.633, 3.172, 2.861, 2.637,
2.470, 2.236, 2.084, 1.978, 1.902, 1.845, 1.802, 1.769, 1.743, 1.706, 1.683,
1.669, 1.661, 1.655, 1.661, 1.672, 1.684, 1.712, 1.740, 1.766, 1.790, 1.812,
1.833, 1.852, 1.870, 1.902, 1.931, 1.956, 1.979, 2.029, 2.069, 2.104, 2.134,
2.185, 2.226, 2.257});

//energy of positrons in keV
gsl::vector_float positron_energy({10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0,
35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 70.0, 80.0, 90.0, 100.0, 125.0, 150.0,
175.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0, 550.0, 600.0, 700.0,
800.0, 900.0, 1000.0, 1250.0, 1500.0, 1750.0, 2000.0, 2500.0, 3000.0, 3500.0,
4000.0, 4500.0, 5000.0, 5500.0, 6000.0, 7000.0, 8000.0, 9000.0, 10000.0,
12500.0, 15000.0, 17500.0, 20000.0, 25000.0, 30000.0, 35000.0});

//stopping power of position for energies in positron_energy, in units of MeV cm^2 g^-1
gsl::vector positron_SP({21.8, 18.25, 15.78, 13.96, 12.56, 10.53, 9.126,
8.096, 7.306, 6.680, 6.171, 5.749, 5.393, 4.826, 4.393, 4.052, 3.776, 3.274,
2.935, 2.692, 2.510, 2.257, 2.091, 1.976, 1.893, 1.831, 1.784, 1.748, 1.719,
1.678, 1.652, 1.636, 1.626, 1.617, 1.621, 1.630, 1.642, 1.668, 1.695, 1.720,
1.744, 1.766, 1.786, 1.805, 1.823, 1.854, 1.883, 1.908, 1.931, 1.980, 2.020,
2.055, 2.085, 2.136, 2.176, 2.208});

//extra constants
double density=1.205E-3; // g cm^-3
double air_molecular_density=2.688E25; // m^-3
double electron_classical_radius=2.8179E-15;

int main()
{
	//convert energies to dimensionless units
	double kev_to_dimensionless=kilo*elementary_charge/electron_rest_energy;
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

	binary_output fout("../tables/ionization_losses");
	array_out.write_out( &fout);
}
