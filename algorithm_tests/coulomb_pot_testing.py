#!/usr/bin/env python
import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import quad
from scipy.optimize import newton

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
    
def LAPLACE(X, b):
    return np.exp(-np.abs(X)/b)/(2.0*b)
    
def NORMAL(X, s):
    return np.exp(-X*X/(2.0*s*s))/(s*np.sqrt(2*3.1415926))
    
def CAUCHY(X, a):
    return 1.0/(a*3.1415926*(1.0+(X/a)**2))

X=np.linspace(0, np.pi, 100000)

#L_B=3.0
#peak=1.0/(2.0*L_B)
#plt.plot(X, LAPLACE(X, L_B)/LAPLACE(0, L_B), '--', label='laplace')

#N_S=3.0
#Npeak=1.0/(N_S*np.sqrt(2*3.1415926))
#plt.plot(X, NORMAL(X, N_S)/Npeak, '--', label='normal')

#C_A=1.0
#Cpeak=CAUCHY(0, C_A)  
#plt.plot(X, CAUCHY(X, C_A)/Cpeak, '--', label='cauchy')


#Y0=dif_cros_sec(X, 0.01)
#Y1=dif_cros_sec(X, 0.1)
#Y2=dif_cros_sec(X, 0.2)
#Y3=dif_cros_sec(X, 0.4)
#Y4=dif_cros_sec(X, 0.6)
#Y5=dif_cros_sec(X, 0.8)
#Y6=dif_cros_sec(X, 0.99)
#plt.plot(X,Y0, label='0.01')
#plt.plot(X,Y1)
#plt.plot(X,Y2)
#plt.plot(X,Y3)
#plt.plot(X,Y4)
#plt.plot(X,Y5)
#plt.plot(X,Y6, label='0.99')
#plt.yscale('log')
#plt.legend()
#plt.show()

##algorithm to find C_A. 

#beta_test=0.1
#C_A_test=0.088
#n_points=10000
#F=1.01
def test_cauchy(C_A_test, beta_test, num_points, F=1.01, plot=False):
    XT=np.linspace(-np.pi, np.pi, num_points)
    dif_cross=dif_cros_sec(XT, beta_test)
    CT=F*CAUCHY(XT, C_A_test)/CAUCHY(0, C_A_test)
    if plot:
        plt.plot(XT, dif_cross, 'r', label='cross section')
        plt.plot(XT, CT, 'b')
        plt.legend()
        plt.yscale('log')
        plt.show()
    return np.all( CT>=dif_cross )
    
#print test_cauchy(C_A_test, beta_test, n_points, F, plot=True)
#area=quad(dif_cros_sec, -np.pi, np.pi, args=(beta_test,))
#print 'eff: ',area[0]*CAUCHY(0, C_A_test)/F
#print 'KE:', KE_fb(beta_test)/(electron_charge*1000.0), "keV" 

##   beta :  C_A    E
##   0.001    13.0  0.148768497046
##   0.01    0.98   0.715776164813
##   0.1    0.088   0.732183940295
##   0.2    0.043   0.736767109792
##   0.4    0.0201   0.73697617506
##   0.6    0.0117   0.736732327831
##   0.8    0.0066   0.734639622007
##   0.9    0.0043   0.728155681914
##   0.99   0.00125   0.736955545057
##   0.999   0.000392   0.73809959
##   0.9999   0.000122   0.74945896


beta_tested=[0.001, 0.01, 0.1, 0.2, 0.4, 0.6, 0.8, 0.9, 0.99, 0.999, 0.9999]
CA_res=[13, 0.98, 0.088, 0.043, 0.0201, 0.0117, 0.0066, 0.0043, 0.00125, 0.000392, 0.000122]

art_beta_num=np.linspace(0.0005,0.99999, 1000)

plt.plot(beta_tested, CA_res, 'bo-')
plt.yscale('log')
plt.xscale('log')

T_beta=[]
T_C=[]
T_eff=[]
F_beta=[]
F_C=[]
F_eff=[]
for bt in art_beta_num:
	CN=(0.9e-2)*(bt**-1.09)*(1.0-bt)**(0.3)
	works=test_cauchy(CN, bt, num_points=1000, F=1.01, plot=False)
	area=quad(dif_cros_sec, -np.pi, np.pi, args=(bt,))
	eff=area[0]*CAUCHY(0, CN)/1.01
	if works:
		T_beta.append(bt)
		T_C.append(CN)
		T_eff.append(eff)
	else:
		F_beta.append(bt)
		F_C.append(CN)
		F_eff.append(eff)

plt.plot(T_beta, T_C, 'go', label='good points')
plt.plot(F_beta, F_C, 'ro',label='bad points')
plt.legend()
plt.show()

plt.plot( KE_fb(np.array(T_beta))/(1000.0*electron_charge), T_eff, 'go', label='good points')
plt.plot( KE_fb(np.array(F_beta))/(1000.0*electron_charge), F_eff, 'ro',label='bad points')
plt.legend()
plt.show()

#def minimizer(C_A_test, beta_value):
#	A=quad(dif_cros_sec, -np.pi, np.pi, args=(beta_value,))
#	return A[0]*CAUCHY(0, C_A_test)/1.01 -0.73
	
#def get_CA_value(beta_value):
#	return newton(minimizer, 0.05, args=(beta_value,))
	
#beta=0.4
#new_CA=get_CA_value(beta)
#print 'CA:',new_CA
#print 'good:',test_cauchy(new_CA, beta, 100000, plot=True)
#print 'eff:',minimizer(new_CA, beta)+0.73

	

##integrate to find effiecency
#area_1=quad(dif_cros_sec, -np.pi, np.pi, args=(0.01,))
#print '0.01: ',area_1[0]*Cpeak 

#area_1=quad(dif_cros_sec, -np.pi, np.pi, args=(0.1,))
#print '0.1: ',area_1[0]*Cpeak 

#area_2=quad(dif_cros_sec, -np.pi, np.pi, args=(0.2,))
#print '0.2: ',area_2[0]*Cpeak 

#area_4=quad(dif_cros_sec, -np.pi, np.pi, args=(0.4,))
#print '0.4: ',area_4[0]*Cpeak 

#area_6=quad(dif_cros_sec, -np.pi, np.pi, args=(0.6,))
#print '0.6: ',area_6[0]*Cpeak 

#area_8=quad(dif_cros_sec, -np.pi, np.pi, args=(0.8,))
#print '0.8: ',area_8[0]*Cpeak 

#area_99=quad(dif_cros_sec, -np.pi, np.pi, args=(0.99,))
#print '0.99: ',area_99[0]*Cpeak 
