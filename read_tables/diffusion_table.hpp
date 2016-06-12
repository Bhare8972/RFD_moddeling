
#ifndef DIFFUSION_TABLE
#define DIFFUSION_TABLE
#include <sstream>
#include <vector>
#include <gsl/gsl_rng.h>
#include <ctime>
#include <fstream>
#include <string>

#include "arrays_IO.hpp"
#include "GSL_utils.hpp"
#include "histogram.hpp"
#include "gen_ex.hpp"
#include "integrate.hpp"
#include "spline.hpp"
#include "rand.hpp"
#include "../physics/shielded_coulomb_diffusion.hpp"

/*
class diffusion_table
{
    public:

    class energy_level
    {
        public:

        gsl::vector timesteps;
        std::vector< poly_spline > samplers;//not sure if these are thread-safe

        energy_level(gsl::vector timesteps_, array_input& table_in)
        {
            timesteps=timesteps_;
            samplers.reserve(timesteps.size());
            for(int i=0; i<timesteps.size(); i++)
            {
                array_input dist_X_table=table_in.get_array();
                auto X=dist_X_table.read_doubles();

                array_input dist_Y_table=table_in.get_array();
                auto Y=dist_Y_table.read_doubles();

                samplers.emplace_back(X,Y);
            }
        }

        double sample(double TS, double uniform_rand)
        {
            size_t TS_index=search_sorted_exponential(timesteps, TS);
            //assume TS is below max(timesteps)
            double lower_linear_factor=(timesteps[TS_index]-TS)/(timesteps[TS_index]-timesteps[TS_index+1]);//factor for linear interpolation
            double lower_guess=samplers[TS_index].call(uniform_rand);
            double upper_guess=samplers[TS_index+1].call(uniform_rand);
            return lower_guess*lower_linear_factor + (1-lower_linear_factor)*upper_guess;
        }
    };

    gsl::vector energies;
    gsl::vector timesteps;
    std::vector<energy_level> energy_samplers;
    rand_threadsafe rand;

    diffusion_table(int file_mode) //0 to load files from both transform and MonteCarlo methods. 1 loads just transform, 2 loads just MonteCarlo
    {
        if(file_mode==0)
        {
            binary_input transform_fin("./tables/diffusion_table_TRANSFORM");
            array_input transform_table_in(transform_fin);

            array_input transform_energy_table=transform_table_in.get_array();
            auto transform_energies=transform_energy_table.read_doubles();

            array_input transform_timesteps_table=transform_table_in.get_array();
            auto transform_timesteps=transform_timesteps_table.read_doubles();



            binary_input MC_fin("./tables/diffusion_table_MONTECARLO");
            array_input MC_table_in(MC_fin);

            array_input MC_energy_table=MC_table_in.get_array();
            auto MC_energies=MC_energy_table.read_doubles();

            array_input MC_timesteps_table=MC_table_in.get_array();
            auto MC_timesteps=MC_timesteps_table.read_doubles();


            int N_energies=transform_energies.size()+MC_energies.size();
            //assume timesteps are the same
            timesteps=transform_timesteps;
            energies=gsl::vector(N_energies);
            energy_samplers.reserve(N_energies);

            AM HERE
            FILL energies VECTOR AND energy_samples

        }
        else if(file_mode==1)
        {
            binary_input fin("./tables/diffusion_table_TRANSFORM");
            array_input table_in(fin);

            array_input energy_table=table_in.get_array();
            energies=energy_table.read_doubles();

            array_input timesteps_table=table_in.get_array();
            timesteps=timesteps_table.read_doubles();

            energy_samplers.reserve(energies.size());
            for(int i=0; i<energies.size(); i++)
            {
                energy_samplers.emplace_back(timesteps, table_in);
            }
        }
        else if(file_mode==2)
        {
            binary_input fin("./tables/diffusion_table_MONTECARLO");
            array_input table_in(fin);

            array_input energy_table=table_in.get_array();
            energies=energy_table.read_doubles();

            array_input timesteps_table=table_in.get_array();
            timesteps=timesteps_table.read_doubles();

            energy_samplers.reserve(energies.size());
            for(int i=0; i<energies.size(); i++)
            {
                energy_samplers.emplace_back(timesteps, table_in);
            }
        }

    }

    inline double max_timestep()
    {
        return timesteps[timesteps.size()-1];
    }

    double sample(double energy, double timestep)
    {
        //assume energy and timestep are in bounds.

        if(timestep<=timesteps[0] or energy>=energies[energies.size()-1])
        {
            return resample(energy, timestep);
        }
        else
        {
            //maybe speed this up by only sampling closest energy

            double uniform_rand=rand.uniform();
            size_t energy_i=search_sorted_exponential(energies, energy);

            double lower_linear_factor=(energies[energy_i]-energy)/(energies[energy_i]-energies[energy_i+1]);
            double lower_sample=energy_samplers[energy_i].sample(timestep, uniform_rand);
            double upper_sample=energy_samplers[energy_i+1].sample(timestep, uniform_rand);

            return lower_linear_factor*lower_sample + (1-lower_linear_factor)*upper_sample;
        }
    }

    double resample(double energy, double timestep)
    //find scattering angle by monte-carlo simulation. Note that this is slow, especilaly for low energy and large timesteps
    {
        diff_cross_section cross_section(energy);
        return sample_timestep(timestep);
    }
};*/


