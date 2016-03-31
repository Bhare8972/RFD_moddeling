#!/usr/bin/python

##constants used in the C++
## this need to be the same from the simulation to insure accurate displaying


##some units
giga=1.0e9
kilo=1.0e3
micro=1.0e-6
nano=1.0e-9

##give constants
C=2.99792e8 #meters per second
electron_rest_energy=9.1093835e-31*C*C# in joules  ##510.998 #in keV
elementary_charge=1.602e-19 #charge of electron in coulombs
average_air_atomic_number=14.5

minimum_energy=2*kilo*elementary_charge/electron_rest_energy #minimum energy is 2 keV.
inv_I_sq=electron_rest_energy*electron_rest_energy/(85.7*85.7*elementary_charge*elementary_charge) #parameter necisary for beth formula

##find unit-less constant conversions
time_units=172.0*nano # seconds
distance_units=C*time_units #fundamental length scale, in meters
E_field_units=electron_rest_energy/(elementary_charge*C*time_units) #units of electric field in G V/m
B_field_units=E_field_units/C #units of magnetic field in GT

##constants not used in simulation
electron_mass=9.1093835e-31 ## mass of electron in kg
