
#include <cmath>
#include <list>

#include "vector.hpp"
#include "vector_long.hpp"

#include "GSL_utils.hpp"
#include "spline.hpp"
#include "integrate.hpp"
#include "spline_2D.hpp"
#include "arrays_IO.hpp"
#include "rand.hpp"

using namespace std;


//this file tests using walker aliasing to sample continuous distribution functions.
//The conclusion is that the algorithm works and is much faster than my standard algorithm (just inverting the CdF and search the splines using a binary search)
//however, the standard algorithm is SO fast that it probably isn't the limiting speed factor in the monte-carlo simulation. I need to sample 10 MILLION points before there is
// a noticeable speed difference. So the speed of using walker aliasing may not be worth the  complication.

class PDF : public functor_1D
{
    const double W=20.0;
public:
    double call(double X)
    {
        return sin(W*X)+1;
    }
};

class alias_data
{
    public:
    size_t index;
    double size;
};

bool alias_increasing_compare(const alias_data& LHS, const alias_data& RHS)
{
    return LHS.size<RHS.size;
}

bool alias_decreasing_compare(const alias_data& LHS, const alias_data& RHS)
{
    return LHS.size>RHS.size;
}

int main()
{
    size_t num_samples=10000000; //have to sample 10 million points from this distribution before there is a noticable speed differance. Walker aliasing is faster

print("setup CDF");
    //// made CDF
    PDF un_normed_pdf;
    cum_adap_simps integrator(&un_normed_pdf, 0, 3.1415926, 1E3);
    gsl::vector x_vals=integrator.points();
    gsl::vector PDF=integrator.values();
    gsl::vector CDF=integrator.cum_quads();
    CDF/=CDF[CDF.size()-1]; //normalize

    //make typical sampler
    poly_spline sampler(CDF, x_vals);

print("setup alias data:", CDF.size());
    //make aliased sampler
    gsl::vector_long aliases(sampler.splines.size());
    gsl::vector alias_probabilities(sampler.splines.size());
    gsl::vector alias_boundAdjust(sampler.splines.size());

    //double medium_height=1.0/sampler.splines.size();
    //print(medium_height);

    list<alias_data> too_low;
    list<alias_data> too_high;

    for(size_t spline_i=0; spline_i<sampler.splines.size(); spline_i++)
    {
        //make alias_data, which is just needed for this algorithm
        alias_data new_data;
        new_data.index=spline_i;
        new_data.size=(sampler.x_vals[spline_i+1] - sampler.x_vals[spline_i])*sampler.splines.size();

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
/*
        double sum=0;
        print("low");
        for(auto LD=low_iter; LD != too_low.end(); ++LD)
        {
            print(" ", LD->index, LD->size);
            sum+=LD->size;
        }

        print("high");
        for(auto LD=high_iter; LD != too_high.end(); ++LD)
        {
            print(" ", LD->index, LD->size);
            sum+=LD->size;
        }
        print(sum);*/


    while( low_iter != too_low.end() and high_iter!=too_high.end())
    {
        //print("high index:", high_iter->index);


        while(true)
        {
            //print("fixed low index:", low_iter->index);
            aliases[low_iter->index]=high_iter->index;
            alias_probabilities[low_iter->index]=low_iter->size;

            alias_boundAdjust[low_iter->index]=sampler.x_vals[high_iter->index] + (high_iter->size-1.0)/double(aliases.size());//

//print();
//print("show:", low_iter->index, high_iter->index);
//print(alias_boundAdjust[low_iter->index]+1.0/aliases.size(), sampler.x_vals[high_iter->index]+high_iter->size/aliases.size(), sampler.x_vals[high_iter->index+1]);
            high_iter->size+=low_iter->size-1.0;
//print(alias_boundAdjust[low_iter->index]+alias_probabilities[low_iter->index]/aliases.size(), sampler.x_vals[high_iter->index]+high_iter->size/aliases.size(), sampler.x_vals[high_iter->index]);
//print();

            ++low_iter;
            if(high_iter->size<0)
            {
                print("ALGORITHM ERROR 2");
            }
            else if( std::abs(1.0-high_iter->size) < 1.0E-10  )
            {
                //just right height!

            //print("fixed high index:", high_iter->index);
                break;
            }
            else if(high_iter->size > 1.0)
            {
                //still too high

            //print("still too high:", high_iter->index);
                continue;
            }
            else
            {
                //now we are too low!!
            //print("now too low!:", high_iter->index, 1.0-high_iter->size);
                too_low.push_back(*high_iter);
                if(low_iter==too_low.end())
                {
                    --low_iter;
                }
                break;
            }
        }
        ++high_iter;
/*
        sum=0;
        print("low");
        for(auto LD=low_iter; LD != too_low.end(); ++LD)
        {
            print(" ", LD->index, LD->size);
            sum+=LD->size;
        }

        print("high");
        for(auto LD=high_iter; LD != too_high.end(); ++LD)
        {
            print(" ", LD->index, LD->size);
            sum+=LD->size;
        }
        print(sum);*/


    }

    if(high_iter != too_high.end() or low_iter!=too_low.end())
    {
        print("ERROR IN ALGORITHM");
    }

print("sample normal method");
    rand_gen rand;
    //now sample the distribution the old way
    gsl::vector old_samples(num_samples);
    for(size_t sample_i=0; sample_i<num_samples; sample_i++)
    {
        double R=rand.uniform();
        old_samples[sample_i]=sampler.call(R);
    }

print("sample new method");
    //new method
    //bool shown=false;
    gsl::vector new_samples(num_samples);
    for(size_t sample_i=0; sample_i<num_samples; sample_i++)
    {
        double R=rand.uniform()*aliases.size();
        int index=int(R);
        double remainder=R-index;

        double sample=-1;
        if(remainder<alias_probabilities[index])
        {
            //sample= sampler.splines[index].y( sampler.x_vals[index] + remainder/double(aliases.size()) );
            double X=sampler.x_vals[index] + remainder*(sampler.x_vals[index+1]-sampler.x_vals[index]); //don't need anything fancy. Just map the remainder to the bound of the spline!!
            sample=sampler.splines[index].y( X );
        }
        else
        {
            int alias_index=aliases[index];
            double X=sampler.x_vals[alias_index] + remainder*(sampler.x_vals[alias_index+1]-sampler.x_vals[alias_index]); //don't need anything fancy. Just map the remainder to the bound of the spline!!
            sample=sampler.splines[alias_index].y( X );
            //sample=sampler.splines[alias_index].y( alias_boundAdjust[index] + remainder/double(aliases.size()) );
/*
            if(sample<0 and not shown)
            {
                shown=true;
                print("TEST:", alias_index);
                print(sampler.x_vals[alias_index], alias_boundAdjust[index] + alias_probabilities[index]/double(aliases.size()));
                print(sampler.x_vals[alias_index+1], alias_boundAdjust[index] + 1.0/double(aliases.size()));
                print(alias_boundAdjust[index] + remainder/double(aliases.size()), alias_probabilities[index]);
            }*/

            //sample=-1;
        }
        new_samples[sample_i]=sample;
    }


print("saving");

    arrays_output tables_out;

    shared_ptr<doubles_output> old_sample_table=make_shared<doubles_output>(old_samples);
    tables_out.add_array(old_sample_table);

    shared_ptr<doubles_output> new_sample_table=make_shared<doubles_output>(new_samples);
    tables_out.add_array(new_sample_table);

	binary_output fout("./walker_test");
	tables_out.write_out( &fout);
}
