
#ifndef GSL_UTILS_IMPORTANCE_SAMPLING
#define GSL_UTILS_IMPORTANCE_SAMPLING

#include "functor.h"
#include "rng.hpp"

namespace gsl_utils
{

    class rejection_sampling_1D
    {
    private:
        functor* function;
        double maximum;
        gsl::rng rand;

    public:
        rejection_sampling_1D(functor* function_, double maximum_, double lower_range, double upper_range)
        {
            function=function_;
            maximum=maximum_;

            rand=gsl::rng(gsl::rng.mt19937_1999() );
        }

        double sample()
        {

        }
    };

}

#endif
