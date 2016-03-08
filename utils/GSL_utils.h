
#ifndef GSL_UTILS
#define GSL_UTILS

#include "vector.hpp"

namespace gsl_utils
{
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
}

#endif
