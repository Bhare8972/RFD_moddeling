
#ifndef CDF_SAMPLING_HPP
#define CDF_SAMPLING_HPP

#include <cmath>
#include <list>

#include <gsl/gsl_sf_gamma.h>

#include "vector.hpp"
#include "vector_long.hpp"

#include "spline.hpp"
#include "GSL_spline.hpp"
#include "gen_ex.hpp"

// class to sample a cumulative distribution function.
// fits the CDF to an akima spline, then finds the correct spline using walker alias sampling
// algorithm is expensive to initialize and takes significant space, but is very fast (constant time) sampling

//TODO:
  //add saving and recalling from text
  //consider removing dependance on spline_sampler


class CDF_sampler
{
    class alias_data //needed for initializing the sampler
    {
        public:
        size_t index;
        double size;
    };

    static bool alias_increasing_compare(const alias_data& LHS, const alias_data& RHS)
    {
        return LHS.size<RHS.size;
    }

    static bool alias_decreasing_compare(const alias_data& LHS, const alias_data& RHS)
    {
        return LHS.size>RHS.size;
    }

public:
    std::shared_ptr< std::vector<polynomial> >  splines; //set of polynomials that each have x-ranges from 0 to 1, that return the sampled value

    gsl::vector_long aliases;
    gsl::vector alias_probabilities;
    //gsl::vector alias_boundAdjust;

    CDF_sampler(){}

    CDF_sampler(gsl::vector x_values, gsl::vector CDF_values)
    {
        //invert
        gsl::vector sampler_X;
        gsl::vector sampler_Y;
        make_fix_spline(CDF_values, x_values, sampler_X, sampler_Y);
        sampler_X/=sampler_X[sampler_X.size() - 1]; //normalize

        //use spline interpolation
        auto spline_sampler=natural_cubic_spline(sampler_X, sampler_Y);

        //get splines and weights
        splines=std::make_shared< std::vector<polynomial> >( spline_sampler->splines );

        gsl::vector weights( splines->size() );
        for(int i=0; i<splines->size(); i++)
        {
            weights[i]=spline_sampler->x_vals[i+1] - spline_sampler->x_vals[i];
        }

        //fix all splines to have ranges between 0 and 1
        for(int i=0; i<splines->size(); i++)
        {
            double Xmin=spline_sampler->x_vals[i];
            double Xrange=spline_sampler->x_vals[i+1] - spline_sampler->x_vals[i];

            polynomial& poly= (*splines)[i];

            double Xrange_factor=1;
            for(int factor_j=0; factor_j<poly.weights.size(); factor_j++)
            {
                double new_factor=0;
                double Xmin_factor=1;
                for(int iter_i=factor_j; iter_i<poly.weights.size(); iter_i++)
                {
                    new_factor+=poly.weights[iter_i]*Xmin_factor*Xrange_factor*gsl_sf_choose(iter_i, factor_j);

                    Xmin_factor*=Xmin;
                }
                Xrange_factor*=Xrange;

                poly.weights[factor_j]=new_factor;
            }
        }
        weights/=weights.sum();

        //perform the walker algorithm
        set(weights);
    }

    void set(gsl::vector weights)
    //sets up the sampler for walker aliasing. Assumes that splines are set
    {
        //setup walker aliasing
        aliases = gsl::vector_long(splines->size());
        alias_probabilities = gsl::vector(splines->size());
        //alias_boundAdjust = gsl::vector(spline_sampler.splines.size());

        std::list<alias_data> too_low;
        std::list<alias_data> too_high;

        for(size_t spline_i=0; spline_i<splines->size(); spline_i++)
        {
            //make alias_data, which is just needed for this algorithm
            alias_data new_data;
            new_data.index=spline_i;
            new_data.size=weights[spline_i]*splines->size();

            //initialize the alias data most of this will change
            aliases[spline_i]=spline_i;
            alias_probabilities[spline_i]=new_data.size;
            //alias_boundAdjust[spline_i]=1.0;

            if( std::abs(1.0-new_data.size) < 1.0E-10 )
            {
                //then new_data.size is approxamently 1.0
                continue;
            }
            else if(new_data.size < 1.0)
            {
                too_low.push_back(new_data);
            }
            else
            {
                too_high.push_back(new_data);
            }
        }

        //print('a', alias_probabilities.sum(), );

        //sort the new lists
        too_low.sort(alias_increasing_compare);
        too_high.sort(alias_decreasing_compare);

        //generate alias data!
        auto low_iter=too_low.begin();
        auto high_iter=too_high.begin();


        while( low_iter != too_low.end() and high_iter!=too_high.end())
        {
            while(true)
            {
                aliases[low_iter->index]=high_iter->index;
                alias_probabilities[low_iter->index]=low_iter->size;

                //alias_boundAdjust[low_iter->index]=spline_sampler.x_vals[high_iter->index] + (high_iter->size-1.0)/double(aliases.size());//???? how to handel?

                high_iter->size+=low_iter->size-1.0;

                ++low_iter;
                if(high_iter->size<0)
                {
                    print("WALKER ALIAS ALGORITHM ERROR 2");
                }
                else if( std::abs(1.0-high_iter->size) < 1.0E-10  )
                {
                    //just right height!
                    break;
                }
                else if(high_iter->size > 1.0)
                {
                    //still too high
                    continue;
                }
                else
                {
                    //now we are too low!!
                    too_low.push_back(*high_iter);
                    if(low_iter==too_low.end())
                    {
                        --low_iter;
                    }
                    break;
                }
            }
            ++high_iter;
        }

        if(high_iter != too_high.end() )
        {
            print("WALKER ALIAS ALGORITHM ERROR 1A");
        }
        if( low_iter!=too_low.end())
        {
            print("WALKER ALIAS ALGORITHM ERROR 1B");
        }
    }

    double sample(double uniform_rand)
    {
        uniform_rand*=aliases.size();
        int index=int(uniform_rand);
        double remainder=uniform_rand-index;

        //double ret;
        if(remainder<alias_probabilities[index])
        {
            remainder/=alias_probabilities[index];

            //ret= spline_sampler.splines[index].y( spline_sampler.x_vals[index] + remainder/aliases.size() );
            //ret= spline_sampler->splines[index].y( spline_sampler->x_vals[index] + remainder*(spline_sampler->x_vals[index+1]-spline_sampler->x_vals[index]) );
        }
        else
        {
            remainder=(remainder-alias_probabilities[index])/(1-alias_probabilities[index]);
            index=aliases[index];

            //ret=spline_sampler.splines[alias_index].y( alias_boundAdjust[index] + remainder/aliases.size() );
            //ret=spline_sampler->splines[alias_index].y( spline_sampler->x_vals[alias_index] + remainder*(spline_sampler->x_vals[alias_index+1]-spline_sampler->x_vals[alias_index]) );
        }
        //return ret;

        return (*splines)[index].call(remainder);
    }

};

#endif // CDF_SAMPLING_HPP
