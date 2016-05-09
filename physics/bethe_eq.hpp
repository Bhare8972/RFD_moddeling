
#ifndef BETH_EQUATION_HPP
#define BETH_EQUATION_HPP

#include <cmath>

#include "vector.hpp"
#include "constants.hpp"

////equations
double bethe_formula(double mom_sq)
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double exp_term1=beta_sq*KE*gamma_sq*inv_I_sq;
    double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
    double term3=KE*KE/(8.0*gamma_sq) + 1.0/gamma_sq;

    return (std::log(exp_term1)-term2_factor*std::log(2)+term3)/beta_sq;
}

double moller_losses(double mom_sq, double min_energy)
//energy loss due to moller scattering for a particle with energy 'KE' that frees electrons to have energy at least 'min_energy', where KE must be greater than 2*min_energy
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double term_1=std::log(KE/(2.0*min_energy));
    double term_2=min_energy/(KE-min_energy);
    double term_3_factor1=1+(2/gamma)-1.0/gamma_sq;
    double term_3_factor2=log(2*(KE-min_energy)/KE);
    double term_4=KE*KE/(8*gamma_sq);
    double term_5=min_energy*min_energy/(2*gamma_sq);
    return (term_1 - term_2 - term_3_factor1*term_3_factor2 + term_4 - term_5 +1)/beta_sq;
}

double bethe_subtract_moller(double mom_sq, double minimum_energy)
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double exp_term1=2*minimum_energy*beta_sq*gamma_sq*inv_I_sq;
    double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
    double exp_term2_factor2=KE/(KE-minimum_energy);
    double term3=minimum_energy/(KE-minimum_energy)-beta_sq;
    double term4=minimum_energy*minimum_energy/(2*gamma_sq);

    return (std::log(exp_term1)-term2_factor*std::log(exp_term2_factor2)+term3+term4)/beta_sq;
}

//add bhaba_losses here

