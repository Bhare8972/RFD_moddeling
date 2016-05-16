
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
    table_in=array_input( binary_input("../tables/bethe_info") )

    electron_mom_sq_table=table_in.get_array()
    raw_electron_mom_sq=electron_mom_sq_table.read_doubles()
    print len(raw_electron_mom_sq), "raw points"
	
    electron_SP_table=table_in.get_array()
    raw_electron_SP=electron_SP_table.read_doubles()
	
    ##converion factors
    SP_conversion_factor=elementary_charge*1.0E8/(2.0*3.1415926*average_air_atomic_number*electron_classical_radius*electron_rest_energy*electron_classical_radius*air_molecular_density)
    kev_to_dimensionless=kilo*elementary_charge/electron_rest_energy
	
    ##print!
    raw_electron_SP_native=raw_electron_SP

    raw_electron_energy=(np.sqrt(raw_electron_mom_sq+1.0)-1.0)/kev_to_dimensionless
    raw_electron_SP=raw_electron_SP/(SP_conversion_factor*density)

    beth_calculation=beth_force(raw_electron_mom_sq)

    ##plot like lehtinen!
    plt.plot(raw_electron_energy/1000.0, raw_electron_SP/10, label='table electron SP') ##stopping power in units of MeV m^2 kg^-1??
    ##plt.plot(positron_energy/1000.0, positron_SP/10, label='table positron SP') ##energ in units of Mev
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim(1.0e-3, 1.0e2)
    plt.ylim(1.0e-1, 1.0e1)
    plt.xlabel("Energy in Mev")
    plt.ylabel("stopping power in MeV m^2 kg^-1")
    plt.legend()
    plt.show()

    ##plot like dwyer
    plt.plot(raw_electron_energy, raw_electron_SP*1000*density, label='table electron SP') ##stopping power in units of KeV /cm
    ##plt.plot(positron_energy, positron_SP*1000*density, label='table positron SP') ##energ in units of Kev
    plt.plot(raw_electron_energy, beth_calculation/(elementary_charge*SP_conversion_factor*1.0e5))
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim(1.0e-2, 1.0e6)
    plt.ylim(1, 1.0e3)
    plt.xlabel("Energy in Kev")
    plt.ylabel("stopping power in Kev/cm")
    plt.legend()
    plt.show()


    ##plot native units
    plt.plot(raw_electron_mom_sq, raw_electron_SP_native, label='table electron SP')
    plt.plot(raw_electron_mom_sq, beth_calculation, label='beth formula')
    ##plt.plot(positron_mom_sq, positron_SP_native, label='table positron SP')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel("Energy (unitless)")
    plt.ylabel("stopping power (unitless)")
    plt.legend()
    plt.show()


    #### some experements ####
    ## first, linear log-log interp
    powers=np.log( raw_electron_SP_native[1:]/raw_electron_SP_native[:-1] )/np.log( raw_electron_mom_sq[1:]/raw_electron_mom_sq[:-1] )
    factors=raw_electron_SP_native[:-1]*(raw_electron_mom_sq[:-1]**(-powers))
    
    mom_to_tst=np.logspace( np.log10(raw_electron_mom_sq[0]), np.log10(raw_electron_mom_sq[-1]), 10000)[:-1]
    indexes=np.searchsorted(raw_electron_mom_sq,  mom_to_tst)-1
    SP_test=factors[indexes]*(mom_to_tst**powers[indexes])
    
    plt.plot(mom_to_tst, SP_test, 'r', label='linear in loglog')
    plt.plot(raw_electron_mom_sq, raw_electron_SP_native, 'bo', label='table electron SP')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel("Energy (unitless)")
    plt.ylabel("stopping power (unitless)")
    plt.legend()
    plt.show()
    
    plt.plot(mom_to_tst, SP_test, 'r', label='linear in loglog')
    plt.plot(raw_electron_mom_sq, raw_electron_SP_native, 'bo', label='table electron SP')
    plt.xlabel("Energy (unitless)")
    plt.ylabel("stopping power (unitless)")
    plt.legend()
    plt.show()
    
    ##re-make interpolant to have less points and to be logrithmic
    n_points=100
    interp_mom_sq=np.logspace( np.log10(raw_electron_mom_sq[0]), np.log10(raw_electron_mom_sq[-1]), n_points)[:-1]
    
    indexes=np.searchsorted(raw_electron_mom_sq,  interp_mom_sq)-1
    interp_samples=factors[indexes]*(interp_mom_sq**powers[indexes])
    
    interp_powers=np.log( interp_samples[1:]/interp_samples[:-1] )/np.log( interp_mom_sq[1:]/interp_mom_sq[:-1] )
    interp_factors=interp_samples[:-1]*(interp_mom_sq[:-1]**(-interp_powers))
    
    ## then make plotting points
    interp_plot_mom_sq=np.logspace( np.log10(interp_mom_sq[0]), np.log10(interp_mom_sq[-1]), 10000)[:-1]
    indexes=np.searchsorted(interp_mom_sq,  interp_plot_mom_sq)-1
    interped_SP_test=interp_factors[indexes]*(interp_plot_mom_sq**interp_powers[indexes])
    
    plt.plot(interp_plot_mom_sq, interped_SP_test, 'r', label='interped')
    plt.plot(raw_electron_mom_sq, raw_electron_SP_native, 'bo', label='table electron SP')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel("Energy (unitless)")
    plt.ylabel("stopping power (unitless)")
    plt.legend()
    plt.show()
    
    plt.plot(interp_plot_mom_sq, interped_SP_test, 'r', label='interped')
    plt.plot(raw_electron_mom_sq, raw_electron_SP_native, 'bo', label='table electron SP')
    plt.xlabel("Energy (unitless)")
    plt.ylabel("stopping power (unitless)")
    plt.legend()
    plt.show()


