
#include <string>
#include <cmath>
#include "arrays_IO.hpp"
#include "GSL_utils.hpp"

#ifndef IONIZATION_TABLE
#define IONIZATION_TABLE

class ionization_table
{
private:
    bool remove_moller_losses;
	gsl::vector_float electron_mom_sq;
	gsl::vector electron_SP;
	gsl::vector_float positron_mom_sq;
	gsl::vector positron_SP;

public:
	ionization_table(bool remove_moller_losses_=true)
	{
        std::string fname("./tables/ionization_losses");
        remove_moller_losses=remove_moller_losses_;
        if(remove_moller_losses_)
        {
            fname="./tables/ionization_losses_RML";
        }
		binary_input fin(fname);
		array_input table_in(fin);

		array_input electron_mom_sq_table=table_in.get_array();
		electron_mom_sq=electron_mom_sq_table.read_floats();

		array_input electron_SP_table=table_in.get_array();
		electron_SP=electron_SP_table.read_doubles();

		array_input positron_mom_sq_table=table_in.get_array();
		positron_mom_sq=positron_mom_sq_table.read_floats();

		array_input positron_SP_table=table_in.get_array();
		positron_SP=positron_SP_table.read_doubles();
	}

    double beth_formula(double mom_sq)
    {
        double gamma_sq=1.0+mom_sq;
        double gamma=sqrt(gamma_sq);
        double beta_sq=mom_sq/gamma;
        double KE=gamma-1;

        double exp_term1=beta_sq*KE*gamma_sq*inv_I_sq;
        double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
        double term3=KE*KE/(8.0*gamma_sq) + 1.0/gamma_sq;

        return (log(exp_term1)-term2_factor*log(2)+term3)/beta_sq;
    }

    double beth_RML_formula(double mom_sq)
    //note that this depends on minimum energy, which may be variable in the future
    {
        double gamma_sq=1.0+mom_sq;
        double gamma=sqrt(gamma_sq);
        double beta_sq=mom_sq/gamma;
        double KE=gamma-1;

        double exp_term1=2*minimum_energy*beta_sq*gamma_sq*inv_I_sq;
        double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
        double exp_term2_factor2=KE/(KE-minimum_energy);
        double term3=minimum_energy/(KE-minimum_energy)-beta_sq;
        double term4=minimum_energy*minimum_energy/(2*gamma_sq);

        return (log(exp_term1)-term2_factor*log(exp_term2_factor2)+term3+term4)/beta_sq;
    }

	double electron_lookup(double electron_mom_sq_)
	{
		//get location
        size_t index;
        try
        {
		    index=search_sorted_f(electron_mom_sq, electron_mom_sq_);
        }
        catch(gen_exception)
        {
            if(electron_mom_sq_<electron_mom_sq[0])
            {
                throw gen_exception("electron momentum squared( ", electron_mom_sq_, ") below table");
            }
            else
            {
                //throw gen_exception("electron momentum squared( ", electron_mom_sq_, ") above table");
                return beth_RML_formula(electron_mom_sq_);//we are assuming we are about minimum energy for now...
            }
        }


		//do linear interpolation
		return electron_SP[index] + (electron_SP[index+1] - electron_SP[index])*(electron_mom_sq[index]-electron_mom_sq_)/(electron_mom_sq[index]-electron_mom_sq[index+1]);
	}

	double positron_lookup(double positron_mom_sq_)
	{
        //get location
        size_t index;
        try
        {
		    index=search_sorted_f(positron_mom_sq, positron_mom_sq_);
        }
        catch(gen_exception)
        {
            if(positron_mom_sq_<positron_mom_sq[0])
            {
                throw gen_exception("positron momentum squared( ", positron_mom_sq_, ") below table");
            }
            else
            {
                //throw gen_exception("electron momentum squared( ", electron_mom_sq_, ") above table");
                return beth_formula(positron_mom_sq_);//what to do here?
            }
        }

		//do linear interpolation
		return positron_SP[index] + (positron_SP[index+1] - positron_SP[index])*(positron_mom_sq[index]-positron_mom_sq_)/(positron_mom_sq[index]-positron_mom_sq[index+1]);
	}
};

#endif
