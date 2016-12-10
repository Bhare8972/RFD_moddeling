

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>
#include <list>
#include <thread>
#include <mutex>

#include <gsl/gsl_sort_vector_long.h>
#include <gsl/gsl_cdf.h>

#include "vector.hpp"
#include "vector_long.hpp"
#include "permutation.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"
#include "root_finding.hpp"

#include "../physics/shielded_coulomb_diffusion.hpp"


class hist_tool
{
private:
    class bin
    {
    public:
        double num_counts;
        double lower_range;
        double upper_range;
        std::list<double> values;
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

            //CHECK VALIDITY
            if(num_counts>10 and splice_from.num_counts==0)
            {
                print("from:", lower_range, "to", upper_range);
                for(double V: values)
                { print(V); }
                print("none from", splice_from.lower_range, "to", splice_from.upper_range);
                for(double V: splice_from.values)
                { print(V); }
                //throw gen_exception("YOa");
                splittable=false;
                splice_from.splittable=false;
            }
            else if(splice_from.num_counts>10 and num_counts==0)
            {
                print("from:", splice_from.lower_range, "to", splice_from.upper_range);
                for(double V: splice_from.values)
                { print(V); }
                print("none from", lower_range, "to", upper_range);
                for(double V: values)
                { print(V); }
                //throw gen_exception("YOb");
                splittable=false;
                splice_from.splittable=false;
            }

           /*
            double min_this=upper_range;
            double max_this=lower_range;
            for( double V : values )
            {
                if(V<min_this)
                {
                    min_this=V;
                }
                if(V>max_this);
                {
                    max_this=V;
                }
            }

            double min_old=splice_from.upper_range;
            double max_old=splice_from.lower_range;
            for( double V : splice_from.values )
            {
                if(V<min_old)
                {
                    min_old=V;
                }
                if(V>max_old);
                {
                    max_old=V;
                }
            }

            if(min_this<lower_range or (max_this>upper_range) or (min_old<splice_from.lower_range) or (max_old>splice_from.upper_range))
            {
                print("Range error:");
                print("NEW BIN: from", lower_range, "to", upper_range,". has", num_counts, "counts from", min_this, "to", max_this);
                print("OLD BIN: from", splice_from.lower_range, "to", splice_from.upper_range,". has", splice_from.num_counts, "counts from", min_old, "to", max_old);
                throw gen_exception("RE");
            }
            */




            //double V=(lower_range+upper_range)/2.0;
            //if( (upper_range-lower_range)<0.022)
            //{
             //   splittable=false;
             //   splice_from.splittable=false;
            //}
            //else
            //{
                //splittable=true;
                //splice_from.splittable=true;
            //}
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
                            values.insert(iter, value);
                            break;
                        }
                    }

                    //CHECK SORT
                    //if(values.size()>1)
                    //{
                    //    double last_value=values.front();

                    //    for(auto iter=(++values.begin()); iter!=values.end(); ++iter)
                    //    {
                    //        if( not ((*iter)>=last_value) )
                    //        {
                    //            print("no insert value", value );
                    //            for(double V: values)
                    //            {
                    //                print(V);
                    //            }
                    //            throw gen_exception("YO MY WOOGIE!!");
                    //        }
                    //        last_value=(*iter);
                    //    }
                    //}

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

    std::list<bin> hist_bins;

public:

    typedef std::list<bin>::iterator iterator;
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

    void dump_data()
    {
        arrays_output output;

        std::list<double> all_values;
        for(bin &B : hist_bins)
        {
            for(double V: B.values)
            {
                all_values.push_back(V);
            }
        }

        output.add_doubles( make_vector(all_values) );
        output.to_file("./error_dump");
    }
};


class std_bars : functor_1D
{
public:
    long num_samples;
    double lower_lambda;
    double upper_lambda;

    std_bars(long _num_samples)
    {
        num_samples=_num_samples;
        if(_num_samples==0)
        {
            lower_lambda=-0.000001;
        }
        else
        {
            upper_lambda=1.0-0.16;
            lower_lambda=root_finder_brent(this, 10.0*(num_samples+1), 0.0001);
        }

        upper_lambda=0.16;
        upper_lambda=root_finder_brent(this, 10.0*(num_samples+1), 0.0001);

        //print(num_samples, lower_lambda, upper_lambda);
    }

    double call(double lambda_tst)
    {
        return gsl_cdf_poisson_P(num_samples, lambda_tst)-upper_lambda;
    }
};

std::vector<std_bars> one_std_errors;

class energy_level_workspace
{
public:
	diff_cross_section cross_section;//this is thread safe
	rand_threadsafe rand;
	double energy;

    gsl::vector timesteps;
    gsl::vector num_interactions_per_timestep;

    std::vector<hist_tool> distributions;//hist tool should be thread safe

