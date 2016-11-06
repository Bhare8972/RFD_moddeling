#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt

from constants import *

if __name__=="__main__":
    table_in=array_input( binary_input("./Lehtinen1999_out") )
    
    hist_edges=table_in.read_doublesArray()
    percent_num_particles=table_in.read_doublesArray()
    
    plt.plot(hist_edges[:-1], percent_num_particles)
    plt.show()
