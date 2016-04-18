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
		energy=energy_kev*1000*elementary_charge/electron_rest_energy
		self.momentum_sq=(energy+1.0)*(energy+1.0)-1
		self.beta=np.sqrt(self.momentum_sq/(1+self.momentum_sq))

    def cross_section(self, angle):
		S=np.sin(angle/2.0)
		numerator=1.0-self.beta*self.beta*S*S
		denom=S*S+self.p_factor/self.momentum_sq
		return numerator*self.prefactor/(denom*denom*self.beta*self.momentum_sq)

    def integrand(self, angle):
        return self.cross_section(angle)*np.sin(angle)


if __name__=='__main__':
    table_index=9
    n_bins=20
    ##open
    table_in=array_input( binary_input("../tables/diffusion/0.01") )
    
    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()
    
    tables=[]
    for table_i in xrange(table_in.size-1):
        samples_table=table_in.get_array()
        samples=samples_table.read_doubles()
        tables.append(samples)
    print len(tables),"energies available"
    
    
    print len(tables[table_index]), "samples at:", energies[table_index], "kev"
    plt.hist(tables[table_index], bins=n_bins, range=(0, np.pi))
    plt.show()
    
    
    #~table_in=array_input( binary_input("./quadrature_tst") )
    #~
    #~cumquads_table=table_in.get_array()
    #~cumquads=cumquads_table.read_doubles()
    #~
    #~points_table=table_in.get_array()
    #~points=points_table.read_doubles()
    #~
    #~value_table=table_in.get_array()
    #~values=value_table.read_doubles()
    #~
    #~quad_X_table=table_in.get_array()
    #~quad_X=quad_X_table.read_doubles()
    #~
    #~quad_Y_table=table_in.get_array()
    #~quad_Y=quad_Y_table.read_doubles()
    #~
    #~plt.plot(points, values, 'r')
    #~plt.plot(points, cumquads, 'g')
    #~plt.show();
    #~
    #~plt.plot(cumquads, points, 'r')
    #~plt.plot(quad_X, quad_Y, 'g')
    #~plt.show()



    