	energy_level_workspace(gsl::vector timesteps_, double energy_, size_t num_bins_over_three) : cross_section(energy_)//, distribution(0, 3.1415926, num_bins_)
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
            distributions.emplace_back(0, PI, num_bins_over_three*3);
        }
	}

	void sample_timesteps()
	{
	    //get our random samples
	    gsl::vector_long num_samples(timesteps.size());
	    LOOP(num_samples, A, num_interactions_per_timestep, rand.poisson(A) );

	    gsl::permutation num_samples_sorter(num_samples.size(), true);
	    gsl_sort_vector_long_index(num_samples_sorter.get(), num_samples.get()); //store sorted values in num_samples_sorter

	    long current_num_sample_index=0;
        long max_num_samples=num_samples[ num_samples_sorter[ num_samples.size()-1 ] ];

        gsl::vector T({0,0,1});

        size_t current_num_interactions=0;
        for(; current_num_interactions<=max_num_samples; current_num_interactions++)
        {
            while( current_num_interactions== num_samples[ num_samples_sorter[current_num_sample_index] ] ) //do this fist, becouse there could be a timestep with 0 interactions
            {
                double Z=acos(T[2]);
                if(Z!=0)
                {
                    distributions[ num_samples_sorter[current_num_sample_index] ].increment(Z);
                }
                current_num_sample_index++;
                if(current_num_sample_index==num_samples.size()) { break; }
            }

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
        //print( "tst:", max_num_samples, T.sum_of_squares() );
	}

	inline void multi_samples(size_t N)
	{
	    for(size_t i=0; i<N; i++)
	    {
            sample_timesteps();
	    }
	}

	void start_thread(size_t samples_perThread_perRun, size_t N_threads, long min_size_of_max_bin)
	{
    //// run for some initial number of runs
        long N_samples=0;

        std::list<std::thread> threads;
        for(size_t i=0; i<N_threads; i++)
        {
            threads.push_back( std::thread(&energy_level_workspace::multi_samples, this, samples_perThread_perRun) );
            //multi_samples(samples_per_thread, i*samples_per_thread);
        }
        for(auto& T : threads)
        {
            T.join();
        }
        threads.clear();
        N_samples+=samples_perThread_perRun*N_threads;


    ////run until all conditions are met////
        int run_i=0;
        while(true)
        {

        ////first, we sample until at least on bin at every timestep has at least "min_size_of_max_bin" number of points, and all bins include at least one sample
            long smallest_of_largest_bin;
            while( true )
            {
            //tst if we need samples
                bool all_hist_good=true; //remain true if all histograms have requisite number of points
                smallest_of_largest_bin=min_size_of_max_bin*100;
                for(hist_tool& histogram : distributions)
                {
                    long largest_of_hist=0;
                    bool this_hist_has_large_bin=false;
                    //bool all_bins_over_1=true;
                    for(auto& bin : histogram)
                    {
                        if(bin.num_counts>largest_of_hist)
                        {
                            largest_of_hist=bin.num_counts;
                        }

                        if(bin.num_counts>min_size_of_max_bin) //we found a large enough bin!
                        {
                            this_hist_has_large_bin=true;
                            break;
                        }

                        //if(bin.num_counts<1)
                        //{
                        //    all_bins_over_1=false;
                        //    break;
                        //}
                    }

                    if(largest_of_hist<smallest_of_largest_bin)
                    {
                        smallest_of_largest_bin=largest_of_hist;
                    }

                    if(not (this_hist_has_large_bin))// and all_bins_over_1))
                    {
                        all_hist_good=false;
                        break;
                    }
                }

                if( all_hist_good ) {break;}

                //now we sample some number of times
                for(size_t i=0; i<N_threads; i++)
                {
                    threads.push_back( std::thread(&energy_level_workspace::multi_samples, this, samples_perThread_perRun) );
                    //multi_samples(samples_per_thread, i*samples_per_thread);
                }
                for(auto& T : threads)
                {
                    T.join();
                }

                N_samples+=samples_perThread_perRun*N_threads;
                threads.clear();
                print("  sample complete:", 100.0*double(smallest_of_largest_bin)/min_size_of_max_bin);
            }

            print("  run",run_i, ". Sampling approx.", 100.0*double(smallest_of_largest_bin)/min_size_of_max_bin, "% complete. Energy:", energy*energy_units_kev);
            print(" ",N_samples,"samples");


        ////now we test to see if all bins that are inbetween two bins are approxemetly linear
            bool did_split=false;
            int hist_i=0;
            for(hist_tool& histogram : distributions)
            {
                auto low_bin_iter=histogram.begin();
                auto middle_bin_iter=histogram.begin();
                ++middle_bin_iter;
                auto high_bin_iter=histogram.begin();
                ++high_bin_iter;
                ++high_bin_iter;

                int hist_bin_i=1;
                while(high_bin_iter!=histogram.end())
                {

                    if((not low_bin_iter->splittable) or (not middle_bin_iter->splittable) or (not high_bin_iter->splittable))
                    {
                        ++low_bin_iter;
                        ++middle_bin_iter;
                        ++high_bin_iter;

                        hist_bin_i++;
                        continue;
                    }

                    auto L_var=one_std_errors[ low_bin_iter->num_counts ];
                    auto M_var=one_std_errors[ middle_bin_iter->num_counts ];
                    auto H_var=one_std_errors[ high_bin_iter->num_counts ];

                    double ave_low=(L_var.lower_lambda+H_var.lower_lambda)/2.0;
                    double ave_high=(L_var.upper_lambda+H_var.upper_lambda)/2.0;

                    if( (ave_low>M_var.upper_lambda) or (ave_high<M_var.lower_lambda) )
                    {
                        did_split=true;
                        print("  split hist", hist_i, "mid bin", hist_bin_i);
                        print("   old:", low_bin_iter->num_counts, middle_bin_iter->num_counts, high_bin_iter->num_counts);
                        histogram.split(low_bin_iter);
                        histogram.split(middle_bin_iter);
                        histogram.split(high_bin_iter);
                        print("   new:", low_bin_iter->num_counts, middle_bin_iter->num_counts, high_bin_iter->num_counts);
                        //print("   bounds: (",low_bin_iter->lower_range, low_bin_iter->upper_range,") (",middle_bin_iter->lower_range, middle_bin_iter->upper_range,") (",high_bin_iter->lower_range, high_bin_iter->upper_range,")");

                        if( (not low_bin_iter->splittable) or (not middle_bin_iter->splittable) or (not high_bin_iter->splittable))
                        {
                            histogram.dump_data();
                            throw gen_exception("data dumped");
                        }

                        break; //do not continue spliting bins from this hist.
                    }

                    ++low_bin_iter;
                    ++middle_bin_iter;
                    ++high_bin_iter;

                    hist_bin_i++;
                }
                hist_i++;
            }

            if(not did_split)
            {
                return;
            }

            run_i++;
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

            auto X_dist_table=std::make_shared<doubles_output>(bin_ranges);
            tables_out.add_array(X_dist_table);

            auto Y_dist_table=std::make_shared<doubles_output>(bin_values);
            tables_out.add_array(Y_dist_table);
        }
    }

};

