
#ifndef RFD_CONSTANTS
#define RFD_CONSTANTS

//some math constants
const double PI=3.1415926535897932384626;
const double euler_number=2.718281828459;

//some units
const double Giga=1.0e9;
const double Kilo=1.0e3;
const double Micro=1.0e-6;
const double Nano=1.0e-9;

//give constants
const double C=2.99792e8;  //meters per second
const double elementary_charge=1.602e-19; //charge of electron in coulombs
const double electron_rest_energy=510.998*Kilo*elementary_charge;   //(9.1093835e-31)*C*C;// in joules   //510.998; //in keV
const double average_air_atomic_number=7.266; //average charge per air atom
const double average_air_atomic_density=5.3518e25;
const double hbar=1.054571726e-34; //J per radius per second
const double electron_classical_radius=2.8179E-15; //m


//const double minimum_energy=2*Kilo*elementary_charge/electron_rest_energy; //minimum energy is 2 keV.   This needs to be removed
const double lowest_physical_energy=100.0*elementary_charge/electron_rest_energy; //lower energy the simulation is good for. 100 ev
const double inv_I_sq=electron_rest_energy*electron_rest_energy/(85.7*80.5*elementary_charge*elementary_charge); //parameter necisary for beth formula

//find unit-less constant conversions
const double time_units=172.0*Nano; // seconds
const double time_units_ns=172.0; // nano seconds
const double distance_units=C*time_units; //fundamental length scale, in  meters
const double E_field_units=electron_rest_energy/(elementary_charge*C*time_units); //units of electric field in V/m
const double B_field_units=E_field_units/C; //units of magnetic field in T
const double energy_units_kev=electron_rest_energy/(1000.0*elementary_charge); //units of energy in terms of keV

#endif
