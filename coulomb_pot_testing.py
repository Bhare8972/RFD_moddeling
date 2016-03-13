#!/usr/bin/env python
import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import quad

C=3.0*10**8
electron_mass=9.11e-31
electron_red_comp_wavelength=2.46310*(10.0**-12)/(2*np.pi)
ave_z=14.5
Z_13=ave_z**(1.0/3.0)
h_bar=1.0545718e-34
scattering_num=(Z_13*h_bar/(2*183.8*electron_red_comp_wavelength))**2

def dif_cros_sec(theta, beta ):
    mom=electron_mass*C*beta/np.sqrt(1-beta*beta)
    sin_sq=np.sin(theta/2.0)**2
    num=1.0-beta*sin_sq
    X=scattering_num/(mom*mom)
    denom=sin_sq + X
    return X*X*num/(denom**2)
    
def LAPLACE(X, b):
    return np.exp(-np.abs(X)/b)/(2.0*b)
    
def NORMAL(X, s):
    return np.exp(-X*X/(2.0*s*s))/(s*np.sqrt(2*3.1415926))
    
def CAUCHY(X, a):
    return 1.0/(a*3.1415926*(1.0+(X/a)**2))

X=np.linspace(-np.pi, np.pi, 100000)

#L_B=0.3
#peak=1.0/(2.0*L_B)
#plt.plot(X, LAPLACE(X, L_B), label='laplace')

#N_S=0.68
#Npeak=1.0/(N_S*np.sqrt(2*3.1415926))
#plt.plot(X, NORMAL(X, N_S)/Npeak, label='normal')

C_A=0.092
Cpeak=CAUCHY(0, C_A)  
plt.plot(X, CAUCHY(X, C_A)/Cpeak, label='cauchy')


Y0=dif_cros_sec(X, 0.01)
Y1=dif_cros_sec(X, 0.1)
Y2=dif_cros_sec(X, 0.2)
Y3=dif_cros_sec(X, 0.4)
Y4=dif_cros_sec(X, 0.6)
Y5=dif_cros_sec(X, 0.8)
Y6=dif_cros_sec(X, 0.99)
plt.plot(X,Y0)
plt.plot(X,Y1)
plt.plot(X,Y2)
plt.plot(X,Y3)
plt.plot(X,Y4)
plt.plot(X,Y5)
plt.plot(X,Y6)
#plt.yscale('log')
plt.legend()
plt.show()

##algorithm to find C_A. 
C_A_test=0.092
beta_test=0.1
def test_cauchy(C_A_test, beta_test, num_points, F=1.01, plot=False):
    XT=np.linspace(-np.pi, np.pi, num_points)
    dif_cross=dif_cros_sec(XT, beta_test)
    CT=F*CAUCHY(XT, C_A_test)/CAUCHY(0, C_A_test)
    if plot:
        plt.plot(XT, dif_cross, 'r')
        plt.plot(XT, CT, 'b')
        plt.yscale('log')
        plt.show()
    return np.all( CT>=dif_cross )
    
print test_cauchy(0.09, 0.09, 100000, plot=True)

##integrate to find effiecency
area_1=quad(dif_cros_sec, -np.pi, np.pi, args=(0.01,))
print '0.01: ',area_1[0]*Cpeak 

area_1=quad(dif_cros_sec, -np.pi, np.pi, args=(0.1,))
print '0.1: ',area_1[0]*Cpeak 

area_2=quad(dif_cros_sec, -np.pi, np.pi, args=(0.2,))
print '0.2: ',area_2[0]*Cpeak 

area_4=quad(dif_cros_sec, -np.pi, np.pi, args=(0.4,))
print '0.4: ',area_4[0]*Cpeak 

area_6=quad(dif_cros_sec, -np.pi, np.pi, args=(0.6,))
print '0.6: ',area_6[0]*Cpeak 

area_8=quad(dif_cros_sec, -np.pi, np.pi, args=(0.8,))
print '0.8: ',area_8[0]*Cpeak 

area_99=quad(dif_cros_sec, -np.pi, np.pi, args=(0.99,))
print '0.99: ',area_99[0]*Cpeak 
