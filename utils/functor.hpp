
#ifndef FUNCTOR_HPP
#define FUNCTOR_HPP

#include <gsl/gsl_integration.h>

#include "vector.hpp"

class functor_1D
{
    static double GSL_func_resolve(double x, void * param)
    {
        return static_cast<functor_1D*>(param)->call(x);
    }

	public:

	virtual double call(double)=0;

	gsl::vector call(const gsl::vector& data)
	{
		gsl::vector out(data.size());
		for(size_t i=0; i<data.size(); i++)
		{
			out[i]=call(data[i]);
		}
		return out;
	}

    gsl::vector callv(const gsl::vector& data)
	{
		gsl::vector out(data.size());
		for(size_t i=0; i<data.size(); i++)
		{
			out[i]=call(data[i]);
		}
		return out;
	}

    gsl_function get_gsl_func()
    {
        gsl_function ret;
        ret.function=&GSL_func_resolve;
        ret.params=this;
        return ret;
    }
};

template<typename CLASS_T>
class method_functor_1D : public functor_1D
{
    public:
    CLASS_T* object;
    double (CLASS_T::*member)(double);

    method_functor_1D(CLASS_T* object_, double (CLASS_T::*member_)(double) )
    {
        object=object_;
        member=member_;
    }

    double call(double X)
    {
        double R=(object->*member)(X);
        return R;
    }
};


#endif

