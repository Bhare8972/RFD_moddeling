

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_integration.h>
#include <ctime>
#include <list>
#include <thread>
#include <mutex>
#include <sstream>
#include <memory>

#include "vector.hpp"
#include "vector_long.hpp"
//#include "histogram.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "spline_2D.hpp"
#include "gen_ex.hpp"

#include "../physics/shielded_coulomb_diffusion.hpp"

using namespace std;

class hist_tool
{
private:
    class bin
    {
    public:
        double num_counts;
        double lower_range;
        double upper_range;
        list<double> values;
        bool splittable;

        bin(double lower, double upper)
        {
            lower_range=lower;
            upper_range=upper;
            num_counts=0;
            splittable=true;
        }

        bin(bin& splice_from, int I)
        {
            I++;//don't worry about it
            lower_range=(splice_from.lower_range+splice_from.upper_range)/2.0;
            upper_range=splice_from.upper_range;
            splice_from.upper_range=lower_range;
            auto iter=splice_from.values.begin();
            for( ; iter!=splice_from.values.end(); ++iter)
            {
                if((*iter)>=lower_range) break;
            }
            values.splice(values.begin(), splice_from.values, iter, splice_from.values.end());
            num_counts=values.size();
            splice_from.num_counts=splice_from.values.size();
            //print(" lower:", splice_from.lower_range, splice_from.upper_range);
            //print(" upper:", lower_range, upper_range);

            double V=(lower_range+upper_range)/2.0;
            if(V!=lower_range and V!=upper_range)
            {
                splittable=false;
            }
            else
            {
                splittable=true;
            }
        }

