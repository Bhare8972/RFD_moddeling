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
    
    X_space_table_FS=table_in.get_array()
    X_space_FS=X_space_table_FS.read_doubles()
    
    cs_table_FS=table_in.get_array()
    CS_FS=cs_table_FS.read_doubles()
    
    ##cumquad_table=table_in.get_array()
    ##cumquads=cumquad_table.read_doubles()
    
    plt.plot(X_space, CS, 'bo-')
    plt.plot(X_space_FS, CS_FS, 'ro-')
    plt.show()
    
    ##cumtrapz_calc=cumtrapz(CS, X_space, initial=0);
    ##plt.plot(X_space, cumtrapz_calc, 'ro-')
    ##plt.plot(X_space, cumquads, 'go-')
    ##plt.show()
    
    ##print( simps(y=CS, x=X_space) )
    #print( simps(y=CS_FS, x=X_space_FS) )
