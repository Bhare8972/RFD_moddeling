

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
//#include "histogram.hpp"

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "spline.hpp"
#include "gen_ex.hpp"

#include "../physics/shielded_coulomb_diffusion.hpp"

using namespace std;

class integrator
{
    public:

    double theta;
    double theta_prime;
    shared_ptr<poly_spline> dp_dOmega;
    diff_cross_section* cross_section;

    int f_calls;

    method_functor_1D<integrator> phi_integrand_functor;
    gsl_function phi_integrand_GSLfunctor;
    gsl_integration_workspace* phi_workspace;


    method_functor_1D<integrator> theta_integrand_functor;
    gsl_function theta_integrand_GSLfunctor;
    gsl_integration_workspace* theta_workspace;

    integrator(diff_cross_section* cross_section_) : phi_integrand_functor( this, &integrator::integrand_of_phi), theta_integrand_functor( this, &integrator::integrand_of_theta)
    {
        cross_section=cross_section_;

        phi_integrand_GSLfunctor=phi_integrand_functor.get_gsl_func();
        phi_workspace=gsl_integration_workspace_alloc (1000);

        theta_integrand_GSLfunctor=theta_integrand_functor.get_gsl_func();
        theta_workspace=gsl_integration_workspace_alloc (1000);
    }

    ~integrator()
    {
        gsl_integration_workspace_free(phi_workspace);
        gsl_integration_workspace_free(theta_workspace);
    }

    void set_dp_dOmega(gsl::vector theta_space, gsl::vector values)
    {
        dp_dOmega=make_shared<poly_spline>(theta_space, values);
    }

    void set_dp_dOmega(shared_ptr<poly_spline> dp_dOmega_)
    {
        dp_dOmega=dp_dOmega_;
    }

    inline double angular_distance(double theta_one, double theta_two, double delta_phi)
    {
    //haversine forumla
        double A=sin((theta_one-theta_two)*0.5);
        A*=A;
        double B=sin(delta_phi*0.5);
        B*=B;
        B*=sin(theta_one)*sin(theta_two); //sin instead of cos since thetas are from Z axis
        return 2*asin(min(1.0,sqrt(A+B)));

    //law of cosines
    //ill-conditioned for small theta (supposibly not a problem for doubles), which may be worth the speed-up?
        //return acos( min(1.0, cos(theta_one)*cos(theta_two) + sin(theta_one)*sin(theta_two)*cos(delta_phi/2.0) ) );
    }

    double integrand_of_phi(double delta_phi)
    // the integrand
    {
        //f_calls++;
        return cross_section->call( angular_distance(theta_prime, theta, delta_phi) );
    }

    double integrand_of_theta(double theta_prime_)
    {
        theta_prime=theta_prime_;

        cum_adap_simps integrator(&phi_integrand_functor, 0, 3.1415926, 1E9, 2);

        return integrator.quad()*2*dp_dOmega->call(theta_prime)*sin(theta_prime);

/*
        theta_prime=theta_prime_;

        double answer;
        double error;
        int result= gsl_integration_qag (&phi_integrand_GSLfunctor, 0, 3.1415926, 0.01, 0.01, 1000, 6, phi_workspace, &answer, &error); //function is semetric in pi

        if(result==GSL_EMAXITER)
        {
            print("maximum itterations exceeded");
        }
        else if(result==GSL_EROUND)
        {
            print("cannot reach tollerance due to round off");
        }
        else if(result==GSL_ESING)
        {
            print("bad integrand behavior");
        }
        else if(result==GSL_EDIVERGE)
        {
            print("integral is too divergent");
        }

        //answer*=dp_dOmega->call(theta_prime)*sin(theta_prime);
        //double B=dp_dOmega->call(theta_prime)*sin(theta_prime);
        //if(answer*B<0)
        //{
        //    print("arg:", theta*180/3.1415926, theta_prime*180/3.1415926);
         //   print("  ", answer, dp_dOmega->call(theta_prime), sin(theta_prime));
       // }
        return answer*dp_dOmega->call(theta_prime)*sin(theta_prime)*2;*/
    }

