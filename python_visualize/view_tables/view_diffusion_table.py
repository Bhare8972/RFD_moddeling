#!/usr/bin/env python3

import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import cumtrapz, quad

import sys
sys.path.append('../')

from constants import *
from read_binary import binary_input, array_input

class CS_table(object):
    def __init__(self, timesteps, table_in):
        self.timesteps=timesteps
        
        self.distribution_data=[]
        
        for t in timesteps:
            self.distribution_data.append( table_in.read_doublesArray() )
            
    def get_CDF_Y(self, timestep_i):
        return np.arange( len(self.distribution_data[timestep_i]), dtype=float)/len(self.distribution_data[timestep_i])
            
    def get_CDF_X(self, timestep_i):
        return self.distribution_data[timestep_i]

if __name__=='__main__':
    
    energy_indeces=[49]
    time_indeces=  [10]
    

    table_in=array_input( binary_input("../../tables/shielded_coulomb_diffusion") )
    
    energies=table_in.read_doublesArray()
    timesteps=table_in.read_doublesArray()
    
    tables=[ CS_table(timesteps, table_in) for e in energies ]
    
    print( "E(keV):", energies*energy_units_kev)
    print("TS:", timesteps)
    
    print( len(energies), "energies")
    print( len(timesteps), "timesteps")
    
    
    
    for print_i in range(len(energy_indeces)):
        energy_index=energy_indeces[print_i]
        time_index=time_indeces[print_i]
        
        print( 'energy:', energies[energy_index]*energy_units_kev, "Kev")
        print( 'timestep:', timesteps[time_index])
        
        dist_X=tables[energy_index].get_CDF_X(time_index)
        dist_Y=tables[energy_index].get_CDF_Y(time_index)
        #print( dist_X )
         
        print( len(dist_X), 'counts')
        
        plt.plot(dist_X[::10], dist_Y[::10], 'g')
        plt.show()
    
    
    
    
    
    
