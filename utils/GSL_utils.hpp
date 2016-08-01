
#ifndef GSL_UTILS
#define GSL_UTILS
#include <cmath>
#include <list>
#include <mutex>
#include "vector.hpp"
#include "vector_float.hpp"
#include "gen_ex.hpp"
#include "iostream"

//first, a python-like print function
//void print()
//{
//	std::cout<<std::endl;
//}
//
//template< typename T, typename...Ts>
//void print(T msg_head, Ts... msg)
//{
//	std::cout<<msg_head<<' ';
//	print(msg...);
//}


//this is a function-like macro for initializing gsl vectors
//it expects that OUT and IN are objects that support random indexing, have a size() method, and are allocated to the same size
//VAR is just a variable name, EXP is an expression. This produces a loop that calls EXP for each value in IN (taking the variable name VAR)
//and places the output in OUT
#define LOOP(OUT, VAR, IN, EXP )  \
for(size_t i=0; i<IN.size(); i++) \
{ double VAR=IN[i]; OUT[i]=(EXP);}

class print_class
// to allow for python-style printing in a thread-safe maner
{
private:
    std::mutex cout_mutex;

public:
    void operator()()
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout<<std::endl;
    }

    template< typename T, typename...Ts>
    void operator()(T msg_head, Ts... msg)
    {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout<<msg_head<<' ';
        }
        (*this)(msg...);
    }
}print; //make available to call as 'print'


//functions for making new vectors
gsl::vector linspace(double start, double stop, size_t length)
{
	gsl::vector out(length);

    if(length==0 or length==1)
	{
		return out;
	}

	double step=(stop-start)/double(length-1);

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
	double step_pow=(stop_pow-start_pow)/double(length-1);

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

gsl::vector make_vector(const std::list<double> &data)
{
	gsl::vector out(data.size());
	size_t i=0;
	for(double value : data)
	{
		out[i]=value;
		i++;
	}
	return out;
}

gsl::vector make_vector(const std::vector<double> &data)
{
	gsl::vector out(data.size());
	size_t i=0;
	for(double value : data)
	{
		out[i]=value;
		i++;
	}
	return out;
}

gsl::vector cross(gsl::vector& A, gsl::vector& B)
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

size_t search_sorted_d(gsl::vector A, double v)
{
	if(v<A[0] or v>=A[A.size()-1]) throw gen_exception("value out of range");
	size_t lower=0;
	size_t upper=A.size()-1;
	while(true)
	{
		//check if found solution
		if( (upper-1)==lower ) return lower;
		//guess new solution
		size_t guess=size_t( (upper-lower)/2 )+lower;
		if(A[guess]>v) upper=guess;
		else if(A[guess+1]<=v) lower=guess+1;
		else return guess;
	}
}

size_t search_sorted_f(gsl::vector_float A, float v)
{
	if(v<A[0] or v>=A[A.size()-1]) throw gen_exception("value out of range");
	size_t lower=0;
	size_t upper=A.size()-1;
	while(true)
	{
		//check if found solution
		if( (upper-1)==lower )
		{
			return lower;
		}
		//guess new solution
		size_t guess=size_t( (upper-lower)/2 ) + lower;
		if(A[guess]>v) upper=guess;
		else if(A[guess+1]<=v) lower=guess+1;
		else return guess;
	}
}

size_t search_sorted_exponential(gsl::vector A, double v)
{
	if(v<A[0] or v>=A[A.size()-1]) throw gen_exception("value out of range");
	size_t lower=0;
	size_t upper=A.size()-1;
	while(true)
	{
		//check if found solution
		if( (upper-1)==lower ) return lower;
		//guess new solution
		size_t guess=size_t( std::log(v/A[lower])*(upper-lower)/std::log(A[upper]/A[lower]) ) + lower;
		if(guess==lower){guess++;}
		else if(guess==upper){guess--;}

		if(A[guess]>v) upper=guess;
		else if(A[guess+1]<=v) lower=guess+1;
		else return guess;
	}
}

size_t search_sorted_linear(gsl::vector A, double v)
{
	if(v<A[0] or v>=A[A.size()-1]) throw gen_exception("value out of range");
	size_t lower=0;
	size_t upper=A.size()-1;
	while(true)
	{
		//check if found solution
		if( (upper-1)==lower ) return lower;
		//guess new solution
		size_t guess=size_t( (v-A[lower])*(upper-lower)/(A[upper]-A[lower]) ) + lower;
		if(guess==lower){guess++;}
		else if(guess==upper){guess--;}

		if(A[guess]>v) upper=guess;
		else if(A[guess+1]<=v) lower=guess+1;
		else return guess;
	}
}

gsl::vector cumsum(gsl::vector IN, bool extra_zero_bin=false)
//cumulative sum of input vector
{
    gsl::vector out(IN.size()+extra_zero_bin);
    double value=0;
    out[0]=0;
    for(int i=extra_zero_bin; i<out.size(); i++)
    {
        value+=IN[i-extra_zero_bin];
        out[i]=value;
    }
    return out;
}

inline double linear_interpolate(gsl::vector X, gsl::vector Y, double X_sample)
//given a number of X and Y samples from a function, return the y-value corresponding to the X_sample using linear interpolation
{
    size_t index=search_sorted_d(X, X_sample);
    double factor=(X_sample-X[index])/(X[index+1]-X[index]);
    double R=Y[index];
    return R + (Y[index+1]-R)*factor;
}


inline double linear_interpolate(double X0, double Y0, double X1, double Y1, double X_sample)
//given a number of X and Y samples from a function, return the y-value corresponding to the X_sample using linear interpolation
{
    return Y0 + (Y1-Y0)*(X_sample-X0)/(X1-X0);
}
#endif
