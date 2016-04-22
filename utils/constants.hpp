
#ifndef RFD_CONSTANTS
#define RFD_CONSTAMTS

//some units
double Giga=1.0e9;
double Kilo=1.0e3;
double Micro=1.0e-6;
double Nano=1.0e-9;

//give constants
double C=2.99792e8;  //meters per second
double elementary_charge=1.602e-19; //charge of electron in coulombs
double electron_rest_energy=510.998*Kilo*elementary_charge;   //(9.1093835e-31)*C*C;// in joules   //510.998; //in keV
double average_air_atomic_number=14.5;


double minimum_energy=2*Kilo*elementary_charge/electron_rest_energy; //minimum energy is 2 keV.
double inv_I_sq=electron_rest_energy*electron_rest_energy/(85.7*80.5*elementary_charge*elementary_charge); //parameter necisary for beth formula

//find unit-less constant conversions
double time_units=172.0*Nano; // seconds
double distance_units=C*time_units; //fundamental length scale, in  meters
double E_field_units=electron_rest_energy/(elementary_charge*C*time_units); //units of electric field in V/m
double B_field_units=E_field_units/C; //units of magnetic field in T
double energy_units_kev=electron_rest_energy/(1000.0*elementary_charge); //units of energy in terms of keV

#endif
