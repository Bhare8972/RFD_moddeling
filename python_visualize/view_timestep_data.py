#!/usr/bin/env python3

import numpy as np
from matplotlib import pyplot as plt
from scipy.optimize import curve_fit

import sys
from constants import *
from read_binary import array_input, binary_input

if __name__=="__main__":
    table_in=array_input( binary_input("./timestep_halving_hist") )
    energies=table_in.read_doublesArray()
    TS=table_in.read_doublesArray()
    halvings=table_in.read_doublesArray()
    
    N_bins=100
    lowest_bin=3.5
    highest_bin=4000
    
    bin_edges=np.logspace(np.log10(lowest_bin), np.log10(highest_bin), N_bins+1)
    
    TS_bins=[ [] for x in range(N_bins) ]
    halvings_bins=[ [] for x in range(N_bins) ]
    
    
    for ei in range(len(energies)):
        E=energies[ei]*energy_units_kev
        T=TS[ei]
        H=halvings[ei]
        
        bin_i=np.searchsorted(bin_edges, E)
        if bin_i>=0 and bin_i<N_bins:
            TS_bins[bin_i].append(T)
            halvings_bins[bin_i].append(H)
            
    ave_TS=[np.average(b) for b in TS_bins]
    ave_halvings=[ np.average(b) for b in halvings_bins]
    TS_std=[np.std(b) for b in TS_bins]
    halving_std=[np.std(b) for b in halvings_bins]
    
    fig, ax1 = plt.subplots()
    
    ax1.errorbar( (bin_edges[1:]+bin_edges[:-1])*0.5, ave_TS, fmt='b', yerr=TS_std)
    ax1.set_xscale("log", nonposx='clip')
    
    ax2 = ax1.twinx()
    ax2.errorbar( (bin_edges[1:]+bin_edges[:-1])*0.5, ave_halvings, fmt='r', yerr=halving_std)
    ax2.set_xscale("log", nonposx='clip')
    plt.show()
    
