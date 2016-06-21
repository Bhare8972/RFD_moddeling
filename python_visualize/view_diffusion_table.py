#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt
from scipy.integrate import quad, cumtrapz

from constants import *

class diff_cross_section(object):
    def __init__(self, timestep, energy=0):
        self.timestep=timestep
        self.prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926)
        self.p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3)
        self.set_energy(energy)

    def set_energy(self, energy):
        self.energy=energy
        self.momentum_sq=(energy+1.0)*(energy+1.0)-1
        self.momentum=np.sqrt(self.momentum_sq)
        self.beta=np.sqrt(self.momentum_sq/(1+self.momentum_sq))
        self.gamma=energy-1
        
        self.diffusion_Ddt=self.timestep*average_air_atomic_number*np.log((164.7*self.momentum/pow(average_air_atomic_number, 1.0/3.0)))/(self.beta*self.beta*self.beta*self.gamma*self.gamma)
    

    def cross_section(self, angle):
        S=np.sin(angle/2.0)
        numerator=1.0-self.beta*self.beta*S*S
        denom=S*S+self.p_factor/self.momentum_sq
        return numerator*self.prefactor/(denom*denom*self.beta*self.momentum_sq)

    def integrand(self, angle):
        return self.cross_section(angle)*np.sin(angle)
        
    def diffusion_approximation(self, angle):
        U=np.cos(angle)
        if(self.diffusion_Ddt>0.1):
            print "ERROR"
            return 0
        else:
            return np.exp((U-1)/(2*self.diffusion_Ddt))/(2*self.diffusion_Ddt)
            
    def diffusion_integrand(self, N):
        print "diffustion dt", self.diffusion_Ddt
        theta_space=np.linspace(0,np.pi,N)
        F=self.diffusion_approximation(theta_space)
        F_integrate=cumtrapz(y=F, x=theta_space, initial=0)
        
        return F_integrate/F_integrate[-1], theta_space
        
    def mollier_scattering(self):
        
        

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
    
    energy_indeces=[198]
    time_indeces=[0]
    
    if len(energy_indeces)!=len(time_indeces):
        print "ERROR"
        exit()
    
    ##open
    table_in=array_input( binary_input("../tables/shielded_coulomb_diffusion") )

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
        
        CS=diff_cross_section(timesteps[time_index], energies[energy_index])
        diffusion_X, diffusion_Y=CS.diffusion_integrand(1000)
        
        plt.plot(diffusion_X, diffusion_Y)
        plt.plot(dist_X, dist_Y)
        plt.show()



