
#ifndef MOLLER_TABLE
#define MOLLER_TABLE

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <ctime>
#include <vector>

#include "arrays_IO.hpp"
#include "GSL_utils.hpp"
#include "gen_ex.hpp"



class moller_table
{
public:
    gsl::vector energies;
    gsl::vector num_interactions_per_tau;
    std::vector<poly_spline> splines;

    size_t current_index;
    double energy_factor; //needed for linear interpolation
    gsl_rng* rand;

    moller_table(bool rnd_seed=false)
    {
        rand=gsl_rng_alloc(gsl_rng_mt19937);
		if(rnd_seed)
		{
		    unsigned int seed_val( time(NULL) );
		    gsl_rng_set(rand, seed_val);
		}
		else
        {
            gsl_rng_set(rand, 0);
        }

        binary_input fin("./tables/moller");
        array_input table_in(fin);

        array_input energy_table=table_in.get_array();
        energies=energy_table.read_doubles();

        array_input interactions_table=table_in.get_array();
        num_interactions_per_tau=interactions_table.read_doubles();

        splines.reserve(energies.size());

        for(size_t i=0; i<num_interactions_per_tau.size(); i++)
        {
            array_input diffusion_table=table_in.get_array();

            array_input points_table=diffusion_table.get_array();
            auto points=points_table.read_doubles();

            array_input cumquads_table=diffusion_table.get_array();
            auto cumquads=cumquads_table.read_doubles();

            splines.emplace_back(cumquads, points);
        }

        set_energy(energies[0]);
    }

    ~moller_table()
    {
        gsl_rng_free(rand);
    }

    double lowest_energy()
    {
        return energies[0];
    }

    void set_energy(double energy)
    {
        try
        {
            current_index=search_sorted_d(energies, energy);
        }
        catch(gen_exception)
        {
            if(energy<energies[0])
            {
                throw gen_exception("energy( ", energy, ") below moller table");
            }
            else
            {
                throw gen_exception("energy( ", energy, ") above moller table");
            }
        }

        energy_factor=(energy-energies[current_index])/(energies[current_index+1]-energies[current_index]);
    }

    size_t num_interactions(double timestep)
    {
        double linear_interactions_expected=num_interactions_per_tau[current_index]*(1-energy_factor) + num_interactions_per_tau[current_index+1]*energy_factor;//linear interpolation
        return gsl_ran_poisson(rand, linear_interactions_expected*timestep);
    }

    double sample_new_energy()
    {
        double U=gsl_rng_uniform(rand);
        double A=splines[current_index].call(U);
        double B=splines[current_index+1].call(U);
        return A*(1-energy_factor) + B*energy_factor;//linear interpolation between the two tables
    }

    double sample_azimuth()
    {
        return gsl_rng_uniform(rand)*2*3.1415926;
    }
};
#endif