        bool increment(double value)
        {
            if(value>=lower_range and value<upper_range)
            {
                if(num_counts==0)
                {
                    values.push_back(value);
                }
                else if(value<values.front())
                {
                    values.push_front(value);
                }
                else if(value>=values.back())
                {
                    values.push_back(value);
                }
                else
                {
                    for(auto iter=(++values.begin()); iter!=values.end(); ++iter)
                    {
                        if(value<(*iter))
                        {
                            --iter;
                            values.insert(iter, value);
                            break;
                        }
                    }
                }

                num_counts++;
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    list<bin> hist_bins;

public:

    typedef list<bin>::iterator iterator;
    std::mutex hist_mutex;

    hist_tool(double start, double stop, size_t num_bins)
    {
        auto bin_ranges=linspace(start, stop, num_bins+1);
        for(size_t bin_i=0; bin_i<num_bins; bin_i++)
        {
            hist_bins.emplace_back(bin_ranges[bin_i], bin_ranges[bin_i+1]);
        }
    }

    hist_tool(const hist_tool& COPY)
    {
        throw gen_exception("not implemented");
    }

    iterator begin()
    {
        return hist_bins.begin();
    }

    iterator end()
    {
        return hist_bins.end();
    }

    void increment(double value)
    {
        std::lock_guard<std::mutex> lock(hist_mutex);
        for(bin &B : hist_bins)
        {
            if(B.increment(value)) break;
        }
    }

    void split(iterator IT)
    {
        std::lock_guard<std::mutex> lock(hist_mutex);
        iterator IN=IT;
        hist_bins.emplace(++IN, *IT, 0);
    }

    gsl::vector get_bin_ranges()
    {
        std::lock_guard<std::mutex> lock(hist_mutex);
        gsl::vector out(hist_bins.size()+1);

        size_t i=0;
        for(bin &B : hist_bins)
        {
            out[i]=B.lower_range;
            i++;
        }

        out[hist_bins.size()]=hist_bins.back().upper_range;

        return out;
    }

    gsl::vector get_bin_values()
    {
        std::lock_guard<std::mutex> lock(hist_mutex);
        gsl::vector out(hist_bins.size());

        size_t i=0;
        for(bin &B : hist_bins)
        {
            out[i]=B.num_counts;
            i++;
        }

        return out;
    }

    size_t num_bins()
    {
        return hist_bins.size();
    }
};

class energy_level_workspace
{
public:
	diff_cross_section cross_section;//this is thread safe, I think
	double energy;

    gsl::vector timesteps;
    gsl::vector num_interactions_per_timestep;

	list<thread> threads;

    std::vector<hist_tool> distributions;//hist tool needs to be made thread safe


	energy_level_workspace(gsl::vector timesteps_, double energy_, size_t num_bins_) : cross_section(energy_)//, distribution(0, 3.1415926, num_bins_)
	{
	    energy=energy_;
	    timesteps=timesteps_;

	    num_interactions_per_timestep=timesteps*cross_section.num_interactions_per_tau;
	    //gsl::vector(timesteps.size());
	    //LOOP(num_interactions_per_timestep, A, timesteps,  cross_section.num_interactions_per_tau*A);

        distributions.reserve(timesteps.size());
        for(auto TS : timesteps)
        {
            (void)TS;
            distributions.emplace_back(0, PI, num_bins_);
        }
	}

	void sample_timesteps()
	//note simularity to sampler in cross_section. Except that it samples for multiple timesteps
	{
	    //get our random samples
	    gsl::vector_long num_samples(timesteps.size());
	    LOOP(num_samples, A, num_interactions_per_timestep, cross_section.rand.poisson(A) );
        long max_num_samples=num_samples.max();

        gsl::vector T({0,0,1});

        for(size_t current_num_interactions=1; current_num_interactions<=max_num_samples; current_num_interactions++)
        {
            double inclination_scattering;
            double azimuth_scattering;
            {
                std::lock_guard<std::mutex> lock(cross_section.spline_sampler_mutex);
                inclination_scattering=cross_section.spline_sampler->call( cross_section.rand.uniform() ); //transform the sample
            }
            azimuth_scattering=cross_section.rand.uniform()*2*PI;

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
            gsl::vector Cv=cross(Bv, T); //Bv and momentum are garenteed to be perpindicular.

            //give Bv correct magnitude
            //Bv*=sqrt(momentum_squared); //correct magnitude is 1

            //find new vector
            T=A*T + B*Bv + C*Cv;

            for(int timestep_j=0; timestep_j<timesteps.size(); timestep_j++)
            {
                if(current_num_interactions==num_samples[timestep_j])
                {
                    distributions[timestep_j].increment(acos(T[2]));
                }
            }
        }
	}

	inline void multi_samples(size_t N)
	{
	    for(size_t i=0; i<N; i++)
	    {
            sample_timesteps();
	    }
	}

	void start_thread(size_t samples_perThread_perRun, size_t N_threads, double percent_error)
	{
        if(threads.size() !=0 ) throw gen_exception("must join before starting more threads");

        //sample for the required number of times
        for(size_t i=0; i<N_threads; i++)
        {
            threads.push_back( thread(&energy_level_workspace::multi_samples, this, samples_perThread_perRun) );
            //multi_samples(samples_per_thread, i*samples_per_thread);
        }

        for(auto& T : threads)
        {
            T.join();
        }
        threads.clear();


        bool keep_running=true;
        size_t n_runs=1;

        std::vector< gsl::vector > old_distributions(timesteps.size());

        while(keep_running)
        {
            for(int timestep_i=0; timestep_i<timesteps.size(); timestep_i++)
            {
                old_distributions[timestep_i]=distributions[timestep_i].get_bin_values();
            }

            //sample for the required number of times
            for(size_t i=0; i<N_threads; i++)
            {
                threads.push_back( thread(&energy_level_workspace::multi_samples, this, samples_perThread_perRun) );
                //multi_samples(samples_per_thread, i*samples_per_thread);
            }

            for(auto& T : threads)
            {
                T.join();
            }
            threads.clear();

            //estimate error
            // note that this could use some better mathematical treatment, but it seems to work well

            double error_factor=0;
            //int max_timestep_i=0;

            for(int timestep_i=0; timestep_i<timesteps.size(); timestep_i++)
            {
                gsl::vector new_distribution=distributions[timestep_i].get_bin_values();

                for(size_t i=0; i<new_distribution.size(); i++)
                {
                    double N_pnts=new_distribution[i];
                    if(N_pnts==0)
                    {
                        continue;
                    }

                    double DH=N_pnts-old_distributions[timestep_i][i];
                    double next_error_factor=DH/N_pnts;

                    if(next_error_factor>error_factor)
                    {
                        error_factor=next_error_factor;
                        //max_timestep_i=timestep_i;
                    }
                }

            }


            print(energy, ":", (n_runs+1)*samples_perThread_perRun*N_threads, "  error:", error_factor, "desired error:", percent_error);
            //print("timestep with max error ==  max_timestep:", max_timestep_i==(timesteps.size()-1), ". if always true, optimize!");
            //never true
            print();

            if(error_factor<percent_error)
            {
                keep_running=false;
            }

            for(int timestep_i=0; timestep_i<timesteps.size(); timestep_i++)
            {
                for(auto iter=distributions[timestep_i].begin(); (++iter)!=distributions[timestep_i].end(); ++iter)
                {
                    double next_H=iter->num_counts;
                    --iter;
                    double H=iter->num_counts;

                    if(iter->splittable and abs(next_H-H)>3*(sqrt(next_H)+sqrt(H)))
                    {
                        keep_running=true;
                        distributions[timestep_i].split(iter);

                        //should this next break be here??
                        break; //WHY IS THIS HERE?
                    }
                }
            }

            n_runs++;
        }
	}


    void output(arrays_output& tables_out)
    {
        for(auto& distribution : distributions)
        {
            auto bin_ranges=distribution.get_bin_ranges();
            auto bin_values=distribution.get_bin_values();

            auto dist_values=cumsum(bin_values, true);
            dist_values/=dist_values[dist_values.size()-1]; //normalize to values from 0 to 1

            gsl::vector quad_X;
            gsl::vector quad_Y;
            make_fix_spline(dist_values, bin_ranges, quad_X, quad_Y);// fix if points are singular

            shared_ptr<doubles_output> X_dist_table=make_shared<doubles_output>(quad_X);
            tables_out.add_array(X_dist_table);

            shared_ptr<doubles_output> Y_dist_table=make_shared<doubles_output>(quad_Y);
            tables_out.add_array(Y_dist_table);
        }
    }

};

int main()
{
	double min_energy=100.0/energy_units_kev;  //100.0/energy_units_kev;
	bool skip_first_energy=true; //if this is true, we do not cover the lower energy, which may be handeled by a different simulation
	double max_energy=100000.0/energy_units_kev; //100000/energy_units_kev;
	int num_energies=100; //100;

    double min_timestep=6e-7;
    double max_timestep=0.01;
    size_t num_timesteps=20;//1000

	size_t threads_per_energy=2; //num threads per energy
	size_t num_samples_per_energy_per_thread_per_run=400/threads_per_energy;
	size_t num_bins=20; //controlls precision of final distribution
	double error_percent=0.15; //controlls error of bins in y-direction

	gsl::vector energy_vector=logspace(log10(min_energy), log10(max_energy), num_energies+1);
    gsl::vector timesteps=logspace(log10(min_timestep), log10(max_timestep), num_timesteps);




    //start procesing for each energy
    std::list<energy_level_workspace> samplers;
    for(int energy_i=skip_first_energy; energy_i<energy_vector.size(); energy_i++) //note that we may skip the first energy
    {
        print("starting energy:", energy_vector[energy_i], "(", energy_i, "/", num_energies, ")");

        samplers.emplace_back(timesteps, energy_vector[energy_i], num_bins);
        samplers.back().start_thread(num_samples_per_energy_per_thread_per_run, threads_per_energy, error_percent);

    }

    print("writing to file");

    //write to file
    arrays_output tables_out;
    shared_ptr<doubles_output> energies_table=make_shared<doubles_output>(energy_vector);
    tables_out.add_array(energies_table);
    shared_ptr<doubles_output> timesteps_table=make_shared<doubles_output>(timesteps);
    tables_out.add_array(timesteps_table);

    for(energy_level_workspace& W : samplers)
    {
        W.output(tables_out);
    }

	binary_output fout("./diffusion_table_MONTECARLO");
	tables_out.write_out( &fout);

}
