#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt
from scipy.integrate import quad, cumtrapz
from scipy.optimize import brentq
from scipy.interpolate import interp1d

from constants import *

#### first three terms in BETHE expansion used in Mollier theory ####
BETHE_EXPANSION_X=np.array([ 
 0.00, 0.10, 0.20, 0.30,
 0.40, 0.50, 0.60, 0.70,
 0.80, 0.90, 1.00, 1.10,
 1.20, 1.30, 1.40, 1.50,
 1.60, 1.70, 1.80, 1.90,
 2.00, 2.20, 2.40, 2.60,
 2.80, 3.00, 3.20, 3.40,
 3.60, 3.80, 4.00, 5.00,
 6.00, 7.00, 8.00, 9.00,
 10.00,11.00,12.00,13.00])

BETHE_EXPANSION_0=np.array( [ 
 0.000000E+00 ,0.995016E-02 ,0.392106E-01 ,0.860688E-01,
 0.147856E+00 ,0.221199E+00 ,0.302324E+00 ,0.387374E+00,
 0.472708E+00 ,0.555142E+00 ,0.632121E+00 ,0.701803E+00,
 0.763072E+00 ,0.815480E+00 ,0.859142E+00 ,0.894601E+00,
 0.922695E+00 ,0.944424E+00 ,0.960836E+00 ,0.972948E+00,
 0.981684E+00 ,0.992093E+00 ,0.996849E+00 ,0.998841E+00,
 0.999606E+00 ,0.999877E+00 ,0.999964E+00 ,0.999990E+00,
 0.999998E+00 ,0.999999E+00 ,0.100000E+01 ,0.100000E+01,
 0.100000E+01 ,0.100000E+01 ,0.100000E+01 ,0.100000E+01,
 1.000000E+00 ,1.000000E+00 ,1.000000E+00 ,1.000000E+00 ])

BETHE_EXPANSION_1=np.array( [ 
 0.000000E+00, 0.414985E-02, 0.154894E-01, 0.310312E-01,
 0.464438E-01, 0.569008E-01, 0.580763E-01, 0.468264E-01,
 0.217924E-01,-0.163419E-01,-0.651205E-01,-0.120503E+00,
 -0.178272E+00,-0.233580E+00,-0.282442E+00,-0.321901E+00,
 -0.350115E+00,-0.366534E+00,-0.371831E+00,-0.367378E+00,
 -0.354994E+00,-0.314803E+00,-0.266539E+00,-0.220551E+00,
 -0.181546E+00,-0.150427E+00,-0.126404E+00,-0.107830E+00,
 -0.933106E-01,-0.817375E-01,-0.723389E-01,-0.436650E-01,
 -0.294700E-01,-0.212940E-01,-0.161406E-01,-0.126604E-01,
 -0.102042E-01,-0.840465E-02,-0.704261E-02,-0.598886E-02])

BETHE_EXPANSION_2=np.array( [ 
 0.000000    , 0.121500E-01, 0.454999E-01, 0.913000E-01,
 0.137300E+00, 0.171400E+00, 0.183900E+00, 0.170300E+00,
 0.132200E+00, 0.763000E-01, 0.126500E-01,-0.473500E-01,
 -0.936000E-01,-0.119750E+00,-0.123450E+00,-0.106300E+00,
 -0.732800E-01,-0.312400E-01, 0.128450E-01, 0.528800E-01,
  0.844100E-01, 0.114710E+00, 0.106200E+00, 0.765830E-01,
  0.435800E-01, 0.173950E-01, 0.695001E-03,-0.809500E-02,
 -0.117355E-01,-0.125449E-01,-0.120280E-01,-0.686530E-02,
 -0.385275E-02,-0.231115E-02,-0.147056E-02,-0.982480E-03,
 -0.682440E-03,-0.489715E-03,-0.361190E-03,-0.272582E-03])
 
BETHE_FUNC_0=interp1d(BETHE_EXPANSION_X, BETHE_EXPANSION_0, kind='quadratic', assume_sorted=True)
BETHE_FUNC_1=interp1d(BETHE_EXPANSION_X, BETHE_EXPANSION_1, kind='quadratic', assume_sorted=True)
BETHE_FUNC_2=interp1d(BETHE_EXPANSION_X, BETHE_EXPANSION_2, kind='quadratic', assume_sorted=True)
 
fine_structure_constant=1.0/137.036

