
import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt

from constants import *

density=1.205E-3 ## g cm^-3
air_molecular_density=2.688E25 # m^-3
electron_classical_radius=2.8179E-15

def beth_force( momentum_squared):
    gamma_squared=1+momentum_squared
    gamma_=np.sqrt(gamma_squared)
    inv_beta_squared=gamma_squared/momentum_squared

    #term1=np.log(inv_I_sq*momentum_squared*(gamma_-1.0))
    term1=np.log(inv_I_sq*momentum_squared*(gamma_-1.0))
    term2=(1+(2.0/gamma_)-1.0/gamma_squared)*np.log(2.0)
    term3=(1.0-2.0/gamma_+1.0/gamma_squared)/8.0
    term4=1.0/gamma_squared

    return np.nan_to_num( inv_beta_squared*(term1 - term2 + term3 + term4))

if __name__=='__main__':
    table_in=array_input( binary_input("./ionization_losses") )
    
    electron_mom_sq_table=table_in.get_array()
    electron_mom_sq=electron_mom_sq_table.read_floats()
	
    electron_SP_table=table_in.get_array()
    electron_SP=electron_SP_table.read_doubles()
	
    positron_mom_sq_table=table_in.get_array()
    positron_mom_sq=positron_mom_sq_table.read_floats()
	
    positron_SP_table=table_in.get_array()
    positron_SP=positron_SP_table.read_doubles()
	
    ##converion factors
    SP_conversion_factor=1.0/(2.0*3.1415926*average_air_atomic_number*electron_classical_radius*electron_rest_energy*electron_classical_radius*air_molecular_density)
    kev_to_dimensionless=kilo*elementary_charge/electron_rest_energy
	
    ##print!
    electron_SP_native=electron_SP
    positron_SP_native=positron_SP
 
    electron_energy=(np.sqrt(electron_mom_sq+1.0)-1.0)/kev_to_dimensionless
    electron_SP=electron_SP/(SP_conversion_factor*density)
    
    positron_energy=(np.sqrt(positron_mom_sq+1.0)-1.0)/kev_to_dimensionless
    positron_SP=positron_SP/(SP_conversion_factor*density)

    print electron_energy
    print
    print electron_SP
    print
    print positron_energy
    print
    print positron_SP

    
    beth_calculation=beth_force(electron_mom_sq)

    ##plot like lehtinen!
    plt.plot(electron_energy/1000.0, electron_SP/10) ##stopping power in units of MeV m^2 kg^-1??
    plt.plot(positron_energy/1000.0, positron_SP/10) ##energ in units of Mev
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim(1.0e-3, 1.0e2)
    plt.ylim(1.0e-1, 1.0e1)
    plt.show()
    
    ##plot like dwyer
    plt.plot(electron_energy, electron_SP*1000*density) ##stopping power in units of KeV /cm
    plt.plot(positron_energy, positron_SP*1000*density) ##energ in units of Kev
    plt.plot(electron_energy, beth_calculation/(elementary_charge*SP_conversion_factor*1.0e5))
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim(1.0e-2, 1.0e6)
    plt.ylim(1, 1.0e3)
    plt.show()
    
    
    ##plot native units
    plt.plot(electron_mom_sq, electron_SP_native*elementary_charge*1.0E8)
    plt.plot(electron_mom_sq, beth_calculation)
    plt.plot(positron_mom_sq, positron_SP_native*elementary_charge*1.0E8)
    plt.xscale('log')
    plt.yscale('log')
    plt.show()
    
    
    
    