class diffusion_table
{
private:

    //static const size_t num_bins=20;

    static
    std::vector<std::string> available_timesteps()
    //open up appropriate file and return the timestep files that are available.
    //keep timesteps in string format, to avoid floating point issues, which we need to use this as a file name later
    {
        std::ifstream input("./tables/diffusion/timesteps.txt");
        size_t num_timesteps=0;
        input>>num_timesteps;
        std::vector<std::string> output;
        output.resize(num_timesteps);
        for(size_t i=0; i<num_timesteps; i++)
        {
            input>>output[i];
        }
        return output; //slow copy. don't care.
    }

    class timestep_table
    {
    public:
        gsl::vector momentum_squareds;
        std::vector<gsl::histogram::pdf> distributions;
        gsl_rng* rand;//do not own this rand

        timestep_table(gsl_rng* rand_, std::string table_name)
        {
            rand=rand_;

            binary_input fin(table_name);
            array_input table_in(fin);

            array_input energy_table=table_in.get_array();
            auto energies=energy_table.read_doubles();
            energies*=1000*elementary_charge/electron_rest_energy; //is in keV, convert to dimensionless units
            //convert to momentum
            energies+=1;
            momentum_squareds=(energies*energies) - 1.0;

            distributions.reserve(energies.size());

            size_t i=1;
            for(double mom_squared : momentum_squareds)
            {
                (void)mom_squared; //does nothing. silance unused variable warning
                array_input distribution_table=table_in.get_array();
                array_input ranges_table=distribution_table.get_array();
                gsl::vector ranges=ranges_table.read_doubles();
                array_input values_table=distribution_table.get_array();
                gsl::vector values=values_table.read_doubles();

                gsl::histogram hist(ranges, values);
                distributions.emplace_back(values.size());
                distributions.back().init(hist);
                i++;
            }
        }

        double sample(double mom_squared)
        {

            size_t index;
            try
            {
                index=search_sorted_d(momentum_squareds, mom_squared);
            }
            catch(gen_exception)
            {
                if(mom_squared<momentum_squareds[0])
                {
                    throw gen_exception("momentum squared( ", mom_squared, ") below diffusion table");
                }
                else
                {
                    throw gen_exception("momentum squared( ", mom_squared, ") above diffusion table");
                }
            }


            double R=gsl_rng_uniform(rand);
            double low_sample=distributions[index].sample(R);
            double high_sample=distributions[index+1].sample(R);
            return high_sample + (mom_squared-momentum_squareds[index+1])*(low_sample-high_sample)/(momentum_squareds[index]-momentum_squareds[index+1]);//do linear interpolation??
        }
    };

public:

    gsl::vector timesteps;
    std::vector<timestep_table> time_tables;

    gsl_rng* rand;//own this rand

    diffusion_table(bool rnd_seed=false)
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


        std::vector<std::string> table_fnames=available_timesteps();
        timesteps=make_vector(table_fnames.size(), 0.0);
        time_tables.reserve(table_fnames.size());
        size_t i=0;
        for(auto table_name : table_fnames)
        {
            std::stringstream s;
            s<<table_name;
            s>>timesteps[i];//name of file is a   timestep
            std::stringstream S2;
            S2<<"./tables/diffusion/"<<table_name;
            time_tables.emplace_back(rand, S2.str());
            i++;
        }
    }

    ~diffusion_table()
    {
		gsl_rng_free(rand);
    }

    double sample_inclination(double mom_squared, double timestep)
    {
        //find right timestep, using linear search becouse we want only exact match, and there may only be one element
        //maybe fix this in future

        size_t timestep_ind=0;
        if(timestep<timesteps[0])
        {
            timestep_ind=0; //for the redudant department of redundancy
        }
        else if(timestep>=timesteps[timesteps.size()-1])
        {
            timestep_ind=timesteps.size()-1;
        }
        else
        {
            timestep_ind=search_sorted_d(timesteps, timestep);
        }

        return time_tables[timestep_ind].sample(mom_squared);
    }

    double sample_azimuth()
    {
        return gsl_rng_uniform(rand)*2*3.1415926;
    }

};
#endif
