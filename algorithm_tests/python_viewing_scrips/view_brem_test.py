#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
import matplotlib.pyplot as plt


if __name__=="__main__":
    table_in=array_input( binary_input("./brem_test_out") )
    
    theta_table=table_in.get_array()
    energy_space=theta_table.read_doubles()
    
    cs_table=table_in.get_array()
    CS=cs_table.read_doubles()
    
    print CS
    
    plt.plot(energy_space, CS, 'o-')
    plt.show()
    
