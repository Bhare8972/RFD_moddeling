#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt

if __name__=='__main__':
    table_in=array_input( binary_input("./tst") )
    
    points_table=table_in.get_array()
    points=points_table.read_doubles()
	
    values_table=table_in.get_array()
    values=values_table.read_doubles()
	
    cum_quads_table=table_in.get_array()
    cum_quads=cum_quads_table.read_doubles()
    
    print points
