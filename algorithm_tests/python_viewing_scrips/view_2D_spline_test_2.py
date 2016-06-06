#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib import pyplot as plt


if __name__=='__main__':
    
    show_val=50;
    
    table_in=array_input( binary_input("./cache_integrand_test") )
    
    X_Table=table_in.get_array()
    angles=X_Table.read_doubles()
    
    for x in xrange(show_val+1):
    
        theta_prime_table=table_in.get_array()
        theta_prime=theta_prime_table.read_doubles()
    
        values_table=table_in.get_array()
        values=values_table.read_doubles()
        
        print x
        
    
    plt.plot(theta_prime, values)
    plt.show()
    
    
    
    
    