    double step(double theta_)
    {/*
        //f_calls=0;
        theta=theta_;

        cum_adap_simps integrator(&theta_integrand_functor, 0, 3.1415926, 1E9, 2);

        //print(f_calls);

        return integrator.quad();*/

        //f_calls=0;
        theta=theta_;

        double answer;
        double error;
        int result= gsl_integration_qag (&theta_integrand_GSLfunctor, 0, 3.1415926, 0.1, 0.1, 1000, 1, theta_workspace, &answer, &error);

        if(result==GSL_EMAXITER)
        {
            print("maximum itterations exceeded");
        }
        else if(result==GSL_EROUND)
        {
            print("cannot reach tollerance due to round off");
        }
        else if(result==GSL_ESING)
        {
            print("bad integrand behavior");
        }
        else if(result==GSL_EDIVERGE)
        {
            print("integral is too divergent");
        }

        //print(f_calls);

        return answer;
    }
};


class output_control : public functor_1D
{
    public:

    shared_ptr<poly_spline> current_interpolant;


    shared_ptr<poly_spline> new_step;
    double new_step_weight;

    output_control()
    {
        gsl::vector points({0, 3.1415926});
        gsl::vector values({0, 0});
        current_interpolant=make_shared<poly_spline>(points, values);
    }

    double call(double X)
    {
        return current_interpolant->call(X) + new_step_weight*new_step->call(X);
    }

    void add_step(shared_ptr<poly_spline> new_step_, double new_step_weight_)
    {
        new_step=new_step_;
        new_step_weight=new_step_weight_;

        current_interpolant= adaptive_sample_retSpline(this, 0.1, 0, 3.1415926);
    }

    double integrand(double X)
    {
        return current_interpolant->call(X)*std::sin(X);
    }
};


int main()
{
    double energy=10000/energy_units_kev;
    double timestep=0.00001;

    double probability_accuracy=0.001; //when to truncate the series
    double sampling_precision=0.1; //required precision in sampling the functions

    ////// initilize //////
    //cross section
    diff_cross_section cross_section(energy);
    double interactions_per_tau=cross_section.num_interactions_per_tau;
    print(interactions_per_tau*timestep, "interactions per timestep");
    method_functor_1D<diff_cross_section> cross_section_functor(&cross_section, &diff_cross_section::dp_dOmega);

    //integrator
    integrator stepper(&cross_section);
    method_functor_1D<integrator> stepper_functor(&stepper, &integrator::step);

    //setup output
    output_control output;


    //////do first step //////
    auto current_dp_dOmega=adaptive_sample_retSpline(&cross_section_functor, 0.001, 0, 3.1415926 );
    current_dp_dOmega->multiply(1.0/current_dp_dOmega->integrate(3.1415926));//we need to normalize it
    stepper.set_dp_dOmega(current_dp_dOmega);

    output.add_step(current_dp_dOmega, gsl_ran_poisson_pdf(1, cross_section.num_interactions_per_tau*timestep)  );

    ///////do next steps//////
    int current_N=2;
    bool reached_peak=false;
    double max_p=0;
    while(true)
    {
        if(current_N==5) break;
        print("doing:", current_N);

        double current_p=gsl_ran_poisson_pdf(current_N, cross_section.num_interactions_per_tau*timestep);
        //output.mult_add(current_dp_dOmega, current_p);

        if(reached_peak and current_p*probability_accuracy<max_p) break;
        if(current_p<max_p) reached_peak=true;
        else if(not reached_peak) max_p=current_p;

        //do the step
        current_dp_dOmega=adaptive_sample_retSpline(&stepper_functor, 0.1, 0, 3.1415926 );

        print("integration complete");
        current_dp_dOmega->multiply(1.0/current_dp_dOmega->integrate(3.1415926));//we need to normalize it
        stepper.set_dp_dOmega(current_dp_dOmega);
        output.add_step(current_dp_dOmega, gsl_ran_poisson_pdf(1, cross_section.num_interactions_per_tau*timestep)  );

        current_N++;
    }


    ////// finally, do inverse transform sampling ///////
    method_functor_1D<output_control> output_integral_functor(&output, &output_control::integrand);
    //cum_adap_simps output_integrator(&output_integral_functor, 0, 3.1415926, 1e4);

    //gsl::vector points=output_integrator.points();
    //gsl::vector values=output_integrator.cum_quads();

    gsl::vector points;
    gsl::vector values=adaptive_sample(&output_integral_functor, 0.01, 0, 3.1415926, points );

    //write to file
    arrays_output tables_out;
    shared_ptr<doubles_output> theta_space_table=make_shared<doubles_output>(points);
    tables_out.add_array(theta_space_table);

    shared_ptr<doubles_output> output_table=make_shared<doubles_output>(values);
    tables_out.add_array(output_table);

	binary_output fout("./out");
	tables_out.write_out( &fout);
}

