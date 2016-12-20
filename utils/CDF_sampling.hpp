
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


//one part uses chebyshev fits. Which need to define here, since chebyshev uses this file
namespace cheby_tables
{
    double U3_i[4]={     1.0,   std::cos(PI/3.0),   std::cos(2*PI/3.0), -1.0};

    double F3_ij[4][4]={{0.5,   1.0,               1.0,                0.5},
                        {0.5,  std::cos(PI/3.0),   std::cos(2*PI/3.0), -0.5},
                        {0.5,  std::cos(2*PI/3.0), std::cos(4*PI/3.0), 0.5},
                        {0.5,  -1.0,               1.0,                -0.5} };



    double U4_i[5]={1.0, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75), -1.0};

    double F4_ij[5][5]={{0.5, 1,                 1,                1,                    0.5},
                        {0.5, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75),    -0.5},
                        {0.5, std::cos(PI*0.5),  std::cos(PI),     std::cos(PI*1.5),     0.5},
                        {0.5, std::cos(PI*0.75), std::cos(PI*1.5), std::cos(PI*2.25),    -0.5},
                        {0.5, std::cos(PI),      std::cos(PI*2.0), std::cos(PI*3.0),     0.5} };
};



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

    //CDF_sampler(CDF_sampler& cpy)
    //{
    //    splines=cpy.splines;
    //    aliases=cpy.aliases;
    //    alias_probabilities=cpy.alias_probabilities;
    //}

    //CDF_sampler(CDF_sampler cpy)
    //{
    //    splines=cpy.splines;
    //    aliases=cpy.aliases;
    //    alias_probabilities=cpy.alias_probabilities;
    //}

    CDF_sampler( array_input& in )
    {
        alias_probabilities=in.read_doublesArray();
        aliases=in.read_intsArray();

        splines=std::make_shared< std::vector<polynomial> >();
        splines->reserve(aliases.size());

        for(int i=0; i<aliases.size(); i++)
        {
            splines->emplace_back(in.read_doublesArray());
        }
    }

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

    CDF_sampler(std::shared_ptr<poly_spline> unNormed_CDF_spline)
    {
        ////need to invert the CDF, use chebyshev polys////
        size_t N=unNormed_CDF_spline->splines.size();

        splines=std::make_shared< std::vector<polynomial> >();
        splines->reserve(N);
        gsl::vector spline_weights(N);

        auto& CDF_splines=unNormed_CDF_spline->splines;
        auto& CDF_theta_vals=unNormed_CDF_spline->x_vals;

        for(int spline_i=0; spline_i<N; spline_i++)
        {
            double high_call=CDF_splines[spline_i].call(CDF_theta_vals[spline_i+1]);
            double low_call=CDF_splines[spline_i].call(CDF_theta_vals[spline_i]);

            spline_weights[spline_i]=high_call - low_call;

            double A=(high_call - low_call)*0.5;
            double B=(high_call + low_call)*0.5;

            //sample the inverse function at 5 points
            double Y0=CDF_theta_vals[spline_i+1];
            //print("A");
            double Y1=bracketed_poly_solver(&CDF_splines[spline_i],  cheby_tables::U4_i[1]*A+B,   CDF_theta_vals[spline_i],CDF_theta_vals[spline_i+1],  10000);
            //print(1);
            double Y2=bracketed_poly_solver(&CDF_splines[spline_i],  cheby_tables::U4_i[2]*A+B,   CDF_theta_vals[spline_i],CDF_theta_vals[spline_i+1],  10000);
            //print(2);
            double Y3=bracketed_poly_solver(&CDF_splines[spline_i],  cheby_tables::U4_i[3]*A+B,   CDF_theta_vals[spline_i],CDF_theta_vals[spline_i+1],  10000);
            //print("B");
            double Y4=CDF_theta_vals[spline_i];

            //get chebyshev coefiencents
            double C0=Y0*cheby_tables::F4_ij[0][0] + Y1*cheby_tables::F4_ij[0][1]  + Y2*cheby_tables::F4_ij[0][2]  + Y3*cheby_tables::F4_ij[0][3]  + Y4*cheby_tables::F4_ij[0][4];
            double C1=Y0*cheby_tables::F4_ij[1][0] + Y1*cheby_tables::F4_ij[1][1]  + Y2*cheby_tables::F4_ij[1][2]  + Y3*cheby_tables::F4_ij[1][3]  + Y4*cheby_tables::F4_ij[1][4];
            double C2=Y0*cheby_tables::F4_ij[2][0] + Y1*cheby_tables::F4_ij[2][1]  + Y2*cheby_tables::F4_ij[2][2]  + Y3*cheby_tables::F4_ij[2][3]  + Y4*cheby_tables::F4_ij[2][4];
            double C3=Y0*cheby_tables::F4_ij[3][0] + Y1*cheby_tables::F4_ij[3][1]  + Y2*cheby_tables::F4_ij[3][2]  + Y3*cheby_tables::F4_ij[3][3]  + Y4*cheby_tables::F4_ij[3][4];
            double C4=Y0*cheby_tables::F4_ij[4][0] + Y1*cheby_tables::F4_ij[4][1]  + Y2*cheby_tables::F4_ij[4][2]  + Y3*cheby_tables::F4_ij[4][3]  + Y4*cheby_tables::F4_ij[4][4];

            //re-weight so that x value goes from 0 to 1
            double W0= C0*0.25 - 0.5*C1 + 0.5*C2 - 0.5*C3 + 0.25*C4;
            double W1= C1 - 4*C2 + 9*C3 - 8*C4;
            double W2= 4*C2 - 24*C3 + 40*C4;
            double W3= 16*C3 - 64*C4;
            double W4= 32*C4;

            splines->emplace_back( polynomial({W0, W1, W2, W3, W4}) );
        }

        spline_weights/=spline_weights.sum();//normalize
        set(spline_weights);
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

            //initialize the alias data most of this will change. Initialize so it is correct if the weight is one.
            aliases[spline_i]=spline_i;
            alias_probabilities[spline_i]=1.0;//new_data.size;
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


    void binary_save( arrays_output& out )
    {
        out.add_doubles(alias_probabilities);
        out.add_ints(aliases);

        for(polynomial& poly : *splines )
        {
            out.add_doubles(poly.weights);
        }
    }

};

#endif // CDF_SAMPLING_HPP
