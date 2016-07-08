#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
import matplotlib.pyplot as plt
from scipy.integrate import cumtrapz

w=20.0
def PDF(x):
    return (np.sin(w*x)+1)*w/(-np.cos(w*np.pi)+w*np.pi+1.0)

if __name__=="__main__":
    table_in=array_input( binary_input("./walker_test") )
    
    old_ST=table_in.get_array()
    old_samples=old_ST.read_doubles()
    
    new_ST=table_in.get_array()
    new_samples=new_ST.read_doubles()
    
    print np.sum(new_samples<0)
    
    nbins=100
    plt.hist(old_samples, nbins, range=(0,np.pi), facecolor='green', alpha=0.75, normed=True)
    plt.hist(new_samples, nbins, range=(0,np.pi), facecolor='red', alpha=0.75, normed=True)
    X=np.linspace(0,np.pi, 50000)
    Y=PDF(X)
    plt.plot(X, Y,'b')
    plt.show()
    
    cumulative_integrate=cumtrapz(Y, X, initial=0)
    print(cumulative_integrate[-1])
    plt.plot(X, cumulative_integrate)
    plt.show()
    
