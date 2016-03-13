#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>

#include "../utils/GSL_utils.h"
#include "vector.hpp"

using namespace std;
using namespace gsl_utils;
using namespace gsl;

double inv_sqrt(double X)
{
    return 1.0/sqrt(X);
}

float FS_inv_sqrt(float X)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = X * 0.5F;
	y  = X;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 0x5f3759df -float 0x5fe6eb50c7b537a9-double
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

int main()
{
    unsigned int length=1000000;

    vector X=logspace(-10,10,length);

    vector normal=vector(size_t(length));
    vector fast=vector(size_t(length));


    int start=clock();
    for(size_t i=0; i<length; i++)
    {
        normal[i]=inv_sqrt(X[i]);
    }
    int stop=clock();
    cout<<"normal time ticks: "<<(stop-start)<<endl;

    int start_f=clock();
    for(size_t i=0; i<length; i++)
    {
        fast[i]=FS_inv_sqrt(X[i]);
    }
    int stop_f=clock();
    cout<<"fast time ticks: "<<(stop_f-start_f)<<endl;


    ofstream isq_tst("isq_tst_out.txt");
    isq_tst<<"value normal_value differance_to_fast_value"<<endl;
    for(size_t i=0; i<length; i++)
    {
        isq_tst<<X[i]<<" "<<normal[i]<<" "<<(normal[i]-fast[i])<<endl;
    }
}
/*

CONCLUSION:

These tests show that, even when fully optimized, the fast inverse sqrt is only twice as fast as the normal sqrt, at best.
Thus, this algorithm does not seem to be worth using.
*/
