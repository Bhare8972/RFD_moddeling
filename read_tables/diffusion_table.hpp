
#ifndef DIFFUSION_TABLE
#define DIFFUSION_TABLE
#include <sstream>
#include <map>
#include <vector>
#include <gsl/gsl_rng.h>
#include <ctime>

#include "directories.hpp"
#include "arrays_IO.hpp"
#include "GSL_utils.hpp"
#include "histogram.hpp"
#include "gen_ex.hpp"

namespace
{
    size_t num_bins=20;
}

class diffusion_table
{
private:
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
                print("energy index:", i);
                (void)mom_squared; //does nothing. silance unused variable warning
                array_input samples_table=table_in.get_array();
                gsl::vector samples=samples_table.read_doubles();

                gsl::histogram hist(num_bins, 0, 2*3.1415926);
                hist.increment(samples);

                distributions.emplace_back(num_bins);
                distributions.back().init(hist);
                i++;
            }
        }

        double sample(double mom_squared)
        {
            size_t index=search_sorted_d(momentum_squareds, mom_squared);
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

        auto tables=list_dir_contents("./tables/diffusion");
        timesteps=make_vector(tables.size(), 0.0);
        time_tables.reserve(tables.size());
        size_t i=0;
        for(auto table_name : tables)
        {
            CHECK IS A FILE
            std::stringstream s;
            s<<table_name;
            s>>timesteps[i];//name of file should be timestep
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
        bool found=false;
        for(; timestep_ind<timesteps.size(); timestep_ind++)
        {
            if( float(timestep + (timestep-timesteps[timestep_ind])) == float(timestep) )
            {
                found=true;
                break;
            }
        }
        if(not found) throw gen_exception("timestep:",timestep," not in tables");

        return time_tables[timestep_ind].sample(mom_squared);
    }

    double sample_azimuth()
    {
        return gsl_rng_uniform(rand)*2*3.1415926;
    }

};
#endif
