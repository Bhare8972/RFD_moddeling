
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
//#include "histogram.hpp"
#include "gen_ex.hpp"
#include "integrate.hpp"
#include "spline.hpp"
#include "rand.hpp"
#include "GSL_spline.hpp"
#include "chebyshev.hpp"

#include "../physics/shielded_coulomb_diffusion.hpp"
#include "../physics/particles.hpp"

class diffusion_table
{
    public:

    class energy_level
    {
        public:

        gsl::vector timesteps;
        std::vector< CDF_sampler > samplers;//not sure if these are thread-safe
        gsl::vector zero_theta_probabilities;

        energy_level(gsl::vector timesteps_, array_input& table_in)
        {
            timesteps=timesteps_;
            samplers.reserve(timesteps.size());
            zero_theta_probabilities=gsl::vector(timesteps.size());

            for(int i=0; i<timesteps.size(); i++)
            {
                array_input dist_X_table=table_in.get_array();
                auto samples=dist_X_table.read_doubles();

                //find number of zeros in samples
                int N_zeros=0;
                for( auto s : samples)
                {
                    if(s>0)
                    {
                        break;
                    }
                    else
                    {
                        N_zeros+=1;
                    }
                }
                zero_theta_probabilities[i]=double(N_zeros)/double(samples.size());


                //create the CDF, and decimate the data
                std::list<double> CDFx_list;
                std::list<double> CDFy_list;
                CDFx_list.push_back(0);
                CDFy_list.push_back(0);

                int decimation_factor=10;
                bool added_last=true;
                for(int cdfi=0; cdfi<(samples.size()-N_zeros); cdfi++)
                {
                    if( (cdfi+1)%decimation_factor ==0 )
                    {
                        CDFy_list.push_back( (cdfi+1.0)/(samples.size()-N_zeros) );
                        CDFx_list.push_back( samples[cdfi+N_zeros] );
                        added_last=true;
                    }
                    else
                    {
                        added_last=false;
                    }
                }
                if(not added_last)
                {
                    CDFy_list.push_back( 1.0 );
                    CDFx_list.push_back( samples.back() );
                }

                //create the walker aliased sampler.
                auto CDF_x=make_vector(CDFx_list);
                auto CDF_y=make_vector(CDFy_list);

                auto CDF_spline=linear_spline(CDF_x, CDF_y);
                samplers.emplace_back( CDF_spline );

                //test sampler
                /*
                if( i==10 )
                {
                    int N_samples=1000;
                    auto& sampler=samplers.back();
                    gsl::vector samples(N_samples);
                    rand_threadsafe rand;
                    for(int si=0; si<N_samples; si++)
                    {
                        samples[si]=sampler.sample( rand.uniform() );
                    }

                    arrays_output tables_out;
                    tables_out.add_doubles(samples);
                    tables_out.to_file("./tst_out");

                }*/

            }
        }

        double sample(double TS, double uniform_randA, double uniform_randB)
        {
            if(TS>=timesteps[timesteps.size()-1])
            {
                if( uniform_randA <  zero_theta_probabilities.back())
                {
                    return 0.0;
                }
                else
                {
                    return samplers.back().sample(uniform_randB);
                }

            }
            else
            {
                size_t TS_index=search_sorted_exponential(timesteps, TS);
                TS_index=closest_interpolate(timesteps[TS_index],TS_index,  timesteps[TS_index+1],TS_index+1,  TS);

                if( uniform_randA <  zero_theta_probabilities[TS_index])
                {
                    return 0.0;
                }
                else
                {
                    return samplers[TS_index].sample(uniform_randB);
                }
            }
        }
    };

    gsl::vector energies;
    gsl::vector timesteps;
    std::vector<energy_level> energy_samplers;
    rand_threadsafe rand;

    //stats:
    int fast_steps;
    int slow_steps_below_timestep;
    int slow_steps_above_energy;

    diffusion_table()
    {
        binary_input fin("./tables/shielded_coulomb_diffusion");
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

        fast_steps=0;
        slow_steps_below_timestep=0;
        slow_steps_above_energy=0;
    }

    inline double max_timestep()
    {
        return timesteps.back();
    }

    double sample(double energy, double timestep)
    {
        //assume energy and timestep are in bounds.

        if(timestep<=timesteps[0] or energy>=energies[energies.size()-1])
        {
            if(timestep<=timesteps[0]){slow_steps_below_timestep++;}
            else {slow_steps_above_energy++;}

            return resample(energy, timestep);
        }
        else
        {
            fast_steps++;

            size_t energy_i=search_sorted_d(energies, energy);
            energy_i=closest_interpolate(energies[energy_i],energy_i,  energies[energy_i+1],energy_i+1,  energy); //get closest energy

            double sample=energy_samplers[energy_i].sample(timestep, rand.uniform(),rand.uniform());

            return sample;
        }
    }

    double resample(double energy, double timestep)
    //find scattering angle by monte-carlo simulation. Note that this is slow, especilaly for low energy and large timesteps
    {
        diff_cross_section cross_section(energy);
        double expected_num_samples=cross_section.num_interactions_per_tau;

        long actual_num_samples=rand.poisson(expected_num_samples);
        if(actual_num_samples==0)
        {
            return 0.0;
        }


        gsl::vector T({0,0,1});
        for(size_t current_num_interactions=0; current_num_interactions<expected_num_samples; current_num_interactions++)
        {

            double inclination_scattering;
            double azimuth_scattering;
            {
                inclination_scattering=cross_section.sample( rand.uniform() ); //transform the sample
            }
            azimuth_scattering=rand.uniform()*2*PI;

            //calculate the three vector magnitudes
            double A=cos(inclination_scattering); //basis vector is original vector
            double B=sin(inclination_scattering)*cos(azimuth_scattering); //basis vector will be vector Bv below
            double C=-sin(inclination_scattering)*sin(azimuth_scattering); //basis vector will be vector Cv below

            //find vector Bv, perpinduclar to momentum
            gsl::vector init({1,0,0});
            gsl::vector Bv=cross(init, T);
            if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
            {
                init=gsl::vector({0,1,0}); //so we try a different init. momentum cannot be parrellel to both inits
                Bv=cross(init, T);
            }

            //normalize Bv
            Bv/=sqrt(Bv.sum_of_squares());

            //now we find Cv
            gsl::vector Cv=cross(Bv, T); //Bv and momentum are garenteed to be perpindicular,therefor Cv should have unit magnitude

            //find new vector
            T=A*T + B*Bv + C*Cv;
        }

        return acos(T[2]);
    }

    inline double sample_azimuth()
    {
        return rand.uniform()*2*PI;
    }

    inline void scatter(double energy, electron_T *particle)
    {
        double inclination=sample(energy, particle->timestep);
        particle->scatter_angle(inclination, sample_azimuth() );
    }

    void print_stats()
    {
        print("num. fast diffusion steps:", fast_steps);
        print("num. slow diffusion steps below timestep:", slow_steps_below_timestep);
        print("num. slow diffusion steps above energy:", slow_steps_above_energy);
    }
};

/*
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
};*/
#endif
