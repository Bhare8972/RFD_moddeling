
#ifndef GSL_UTILS
#define GSL_UTILS
#include <cmath>
#include "vector.hpp"


namespace gsl_utils
{
    class gsl_util_exception: public exception
    {
    private:
        int error;
    public:

      virtual const char* what() const throw()
      {
        return "GSL error in gsl utility";
      }

    };

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
}

#endif
