#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt
from scipy.integrate import quad

from constants import *

class diff_cross_section(object):
    def __init__(self, timestep, energy_kev=0):
		self.prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926)
		self.p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3)
		self.set_energy(energy_kev)

    def set_energy(self, energy_kev):
		energy=energy_kev/(elementary_charge*electron_rest_energy)
		self.momentum_sq=(energy+1.0)*(energy+1.0)-1
		self.beta=np.sqrt(self.momentum_sq/(1+self.momentum_sq))

    def call(self, angle):
		S=np.sin(angle/2.0)
		numerator=1.0-self.beta*self.beta*S*S
		denom=S*S+self.p_factor/self.momentum_sq
		return numerator*self.prefactor/(denom*denom*self.beta*self.momentum_sq)



if __name__=='__main__':
    
    ##open
    table_in=array_input( binary_input("./tst") )
    
    points_table=table_in.get_array()
    points=points_table.read_doubles()
	
    values_table=table_in.get_array()
    values=values_table.read_doubles()
	
    cum_quads_table=table_in.get_array()
    cum_quads=cum_quads_table.read_doubles()
    print "my integrate:", cum_quads[-1]
    
    ##self plot
    scatterer=diff_cross_section(0.01, 30000)
    angles=np.linspace(0, np.pi, 1000000)
    cross_sections=scatterer.call(angles)
    print "scipy integrate:", quad(scatterer.call, 0, np.pi, full_output=0, epsabs=1.0e-71, limit=200)
    
    
    plt.plot(points, values, 'bo')
    plt.plot(angles, cross_sections, 'b-')
    #plt.xscale('log')
    plt.yscale('log')
    #plt.plot(points, cum_quads, 'ro--')
    ##plt.plot(points, func(points), 'b-')
    ##plt.plot(points, I(points), 'r-')
    plt.show()
    