long find_bin_size(double one_std_error)
{
    long K=1;
    while(true)
    {
        double lambda_low=(1.0-one_std_error)*K;
        double P_low=gsl_cdf_poisson_P(K, lambda_low);

        if( P_low>(1.0-0.16) )
        {
            double lambda_high=(1.0+one_std_error)*K;
            double P_high=gsl_cdf_poisson_P(K, lambda_high);

            if( P_high<0.16 ){ return K; }
        }

        K++;
    }
}


int main()
{
	double min_energy=lowest_physical_energy;  //100.0/energy_units_kev;
	double max_energy=100000.0/energy_units_kev; //100000/energy_units_kev;
	int num_energies=5; //200;

    double min_timestep=6e-7;
    double max_timestep=0.01;
    size_t num_timesteps=5;//1000, 20

    double percent_STD_size=0.1;
    //double linearity_error=0.1;

	size_t threads_per_energy=3; //num threads per energy
	size_t num_samples_per_energy_per_thread_per_run=60;
	size_t initial_num_bins_over_three=3;

	gsl::vector energy_vector=logspace(log10(min_energy), log10(max_energy), num_energies);
    gsl::vector timesteps=logspace(log10(min_timestep), log10(max_timestep), num_timesteps);


    long min_samples_of_max_bin=find_bin_size(percent_STD_size);
    one_std_errors.reserve(min_samples_of_max_bin*2);
    for(long i=0; i<min_samples_of_max_bin*2; i++)
    {
        one_std_errors.emplace_back(i);
    }
    print("min size of max bin:", min_samples_of_max_bin);

    //file IO
    arrays_output tables_out;
    auto energies_table=std::make_shared<doubles_output>(energy_vector);
    tables_out.add_array(energies_table);
    auto timesteps_table=std::make_shared<doubles_output>(timesteps);
    tables_out.add_array(timesteps_table);


    //start procesing for each energy
    std::list<energy_level_workspace> samplers;
    for(int energy_i=0; energy_i<energy_vector.size(); energy_i++)
    {
        print("starting energy:", energy_vector[energy_i], "(", energy_i, "/", num_energies, ")");

        energy_level_workspace workspace(timesteps, energy_vector[energy_i], initial_num_bins_over_three);
        workspace.start_thread(num_samples_per_energy_per_thread_per_run, threads_per_energy, min_samples_of_max_bin);
        workspace.output(tables_out);

    }
    //write to file

	binary_output fout("./diffusion_table_MONTECARLO");
	tables_out.write_out( &fout);
}

