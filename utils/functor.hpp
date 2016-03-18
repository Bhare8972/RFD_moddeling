

#ifndef GSL_UTILS_FUNCTOR
#define GSL_UTILS_FUNCTOR

#include<gsl/gsl_roots.h>

///generic class to be able to pass functions
namespace gsl_utils
{
    class functional;
    double call_functional(double X, void* info)
    {
        functional* F=(functional&)F;
        return F->call(X);
    }

    class functional
    {
    public:
        virtual double call(double X)=0;

        gsl_function get_gsl_func()
        {
            gsl_function ret;
            ret.function=&call_functional;
            ret.params=(void*)this;
        }
    };
}

#endif
