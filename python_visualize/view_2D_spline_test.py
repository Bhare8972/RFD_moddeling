#!/usr/bin/env python

import numpy as np
from read_binary import array_input, binary_input
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib import pyplot as plt


if __name__=='__main__':
    
    table_in=array_input( binary_input("./2D_tst") )
    
    X_Table=table_in.get_array()
    X=X_Table.read_doubles()
    
    Y_Table=table_in.get_array()
    Y=Y_Table.read_doubles()
    
    SV_Table=table_in.get_array()
    spline_values=SV_Table.read_doubles()
    
    spline_values=spline_values.reshape(len(X), len(Y))
    X,Y=np.meshgrid(X,Y)
    
    print spline_values[0,0]
    
    plt.pcolor(X, Y, spline_values)
    plt.show()
    
    P=[]
    #P.append( [ 0 , 0 , 0 ]) 
    #P.append( [ 6.28319 , 0 , -3.21539e-07 ]) 
    #P.append( [ 6.28319 , 6.28319 , -3.21539e-07 ]) 
    #P.append( [ 0 , 6.28319 , 0 ]) 
    #P.append( [ 3.14159 , 3.14159 , 1.60769e-07 ])
    
    PX=[A[0] for A in P]
    PY=[A[1] for A in P]
    PZ=[A[2] for A in P]
    
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ##surf = ax.plot_surface(X, Y, spline_values, rstride=1, cstride=1, linewidth=0)
    ax.plot_surface(X, Y, spline_values, linewidth=0)
    ax.plot(PX,PY,PZ,'ro')
        
    plt.show()
    
    
    
    
    