/*
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

    hist_tool(double start, double stop, size_t num_bins)
    {
        auto bin_ranges=linspace(start, stop, num_bins+1);
        for(size_t bin_i=0; bin_i<num_bins; bin_i++)
        {
            hist_bins.emplace_back(bin_ranges[bin_i], bin_ranges[bin_i+1]);
        }
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
        for(bin &B : hist_bins)
        {
            if(B.increment(value)) break;
        }
    }

    void split(iterator IT)
    {
        iterator IN=IT;
        hist_bins.emplace(++IN, *IT, 0);
    }

    gsl::vector get_bin_ranges()
    {
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



I AM HERE


class workspace
{
public:
	diff_cross_section cross_section;
	double energy_;

	shared_ptr<poly_quad_spline> spline_sampler; //why do we need a mutex for this?
    mutex sampler_mutex;
	double num_interactions;


	list<thread> threads;

	gsl_rng* rand;
    mutex rand_mutex;

	hist_tool distribution;
    mutex dist_mutex;


	workspace(double timestep, double energy, size_t num_bins_, bool rnd_seed=false) : cross_section(timestep), distribution(0, 3.1415926, num_bins_)
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

		set_energy(energy);
		//distribution=gsl::histogram(num_bins, 0, 3.1415926);
	}

    ~workspace()
	{
		gsl_rng_free(rand);
	}

    double sample_uniform()
	//gives a uniform random sample between 0 and 1
	{
        lock_guard<mutex> lock(rand_mutex);
		return gsl_rng_uniform(rand);
	}

    unsigned int sample_num_interactions()
	//samples poisson distribtion for number of diffusion interactions
	{
        lock_guard<mutex> lock(rand_mutex);
        return gsl_ran_poisson (rand, num_interactions);
	}

	void set_energy(double energy)
	{
        energy_=energy;
		cross_section.set_energy(energy);
		cum_adap_simps integrator(&cross_section, 0, 3.1415926, 1E4);
		gsl::vector points=integrator.points();
		gsl::vector cum_quads=integrator.cum_quads();
		num_interactions=cum_quads[cum_quads.size()-1]*2*3.1415926;
		cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

		gsl::vector quad_X;
		gsl::vector quad_Y;
        make_fix_spline(cum_quads, points, quad_X, quad_Y);// hope this function will fix the function if the points are singular!
		spline_sampler=make_shared<poly_quad_spline>(quad_X, quad_Y);
		spline_sampler->set_lower_fill(quad_X[0]);
		spline_sampler->set_upper_fill(quad_X[quad_X.size()-1]);


        //shared_ptr<doubles_output> cum_quad_table=make_shared<doubles_output>(cum_quads);
        //shared_ptr<doubles_output> point_table=make_shared<doubles_output>(points);
        //shared_ptr<doubles_output> value_table=make_shared<doubles_output>(integrator.values());
        //shared_ptr<doubles_output> quad_X_table=make_shared<doubles_output>(quad_X);
        //shared_ptr<doubles_output> quad_Y_table=make_shared<doubles_output>(quad_Y);

        //arrays_output array_out;
        //array_out.add_array(cum_quad_table);
        //array_out.add_array(point_table);
        //array_out.add_array(value_table);
        //array_out.add_array(quad_X_table);
        //array_out.add_array(quad_Y_table);

        //binary_output fout("./quadrature_tst");
        //array_out.write_out( &fout);
    }

	double sample_timestep()
	{
	    //get our random samples
        unsigned int num_samples=sample_num_interactions();

        gsl::vector T({0,0,1});

        for(size_t i=0; i<num_samples; i++)
        {
            double inclination_scattering;
            double azimuth_scattering;
            {
                lock_guard<mutex> lock(sampler_mutex);
                inclination_scattering=spline_sampler->call( sample_uniform() ); //transform the sample
                azimuth_scattering=sample_uniform()*2*3.1415926;
            }

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
        }

        return acos(T[2]);
	}

	void multi_samples(size_t N)
	{
	    for(size_t i=0; i<N; i++)
	    {
            double sample=sample_timestep();

            {
                lock_guard<mutex> lock(dist_mutex);
                distribution.increment(sample);
                //print("A:", i+lowest_index, final_distribution.size());
                //final_distribution[i+lowest_index]=sample;
                //print("B");
            }
	    }
	}

	void start_thread(size_t samples_perThread_perRun, size_t N_threads, double percent_error)
	{
        if(threads.size() !=0 ) throw gen_exception("must join before starting more threads");

        //sample for the required number of times
        for(size_t i=0; i<N_threads; i++)
        {
            threads.push_back( thread(&workspace::multi_samples, this, samples_perThread_perRun) );
            //multi_samples(samples_per_thread, i*samples_per_thread);
        }

        for(auto& T : threads)
        {
            T.join();
        }
        threads.clear();


        bool keep_running=true;
        size_t n_runs=1;
        while(keep_running)
        {
            gsl::vector old_distribution=distribution.get_bin_values();

            //sample for the required number of times
            for(size_t i=0; i<N_threads; i++)
            {
                threads.push_back( thread(&workspace::multi_samples, this, samples_perThread_perRun) );
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

            gsl::vector new_distribution=distribution.get_bin_values();
            for(size_t i=0; i<new_distribution.size(); i++)
            {
                double N_pnts=new_distribution[i];
                if(N_pnts==0)
                {
                    continue;
                }

                double DH=N_pnts-old_distribution[i];
                double next_error_factor=DH/N_pnts;

                if(next_error_factor>error_factor)
                {
                    error_factor=next_error_factor;
                }
            }

            print(energy_, ":", (n_runs+1)*samples_perThread_perRun*N_threads, "  error:", error_factor, "desired error:", percent_error);

            if(error_factor<percent_error)
            {
                keep_running=false;
            }

            for(auto iter=distribution.begin(); (++iter)!=distribution.end(); ++iter)
            {
                double next_H=iter->num_counts;
                --iter;
                double H=iter->num_counts;

                if(iter->splittable and abs(next_H-H)>3*(sqrt(next_H)+sqrt(H)))
                {
                    keep_running=true;
                    print("SPLIT!:", iter->lower_range, iter->upper_range);
                    distribution.split(iter);
                    //auto ranges=distribution.get_bin_ranges();
                    //for(auto v : ranges)
                    //{
                        //cout<<" "<<v;
                    //}
                    //cout<<endl;
                    break; //WHY IS THIS HERE?
                }
            }

            n_runs++;
        }

	}
//
//	void join_thread()
//    {
//        for(auto& T : threads)
//        {
//            T.join();
//        }
//        threads.clear();
//    }

};

void do_timestep(double time_step)
{
	//double time_step=0.0001;
	double min_energy=0.05; //keV
	double max_energy=100000; //kev
	int num_energies=100;
	size_t threads_per_energy=16; //num threads per energy
	size_t num_samples_per_energy_per_thread_per_run=100;
	size_t num_bins=20; //controlls precision of final distribution
	double error_percent=0.15; //controlls error of bins in y-direction
	bool rnd_seed=false;
	gsl::vector energy_vector=logspace(log10(min_energy), log10(max_energy), num_energies);




    //start procesing for each energy
    std::list<workspace> samplers;
    for(double energy : energy_vector)
    {
        samplers.emplace_back(time_step, energy, num_bins, rnd_seed);
        samplers.back().start_thread(num_samples_per_energy_per_thread_per_run, threads_per_energy, error_percent);
        //samplers.back().join_thread();
    }

    //join threads
//    for(workspace& W : samplers)
//    {
//        W.join_thread();
//    }
    print("writing to file");

    //write to file
    arrays_output tables_out;
    shared_ptr<doubles_output> energies_table=make_shared<doubles_output>(energy_vector);
    tables_out.add_array(energies_table);

    for(workspace& W : samplers)
    {
        shared_ptr<arrays_output> distribution_table=make_shared<arrays_output>();

        shared_ptr<doubles_output> distribution_ranges_table=make_shared<doubles_output>(W.distribution.get_bin_ranges());
        shared_ptr<doubles_output> distribution_values_table=make_shared<doubles_output>(W.distribution.get_bin_values());

        distribution_table->add_array(distribution_ranges_table);
        distribution_table->add_array(distribution_values_table);

        tables_out.add_array(distribution_table);
    }

    stringstream fname;
    fname<<"../tables/diffusion/"<<time_step;
	binary_output fout(fname.str());
	tables_out.write_out( &fout);

}

int main()
{
    do_timestep(0.00005);
    do_timestep(0.0001);
    do_timestep(0.0005);
    do_timestep(0.001);
    do_timestep(0.005);
    do_timestep(0.01);
    do_timestep(0.05);
    do_timestep(0.1);
    do_timestep(0.5);
}*/
