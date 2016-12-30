#!/usr/bin/env python3

import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import quad

import sys
sys.path.append('../')

from constants import *
from read_binary import array_input, binary_input

class moller_cross_section(object):

    def __init__(self, energy_=0):
        self.set_energy(energy_)

    def set_energy(self, energy_):
        self.energy=energy_
        momentum_sq=(self.energy+1.0)*(self.energy+1.0)-1
        self.momentum=np.sqrt(momentum_sq)
        self.gamma=np.sqrt(1+momentum_sq)
        self.beta=self.momentum/self.gamma

    def __call__(self, production_energy):
    ##return cross section per tau
        term_1_sqrt=self.energy/(production_energy*(self.energy-production_energy))
        term_2_num=2*(self.gamma*self.gamma+self.gamma)-1
        term_2_denom=production_energy*(self.energy-production_energy)*self.gamma*self.gamma

        return (term_1_sqrt*term_1_sqrt - term_2_num/term_2_denom + 1.0/(self.gamma*self.gamma))/self.beta
        
    def integral_test(self, production_energy):
        term_2_F1=(2*self.gamma-1)/(self.energy*self.gamma*self.gamma);
        term_2_F2=np.log(production_energy/(self.energy-production_energy));
        term_3=(2*production_energy-self.energy)/(production_energy*(production_energy-self.energy));

        return (production_energy/self.gamma-term_2_F1*term_2_F2-term_3 )/self.beta;
        

    def integrate(self, min_energy, upper_energy=None):
        if(upper_energy==None):
            upper_energy=self.energy/2.0
        G=quad(self, min_energy, upper_energy)
        return G


if __name__=="__main__":

    ##open data
    table_in=array_input( binary_input("./moller_tables_output") )
    lowest_sim_energy=2.0/energy_units_kev

    energies=table_in.read_doublesArray()
    print( len(energies), "energies available" )
    
    energy_indexes=np.arange(len(energies))[::10]
    tabled_rates=[]
    
    for i in range(len(energies)):
        production_energies=table_in.read_doublesArray()
        un_normedCDF=table_in.read_doublesArray()
        tabled_rates.append(un_normedCDF[-1])
        
        
        if i in energy_indexes: 
            print( i,":",energies[i]*energy_units_kev, "keV" )
            print( "  :",un_normedCDF[-1],"interactions per tau tabled")
            MC=moller_cross_section(energies[i])
            V,E=MC.integrate(lowest_sim_energy)
            print( "  :",V,"+/-", E, "interactions per tau gaussian integrated")
            
            plt.plot(production_energies*energy_units_kev, un_normedCDF, 'ro-')
            
            CDF_I=[]
            for pe in production_energies:
                CDF_I.append( MC.integrate(lowest_sim_energy, pe)  )
            plt.plot( production_energies*energy_units_kev, CDF_I , 'b')
            
            plt.show()
            
    plt.plot(energies*energy_units_kev, tabled_rates, 'o')
    plt.show()

