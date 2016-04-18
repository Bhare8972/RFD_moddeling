
#include "arrays_IO.hpp"
#include "GSL_utils.hpp"

#ifndef IONIZATION_TABLE
#define IONIZATION_TABLE

class ionization_table
{
private:
	gsl::vector_float electron_mom_sq;
	gsl::vector electron_SP;
	gsl::vector_float positron_mom_sq;
	gsl::vector positron_SP;

public:
	ionization_table()
	{
		binary_input fin("./tables/ionization_losses");
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

	double electron_lookup(double electron_mom_sq_)
	{
		//get location
		size_t index=search_sorted_f(electron_mom_sq, electron_mom_sq_);

		//do linear interpolation
		return electron_SP[index] + (electron_SP[index+1] - electron_SP[index])*(electron_mom_sq[index]-electron_mom_sq_)/(electron_mom_sq[index]-electron_mom_sq[index+1]);
	}

	double positron_lookup(double positron_mom_sq_)
	{
		//get location
		size_t index=search_sorted_f(positron_mom_sq, positron_mom_sq_);

		//do linear interpolation
		return positron_SP[index] + (positron_SP[index+1] - positron_SP[index])*(positron_mom_sq[index]-positron_mom_sq_)/(positron_mom_sq[index]-positron_mom_sq[index+1]);
	}
};

#endif
