
import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt
from scipy.integrate import cumtrapz

from constants import *



if __name__=="__main__":


    ##open data
    table_in=array_input( binary_input("./out_arg") )
    
    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()
    
    diff_CS_table=table_in.get_array()
    diff_CS=diff_CS_table.read_doubles()
    
    CS_table=table_in.get_array()
    CS=CS_table.read_doubles()
    
    ##plt.plot(energies, diff_CS, 'b')
    TI=cumtrapz(diff_CS, energies, initial=0)
    plt.plot(energies, TI+0.01, 'r')
    plt.plot(energies, CS, 'g')
    plt.show()
    
    
