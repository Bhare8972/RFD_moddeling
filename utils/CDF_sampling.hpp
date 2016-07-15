
#ifndef CDF_SAMPLING_HPP
#define CDF_SAMPLING_HPP

#include <cmath>
#include <list>

#include "vector.hpp"
#include "vector_long.hpp"

#include "spline.hpp"
#include "GSL_spline.hpp"
#include "gen_ex.hpp"

// class to sample a cumulative distribution function.
// fits the CDF to an akima spline, then finds the correct spline using walker alias sampling
// algorithm is expensive to initialize and takes significant space, but is very fast (constant time) sampling


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
    std::shared_ptr<poly_spline>  spline_sampler;

    gsl::vector_long aliases;
    gsl::vector alias_probabilities;
    gsl::vector alias_boundAdjust;

    CDF_sampler(gsl::vector x_values, gsl::vector CDF_values)
    {
        set(x_values, CDF_values);
    }

    void set(gsl::vector x_values, gsl::vector CDF_values)
    {
        //invert
        gsl::vector sampler_X;
        gsl::vector sampler_Y;
        make_fix_spline(CDF_values, x_values, sampler_X, sampler_Y);
        sampler_X/=sampler_X[sampler_X.size() - 1]; //normalize

        //use akima spline interpolation
        spline_sampler=akima_spline(sampler_X, sampler_Y);

        //setup walker aliasing
        aliases = gsl::vector_long(spline_sampler.splines.size());
        alias_probabilities = gsl::vector(spline_sampler.splines.size());
        alias_boundAdjust = gsl::vector(spline_sampler.splines.size());

        std::list<alias_data> too_low;
        std::list<alias_data> too_high;

        for(size_t spline_i=0; spline_i<spline_sampler.splines.size(); spline_i++)
        {
            //make alias_data, which is just needed for this algorithm
            alias_data new_data;
            new_data.index=spline_i;
            new_data.size=(spline_sampler.x_vals[spline_i+1] - spline_sampler.x_vals[spline_i])*spline_sampler.splines.size();

            //initialize the alias data most of this will change
            aliases[spline_i]=spline_i;
            alias_probabilities[spline_i]=new_data.size;
            alias_boundAdjust[spline_i]=1.0;

            if( std::abs(1.0-new_data.size) < 1.0E-10  )
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

                alias_boundAdjust[low_iter->index]=spline_sampler.x_vals[high_iter->index] + (high_iter->size-1.0)/double(aliases.size());//

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

        if(high_iter != too_high.end() or low_iter!=too_low.end())
        {
            print("WALKER ALIAS ALGORITHM ERROR 1");
        }
    }

    double sample(double uniform_rand)
    {
        uniform_rand*=aliases.size();
        int index=int(uniform_rand);
        double remainder=uniform_rand-index;

        double ret=-1;
        if(remainder<alias_probabilities[index])
        {
            ret= spline_sampler.splines[index].y( spline_sampler.x_vals[index] + remainder/aliases.size() );
        }
        else
        {
            int alias_index=aliases[index];
            ret=spline_sampler.splines[alias_index].y( alias_boundAdjust[index] + remainder/aliases.size() );
        }
        return ret;
    }

};

#endif // CDF_SAMPLING_HPP