////tables
namespace bethe_table{

double density=1.205E-3; // g cm^-3
double air_molecular_density=2.688E25; // m^-3
double electron_classical_radius=2.8179E-15;

//energy of electrons in keV
gsl::vector electron_energy({ 0.02, 0.04, 0.06, 0.1, 0.2, 0.4, 0.6, 1, 2,
4, 6, 10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0,
60.0, 70.0, 80.0, 90.0, 100, 125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
550, 600, 700, 800, 900, 1000, 1250, 1500, 1750, 2000, 2500, 3000, 3500,
4000, 4500, 5000, 5500, 6000, 7000, 8000, 9000, 10000, 12500, 15000, 17500,
20000, 25000, 30000, 35000});

//stopping power of electrons for energies in electron_energy, in units of MeV cm^2 g^-1
gsl::vector electron_SP({26.0, 104, 170, 218, 216, 170, 139, 97.9, 65.8,
40.5, 30.1, 20.4, 16.63, 14.45, 12.83, 11.57, 9.753, 8.492, 7.563, 6.848,
6.281, 5.819, 5.435, 5.111, 4.593, 4.198, 3.886, 3.633, 3.172, 2.861, 2.637,
2.470, 2.236, 2.084, 1.978, 1.902, 1.845, 1.802, 1.769, 1.743, 1.706, 1.683,
1.669, 1.661, 1.655, 1.661, 1.672, 1.684, 1.712, 1.740, 1.766, 1.790, 1.812,
1.833, 1.852, 1.870, 1.902, 1.931, 1.956, 1.979, 2.029, 2.069, 2.104, 2.134,
2.185, 2.226, 2.257});

//energy of positrons in keV
gsl::vector positron_energy({10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0,
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

gsl::vector get_electron_energy()
//get energies of electrons for table in terms of dimensionless momentum squared
{
    gsl::vector ret=electron_energy.copy();
	ret/=energy_units_kev;

	//convert energies to momentum squared
	ret+=1.0; //now gamma
 	ret*=ret; //square
	ret-=1.0; //subtract one
	return ret;
}

gsl::vector get_electron_SP()
//get stopping powers of electrons for table as dimensionless force
{
    double conversion_factor=elementary_charge*1.0E8*density/(2.0*3.1415926*average_air_atomic_number*electron_classical_radius*electron_rest_energy*electron_classical_radius*air_molecular_density);

    gsl::vector ret=electron_SP.copy();
    ret*=conversion_factor;

	return ret;
}

//positrons not implemented

}


class electron_ionization_table
//class to calculate stopping powers for electrons
// has three modes:
//    1) do not remove moller losses: use default constructor and electron_lookup
//    2) remove moller losses, and minimum energy is constant:  use non_default constructor and electron_lookup
//    3) remove moller losses, and minimum energy is variable:  use default constructor and electron_lookup_variable_RML
{
public:

    gsl::vector electron_mom_sq;
    gsl::vector stopping_power;
    bool moller_removed;
    double min_mom_sq_for_moller;
    double min_energy;

    electron_ionization_table()
    {
        electron_mom_sq=bethe_table::get_electron_energy();
        stopping_power=bethe_table::get_electron_SP();
        min_mom_sq_for_moller=0.0;
        moller_removed=false;
        min_energy=0.0;
    }

    electron_ionization_table(double const_min_energy_dimensionless)
    //use this if the minimum energy is a constant
    {
        electron_mom_sq=bethe_table::get_electron_energy();
        stopping_power=bethe_table::get_electron_SP();

        for(size_t i=0; i<bethe_table::electron_energy.size(); i++)
        {
            if(electron_energy[i]>(const_min_energy_dimensionless*2.0*energy_units_kev))
            {
                stopping_power[i]-=moller_losses(electron_mom_sq[i]);
            }
        }

        min_mom_sq_for_moller=const_min_energy_dimensionless*2+1;
        min_mom_sq_for_moller*=min_mom_sq_for_moller;
        min_mom_sq_for_moller-=1.0;
        min_energy=const_min_energy_dimensionless;
        moller_removed=true;
    }

    double electron_lookup(double electron_mom_sq_)
    //give stopping power
	{
        if(electron_mom_sq_<electron_mom_sq[0])
        {
            throw gen_exception("electron momentum squared( ", electron_mom_sq_, ") below table");
        }
        else if(electron_mom_sq_>electron_mom_sq[electron_mom_sq.size()-1])
        {
            if(moller_removed and electron_mom_sq_>=min_mom_sq_for_moller)
            {
                return bethe_subtract_moller(electron_mom_sq_, min_energy);
            }
            else
            {
                return bethe_formula(electron_mom_sq_);
            }

        }
        else
        {
            size_t index=search_sorted_f(electron_mom_sq, electron_mom_sq_);
            return stopping_power[index] + (stopping_power[index+1] - stopping_power[index])*(electron_mom_sq[index]-electron_mom_sq_)/(electron_mom_sq[index]-electron_mom_sq[index+1]);
        }
	}

	double electron_lookup_variable_RML(double electron_mom_sq_, double min_energy)
	//use this if minimum energy can vary, and used default constructor
	{
	    double min_mom_sq_for_moller__=min_energy*2.0+1.0;
	    min_mom_sq_for_moller__*=min_mom_sq_for_moller__;
	    min_mom_sq_for_moller__-=1.0;

        if(electron_mom_sq_<electron_mom_sq[0])
        {
            throw gen_exception("electron momentum squared( ", electron_mom_sq_, ") below table");
        }
        else if(electron_mom_sq_>electron_mom_sq[electron_mom_sq.size()-1])
        {
            if(electron_mom_sq_>=min_mom_sq_for_moller__)
            {
                return bethe_subtract_moller(electron_mom_sq_, min_energy);
            }
            else
            {
                return bethe_formula(electron_mom_sq_);
            }

        }
        else
        {
            size_t index=search_sorted_f(electron_mom_sq, electron_mom_sq_);
            double SP=stopping_power[index] + (stopping_power[index+1] - stopping_power[index])*(electron_mom_sq[index]-electron_mom_sq_)/(electron_mom_sq[index]-electron_mom_sq[index+1]);
            if( electron_mom_sq_>=min_mom_sq_for_moller__)
            {
                return SP-=moller_losses(electron_mom_sq_, min_energy);
            }
            else
            {
                return SP;
            }
        }
	}
};


////positrons not implemented yet

#endif
