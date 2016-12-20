#!/usr/bin/env python3

import numpy as np
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

import sys
sys.path.append('../')
from constants import *
from read_binary import array_input, binary_input

def EXP(t, R):
    return np.exp(R*t)

if __name__=="__main__":

    
    table_in=array_input( binary_input("./Lehtinen1999_out") )
    hist_edges=table_in.read_doublesArray()
    N_samples=table_in.num_left
    R_values=[]
    for i in range(N_samples):
        values=table_in.read_doublesArray()
        popt, pcov = curve_fit(EXP, hist_edges, values)
        print("run:", i, ":", popt[0], "+/-", np.sqrt(pcov[0])[0])
        R_values.append(popt[0])
        plt.plot(hist_edges, EXP(hist_edges, popt[0]), '-')
        plt.plot(hist_edges, values, 'o')
        
    print()
    print(np.average(R_values), np.std(R_values))
    plt.show()
        
    