class diff_cross_section(object):
    def __init__(self, timestep, energy=0):
        self.timestep=timestep
        self.prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926)
        self.p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3)
        self.set_energy(energy)
        
        self.integrated_num_interactions=quad(self.cross_section, 0, np.pi)[0]
        #print "predicted num interactions:", self.integrated_num_interactions
        
        self.set_mollier_variables()

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
            print( "ERROR")
            return 0
        else:
            return np.exp((U-1)/(2*self.diffusion_Ddt))/(2*self.diffusion_Ddt)
            
    def diffusion_integrand(self, N):
        print( "diffustion dt", self.diffusion_Ddt)
        theta_space=np.linspace(0,np.pi,N)
        F=self.diffusion_approximation(theta_space)
        F_integrate=cumtrapz(y=F, x=theta_space, initial=0)
        
        return F_integrate/F_integrate[-1], theta_space
        
    def set_mollier_variables(self):
        #### first set the atmosphere  ####
        M_ave=14.54 ## average atomic weight of air
        n1=0.784    ##fraction of air that is N2. Should this be fraction of ATOMS?
        n2=0.211    ##fraction of air that is O2. should this frcation be attoms??
        n3=0.005    ## argon
        Z1= 7.0     ## charge of Nitrogen
        Z2= 8.0     ##charge of oxegen
        Z3= 18.0    ##charge of argon
        
        Qs=n1*Z1*(Z1+1) + n2*Z2*(Z2+1) + n3*Z3*(Z3+1)
        Qe=n1*Z1*(Z1+1)*np.log(Z1**(-2.0/3.0)) + n2*Z2*(Z2+1)*np.log(Z2**(-2.0/3.0)) + n3*Z3*(Z3+1)*np.log(Z3**(-2.0/3.0))
        Qx=n1*Z1*(Z1+1)*np.log(1.0 + 3.34*(Z1*fine_structure_constant)) + \
            n2*Z2*(Z2+1)*np.log(1.0 + 3.34*(Z2*fine_structure_constant)) + \
            n3*Z3*(Z3+1)*np.log(1.0 + 3.34*(Z3*fine_structure_constant))
            
        #### find number of interactions, omega ####
        omega_prefactor=6702.33*Qs*np.exp( (Qe-Qx)/Qs)/M_ave
        distance_traveled=C*self.timestep*time_units*self.beta
        ##num_interactions=omega_prefactor*distance_traveled/(self.beta*self.beta)
        num_interactions=self.integrated_num_interactions
        
        print( "mollier theory:", num_interactions)
        
        if(num_interactions<20):
            print( "mollier theory not applicable")
        
        #### next we find 'B' ####
        def B_finder(B): ##B solves this equation
            return B-np.log(B*num_interactions)
        self.B=brentq(B_finder, 1, num_interactions)
        
        ### then the critical angle###
        critical_angle=0.00039612*np.sqrt(Qs*distance_traveled/M_ave)/(self.beta*self.beta*self.energy*energy_units_kev*10**-6) ##energy units need to be GeV
        
        #### info for reduced angle
        self.angle_reduction_factor=1.0/(critical_angle*np.sqrt(self.B))
        
        
    def mollier_scattering(self, theta):
        ##first  find reduced angles
        reduced_angle=theta*self.angle_reduction_factor
        
        ##find F
        F=BETHE_FUNC_0(reduced_angle) + (BETHE_FUNC_1(reduced_angle) + BETHE_FUNC_2(reduced_angle)/self.B)/self.B
        
        ##solution!!
        return np.sqrt( np.sin(theta)/theta )*F

        

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
    
    energy_indeces=[92, 94, 96, 98, 100, 102, 104, 106]
    t=10
    time_indeces=[t, t, t, t, t, t, t, t]
    
    if len(energy_indeces)!=len(time_indeces):
        print( "ERROR" )
        exit()
    
    ##open
    table_in=array_input( binary_input("../tables/shielded_coulomb_diffusion") )

    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()
    timesteps_table=table_in.get_array()
    timesteps=timesteps_table.read_doubles()
    
    print( "E(keV):", energies*energy_units_kev)
    print("TS:", timesteps)
    
    #energies=energies[1:]
    
    print( len(energies), "energies")
    print( len(timesteps), "timesteps")
    
    tables=[]
    for table_i in range(len(energies)):
        tables.append(CS_table(len(timesteps), table_in))

    for print_i in range(len(energy_indeces)):
        energy_index=energy_indeces[print_i]
        time_index=time_indeces[print_i]
        
        print( 'energy:', energies[energy_index]*energy_units_kev, "Kev")
        print( 'timestep:', timesteps[time_index])
        
        dist_X=tables[energy_index].get_X(time_index)
        dist_Y=tables[energy_index].get_Y(time_index)
        
        #CS=diff_cross_section(timesteps[time_index], energies[energy_index])
        
        #mollier_theta=np.linspace(0.0001,np.pi,100000)
        #MS=CS.mollier_scattering(mollier_theta)
        #MS_integrand=cumtrapz(y=MS, x=mollier_theta, initial=0)
       # plt.plot(MS_integrand/MS_integrand[-1], mollier_theta,'r')
        
        ##diffusion_X, diffusion_Y=CS.diffusion_integrand(1000)
        ##plt.plot(diffusion_X, diffusion_Y)
        plt.plot(dist_X, dist_Y)
    plt.show()



