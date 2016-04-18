
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
#endif
