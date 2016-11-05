#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
import matplotlib.pyplot as plt
from scipy.integrate import cumtrapz

w=1.0
def PDF(x):
    return (np.sin(w*x)+1)*w/(-np.cos(w*np.pi)+w*np.pi+1.0)

if __name__=="__main__":
    table_in=array_input( binary_input("./cheby_test") )
    
    X_table=table_in.get_array()
    X=X_table.read_doubles()
    
    actual_Y_table=table_in.get_array()
    actual_Y=actual_Y_table.read_doubles()
    
    cheby_Y_table=table_in.get_array()
    cheby_Y=cheby_Y_table.read_doubles()
    
    simps_Y_table=table_in.get_array()
    simps_Y=simps_Y_table.read_doubles()
    
    cheby_samples_table=table_in.get_array()
    samples=cheby_samples_table.read_doubles()
    
    inverse_X_table=table_in.get_array()
    inverse_X=inverse_X_table.read_doubles()
    
    inverse_Y_table=table_in.get_array()
    inverse_Y=inverse_Y_table.read_doubles()
    
    plt.plot(X, cheby_Y, 'b')
    plt.plot(X, actual_Y, 'g')
    plt.plot(X, simps_Y, 'r')
    plt.show()
    
    
    plt.plot(X, np.abs(cheby_Y-actual_Y), 'b')
    plt.plot(X, np.abs(simps_Y-actual_Y), 'r')
    plt.show()
    
    plt.hist(samples, normed=True)
    X_pdf=np.linspace(0,np.pi,10000)
    plt.plot(X_pdf, PDF(X_pdf))
    plt.show()
    
    
    
    I=cumtrapz(actual_Y, X, initial=0)
    
    plt.plot(inverse_X, inverse_Y, '-o')
    plt.plot(I, X)
    plt.show()
    
