#!/usr/bin/env python

import numpy as np
from read_binary import binary_input, array_input
from matplotlib import pyplot as plt
from scipy.integrate import cumtrapz, quad

from constants import *

class diff_cross_section:
    def __init__(self, energy):
        self.nitrogen_prefactor=0.784*7.0*7.0/(4.0*average_air_atomic_number)
        self.nitrogen_p_factor=(7.0**(2.0/3.0))/(4*183.3*183.3)
        self.oxygen_prefactor=0.211*8.0*8.0/(4.0*average_air_atomic_number)
        self.oxygen_p_factor=(8.0**(2.0/3.0))/(4*183.3*183.3)
        self.argon_prefactor=0.005*18.0*18.0/(4.0*average_air_atomic_number)
        self.argon_p_factor=(18.0**(2.0/3.0))/(4*183.3*183.3)
        
        self.energy=energy
        self.momentum_sq=(energy+1.0)*(energy+1.0)-1
        self.beta_sq=self.momentum_sq/(1+self.momentum_sq)
        self.beta=np.sqrt(self.beta_sq)

        self.nitrogen_p_factor/=self.momentum_sq
        self.oxygen_p_factor/=self.momentum_sq
        self.argon_p_factor/=self.momentum_sq

        self.nitrogen_prefactor/=(self.beta*self.momentum_sq)
        self.oxygen_prefactor/=(self.beta*self.momentum_sq)
        self.argon_prefactor/=(self.beta*self.momentum_sq)

        ##self.CDF_offset=0.0
        ##self.CDF_offset=CDF(0.0)
        
    def CS(self, angle):
        S2=np.sin(angle*0.5)
        S2*=S2
        
        num=(1.0-self.beta_sq*S2)*np.sin(angle)
        
        nitrogen_denom_sqrt=S2+self.nitrogen_p_factor
        oxygen_denom_sqrt=S2+self.oxygen_p_factor
        argon_denom_sqrt=S2+self.argon_p_factor
        
        return num*( self.nitrogen_prefactor/(nitrogen_denom_sqrt*nitrogen_denom_sqrt) + self.oxygen_prefactor/(oxygen_denom_sqrt*oxygen_denom_sqrt) + self.argon_prefactor/(argon_denom_sqrt*argon_denom_sqrt) )


##  double call(double angle)
##        double S_2=std::sin(angle*0.5);
##      S_2*=S_2;


##      double nitrogen_denom=nitrogen_p_factor + S_2;
##      double oxygen_denom=oxygen_p_factor + S_2;
##      double argon_denom=argon_p_factor + S_2;

##      double nitrogen_num=1.0-beta_sq*nitrogen_p_factor;
##      double oxygen_num=1.0-beta_sq*oxygen_p_factor;
##      double argon_num=1.0-beta_sq*argon_p_factor;

##      return nitrogen_prefactor*(std::log(nitrogen_denom) - nitrogen_num/nitrogen_denom) + oxygen_prefactor*(std::log(oxygen_denom)-oxygen_num/oxygen_num) + argon_prefactor*(std::log(argon_denom)-argon_num/argon_denom) - CDF_offset;
        

class CS_table(object):
    def __init__(self, num_timesteps, table_in):
        self.dist_X=[]
        self.dist_Y=[]
        
        for x in range(num_timesteps):
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

    table_in=array_input( binary_input("./error_dump") )
    samples=table_in.read_doublesArray()
    
    X=[0.0]
    Y=[0.0]
    for i in range(len(samples)):
        Y.append(Y[-1])
        Y.append(Y[-1]+1.0)
        X.append(samples[i])
        X.append(samples[i])
    Y=np.array(Y)
    Y/=len(samples)
    plt.plot(X,Y)
    plt.show()
    
    print(len(samples))
    
    quit()
    
    energy=100.0/energy_units_kev
    max_theta=0.5
    n_bins=100
    
    table_in=array_input( binary_input("./SCD_tst") )
    samples=table_in.read_doublesArray()
    
    print("read")
    print(samples)
    print()
    
    diff_CS=diff_cross_section(energy)
    I=quad(diff_CS.CS, 0, max_theta)
    print( I )
    
    plt.hist(samples, bins=n_bins, range=(0,max_theta), normed=True)
    Z=np.linspace(0, max_theta, 5*n_bins)
    plt.plot(Z, diff_CS.CS(Z)/I[0])
    plt.show()
    



