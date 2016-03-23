
#ifndef GSL_UTILS
#define GSL_UTILS
#include <cmath>
#include "vector.hpp"
#include "gen_ex.hpp"


namespace gsl_utils
{

    //functions for making new vectors
    gsl::vector linspace(double start, double stop, size_t length)
    {
        gsl::vector out(length);
        double step=(stop-start)/double(length);

        if(length==0)
        {
            return out;
        }

        out[0]=start;
        for(size_t i=1; i<length; i++)
        {
            out[i]=out[i-1]+step;
        }
        return out;
    }

    gsl::vector linspace(double stop, size_t length)
    {
        return linspace(0.0, stop, length);
    }

    gsl::vector logspace(double start_pow, double stop_pow, size_t length,  double base=10.0)
    {
        gsl::vector out(length);
        double step_pow=(stop_pow-start_pow)/double(length);

        if(length==0)
        {
            return out;
        }

        double power=start_pow;
        out[0]=std::pow(base, power);
        for(size_t i=1; i<length; i++)
        {
            power+=step_pow;
            out[i]=std::pow(base, power);
        }
        return out;
    }

    gsl::vector make_vector(size_t length, double value=0.0)
    {
        gsl::vector out(length);
        for(size_t i=1; i<length; i++)
        {
            out[i]=value;
        }
        return out;
    }
    
    gsl::vector cross(gsl::vector A, gsl::vector B)
    {
		if((A.size() != 3) or (B.size() != 3))
		{
			throw gen_exception("input vectors must both have a length of 3");
		}
		
		gsl::vector out(3);
		out[0]=A[1]*B[2] - A[2]*B[1];
		out[1]=A[2]*B[0] - A[0]*B[2];
		out[2]=A[0]*B[1] - A[1]*B[0];
		return out;
	}
}

#endif
