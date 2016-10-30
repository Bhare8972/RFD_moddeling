#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
import matplotlib.pyplot as plt
from scipy.integrate import simps, cumtrapz


if __name__=="__main__":
    table_in=array_input( binary_input("./brem_test_out") )
    
    X_space_table=table_in.get_array()
    X_space=X_space_table.read_doubles()
    
    cs_table=table_in.get_array()
    CS=cs_table.read_doubles()
    
    
    walker_samples_tables=table_in.get_array()
    walker_samples=walker_samples_tables.read_doubles()
    
    #inverse_samples_tables=table_in.get_array()
    #inverse_samples=inverse_samples_tables.read_doubles()
    
    
    #X_space_table_FS=table_in.get_array()
    #X_space_FS=X_space_table_FS.read_doubles()
    
    #cs_table_FS=table_in.get_array()
    #CS_FS=cs_table_FS.read_doubles()
    
    ##cumquad_table=table_in.get_array()
    ##cumquads=cumquad_table.read_doubles()
    
    
    
    ##plt.plot(X_space, CS, 'bo-')
    ##plt.plot(X_space_FS, CS_FS, 'ro-')
    ##plt.show()
    
    
    
    #cumtrapz_calc=cumtrapz(CS, X_space, initial=0);
    #plt.plot(X_space, cumtrapz_calc, 'bo-')
    #plt.plot(CS_FS, X_space_FS, 'ro-')
    #plt.show()


    plt.plot(X_space, CS/0.0313852)
    plt.hist(walker_samples, bins=10000, normed=True, histtype="step")
    #plt.hist(inverse_samples, bins=1000, normed=True, histtype="step")
    plt.show()
