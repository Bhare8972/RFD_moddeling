#!/usr/bin/python3

##constants used in the simulation
## this need to be the same from the simulation to insure accurate displaying

RTD=180.0/(3.1415926);

##some units
giga=1.0e9
kilo=1.0e3
micro=1.0e-6
nano=1.0e-9

##give constants
C=2.99792e8 #meters per second
electron_rest_energy=9.1093835e-31*C*C# in joules  ##510.998 #in keV
elementary_charge=1.602e-19 #charge of electron in coulombs
average_air_atomic_number=7.266
average_air_atomic_density=5.3518e25
hbar=1.054571726e-34
electron_classical_radius=2.8179E-15

lowest_physical_energy=100.0*elementary_charge/electron_rest_energy; ##lower energy the simulation is good for
inv_I_sq=electron_rest_energy*electron_rest_energy/(85.7*85.7*elementary_charge*elementary_charge) #parameter necisary for beth formula

##find unit-less constant conversions
time_units=172.0*nano # seconds
time_units_ns=172.0
distance_units=C*time_units #fundamental length scale, in meters
E_field_units=electron_rest_energy/(elementary_charge*C*time_units) #units of electric field in G V/m
B_field_units=E_field_units/C #units of magnetic field in GT
energy_units_kev=510.998 ##units of energy in terms of keV

##constants not used in simulation
electron_mass=9.1093835e-31 ## mass of electron in kg
