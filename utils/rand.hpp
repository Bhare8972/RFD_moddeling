
#ifndef RAND_HPP
#define RAND_HPP

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <ctime>
#include <mutex>


//two usefull classes for random numbers

class rand_gen
//general random number generator
{
public:
    gsl_rng* rand_;

    rand_gen(bool do_seed=true)
    {
        rand_=gsl_rng_alloc(gsl_rng_mt19937);
        if(do_seed)
		{
		    unsigned int seed_val( time(NULL) );
		    gsl_rng_set(rand_, seed_val);
		}
		else
        {
            gsl_rng_set(rand_, 0);
        }
    }

    rand_gen(double seed_val)
    {
        rand_=gsl_rng_alloc(gsl_rng_mt19937);
        gsl_rng_set(rand_, seed_val);
    }

    ~rand_gen()
	{
		gsl_rng_free(rand_);
	}

    double uniform()
    {
        return gsl_rng_uniform(rand_);
    }

    double uniform(double a, double b)
    {
        return a + (b-a)*gsl_rng_uniform(rand_);
    }

    double poisson(double mu)
    {
        return gsl_ran_poisson(rand_, mu);
    }

    double exponential(double mu)
    {
        return gsl_ran_exponential (rand_, mu);
    }
};

class rand_threadsafe
//slightly heavier than just the ran class
//use if need to be threadsafe
{
public:
    gsl_rng* rand;
    std::mutex rand_mutex;

    rand_threadsafe(bool do_seed=true)
    {
        rand=gsl_rng_alloc(gsl_rng_mt19937);
        if(do_seed)
		{
		    unsigned int seed_val( time(NULL) );
		    gsl_rng_set(rand, seed_val);
		}
		else
        {
            gsl_rng_set(rand, 0);
        }
    }

    rand_threadsafe(double seed_val)
    {
        rand=gsl_rng_alloc(gsl_rng_mt19937);
        gsl_rng_set(rand, seed_val);
    }

    ~rand_threadsafe()
	{
		gsl_rng_free(rand);
	}

    double uniform()
    {
        std::lock_guard<std::mutex> lock(rand_mutex);
        return gsl_rng_uniform(rand);
    }

    double uniform(double a, double b)
    {
        std::lock_guard<std::mutex> lock(rand_mutex);
        return a + (b-a)*gsl_rng_uniform(rand);
    }

    double poisson(double mu)
    {
        std::lock_guard<std::mutex> lock(rand_mutex);
        return gsl_ran_poisson(rand, mu);
    }

    double exponential(double mu)
    {
        std::lock_guard<std::mutex> lock(rand_mutex);
        return gsl_ran_exponential (rand, mu);
    }
};


#endif

