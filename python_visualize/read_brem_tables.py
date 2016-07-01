#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import quad

from read_binary import array_input, binary_input
from constants import *

if __name__=="__main__":
    table_in=array_input( binary_input("./test_outV72") )
    
    energiesTST_table=table_in.get_array()
    TST_energies=energiesTST_table.read_doubles()
    
    rateTST_table=table_in.get_array()
    TST_rate=rateTST_table.read_doubles()
    
    print len(TST_energies)
    #plt.plot(TST_energies*energy_units_kev, TST_rate, 'o-')
    #plt.plot(TST_rate*energy_units_kev, TST_energies, 'o-')
    plt.plot(TST_rate, TST_energies, 'o-')
    plt.show()
