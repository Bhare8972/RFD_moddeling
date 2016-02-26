#!/usr/bin/python

##constants used in the C++
## this need to be the same from the simulation to insure accurate displaying

##give constants
C=2.99792e8 #meters per second
electron_rest_mass=9.1093835e-31*C*C# in joules  ##510.998 #in keV
elementary_charge=1.602e-19 #charge of electron in coulombs

##some units
kilo=1.0e3
nano=1.0e-9

##find unit-less constant conversions
time_units=172.0*nano # seconds
distance_units=C*time_units #fundamental length scale, in meters
E_field_units=electron_rest_mass/(elementary_charge*C*time_units) #units of electric field in G V/m
B_field_units=E_field_units/C #units of magnetic field in GT

##constants not used in simulation
electron_mass=9.1093835e-31 ## mass of electron in kg
