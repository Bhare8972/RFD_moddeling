////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
//#include "vector.h"
//
////units:
////energy: keV
////time: nano seconds
//
//
////give constants
//double electron_rest_mass=510.998;
//
////find unit-less constants
//tau=172 //nano seconds
//
//
//class electron
//{
//public:
//    vector position;
//    vector momentum; //dimensionless, in units of electron-rest-mass/c
//};

#include <stdio.h>
#include "gsl/gsl_sf_bessel.h"

int
main (void)
{
  double x = 5.0;
  double y = gsl_sf_bessel_J0 (x);
  printf ("J0(%g) = %.18e\n", x, y);
  return 0;
}s
