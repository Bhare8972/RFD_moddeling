#!/usr/bin/python
import numpy as np
from matplotlib import pyplot as plt

C=3.0*10**8
electron_mass=9.11e-31
electron_red_comp_wavelength=2.46310*(10.0**-12)/(2*np.pi)
electron_charge=1.6e-19 ##coulombs
ave_z=14.5
Z_13=ave_z**(1.0/3.0)
h_bar=1.0545718e-34
scattering_num=(Z_13*h_bar/(2.0*183.8*electron_red_comp_wavelength))**2

kev_to_joules=1000.0*electron_charge

def KE_fb(beta):
	return (1.0/np.sqrt(1-beta*beta)-1.0)*electron_mass*C*C
	
def beta_fKE(KE):
	gamma=1+KE/(electron_mass*C*C)
	return np.sqrt(1.0-1.0/(gamma*gamma))

def dif_cros_sec(theta, beta ):
    mom=electron_mass*C*beta/np.sqrt(1.0-beta*beta)
    sin_sq=np.sin(theta/2.0)**2
    num=1.0-beta*beta*sin_sq
    X=scattering_num/(mom*mom)
    denom=sin_sq + X
    return X*X*num/(denom**2)
    
angles=[]
with open('./scattering_test.txt','r') as fin:
	for line in fin:
		angles.append(float(line))
		
beta=0.5
X=np.linspace(-np.pi, np.pi, 1000)
n, bins, patches=plt.hist(angles, 50, normed=1, facecolor='green', alpha=0.75)
plt.plot(X, dif_cros_sec(X, beta)*np.max(n), 'r')
plt.show()
