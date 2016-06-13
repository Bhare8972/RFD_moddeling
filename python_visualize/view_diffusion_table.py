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


class CS_table(object):
    def __init__(self, num_timesteps, table_in):
        self.dist_X=[]
        self.dist_Y=[]
        
        for x in xrange(num_timesteps):
            X_table=table_in.get_array()
            X_values=X_table.read_doubles()
            Y_table=table_in.get_array()
            Y_values=Y_table.read_doubles()
            
            self.dist_X.append(X_values)
            self.dist_Y.append(Y_values)
            
    def get_X(self, timestep):
        return self.dist_X[timestep]
            
    def get_Y(self, timestep):
        return self.dist_Y[timestep]

if __name__=='__main__':
    
    energy_indeces=[99,99,99,99,99,99,99,99,99,99]
    time_indeces=[0,2,4,6,8,10,12,14,16,18]
    
    if len(energy_indeces)!=len(time_indeces):
        print "ERROR"
        exit()
    
    ##open
    table_in=array_input( binary_input("./diffusion_table_MONTECARLO_A") )

    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()
    timesteps_table=table_in.get_array()
    timesteps=timesteps_table.read_doubles()
    
    energies=energies[1:]
    
    print len(energies), "energies"
    print len(timesteps), "timesteps"
    
    tables=[]
    for table_i in xrange(len(energies)):
        tables.append(CS_table(len(timesteps), table_in))

    for print_i in xrange(len(energy_indeces)):
        energy_index=energy_indeces[print_i]
        time_index=time_indeces[print_i]
        
        print 'energy:', energies[energy_index]
        print 'timestep:', timesteps[time_index]
        
        dist_X=tables[energy_index].get_X(time_index)
        dist_Y=tables[energy_index].get_Y(time_index)
        plt.plot(dist_X, dist_Y)
        plt.show()



