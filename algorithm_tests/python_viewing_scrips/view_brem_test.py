#!/usr/bin/env python3

import numpy as np
from read_binary import array_input, binary_input
import matplotlib.pyplot as plt
from scipy.integrate import simps, cumtrapz

if __name__=="__main__":
    matchings=[0,5,4,3,7,6,2,9,8,1]
    table_in=array_input( binary_input("./brem_PE_test") )
    
    
    PE_space=[]
    PE_CS=[]
    for i in xrange( (table_in.size-2)/2 ):
        
        PE_space.append( table_in.read_doublesArray() )
        PE_CS.append( table_in.read_doublesArray() )
        
        plt.plot(PE_space[-1], PE_CS[-1])
    
    plt.show()
    
    CS_space=table_in.read_doublesArray() 
    CS_samples=table_in.read_doublesArray() 

        
    for i in xrange(0,len(matchings)-1):
        plt.plot(PE_space[matchings[i]], PE_CS[matchings[i]])
        plt.plot(PE_space[matchings[i+1]], PE_CS[matchings[i+1]])
        plt.show()
        
    
    
