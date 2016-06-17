
import numpy as np
from read_binary import array_input, binary_input
from matplotlib import pyplot as plt
from scipy.integrate import quad

from constants import *

class moller_cross_section(object):

    def __init__(self, energy_=0):
        self.set_energy(energy_)

    def set_energy(self, energy_):
        self.energy=energy_
        momentum_sq=(self.energy+1.0)*(self.energy+1.0)-1
        self.momentum=np.sqrt(momentum_sq)
        self.gamma=np.sqrt(1+momentum_sq)
        self.beta=self.momentum/self.gamma

    def __call__(self, production_energy):
    ##return cross section per tau
        term_1_sqrt=self.energy/(production_energy*(self.energy-production_energy))
        term_2_num=2*(self.gamma*self.gamma+self.gamma)-1
        term_2_denom=production_energy*(self.energy-production_energy)*self.gamma*self.gamma

        return (term_1_sqrt*term_1_sqrt - term_2_num/term_2_denom + 1.0/(self.gamma*self.gamma))/self.beta
        
    def integral_test(self, production_energy):
        term_2_F1=(2*self.gamma-1)/(self.energy*self.gamma*self.gamma);
        term_2_F2=np.log(production_energy/(self.energy-production_energy));
        term_3=self.energy/(production_energy*(self.energy-production_energy));

        return (1.0/self.gamma-term_2_F1*term_2_F2-term_3 )/self.beta;
        

    def integrate(self, min_energy, upper_energy=None):
        if(upper_energy==None):
            upper_energy=self.energy/2.0
        G=quad(self, min_energy, upper_energy)
        return G


if __name__=="__main__":
    
    #~ ###test integral
    #~ energy=1
    #~ /energy_units_kev
    #~ CS=moller_cross_section(energy)
    #~ production_energy=np.linspace(lowest_physical_energy, energy/2.0, 1000)
    #~ analytic_rate=[]
    #~ quadrature_rate=[]
    #~ for pe in production_energy:
        #~ analytic_rate.append( CS.integral_test(pe)-CS.integral_test(lowest_physical_energy) )
        #~ quadrature_rate.append( CS.integrate(lowest_physical_energy, pe)[0] )
    #~ plt.plot(production_energy, analytic_rate, 'g')
    #~ plt.plot(production_energy, quadrature_rate, 'r')
    #~ plt.show()
    #~ exit()
    
    
    energy_indexes=[0,10,20,30,40,50,60,70,80,90,100,120,140,160,180,200,220,240,260,280,300,340,380,399]

    ##open data
    table_in=array_input( binary_input("./moller_tables_output") )
    lowest_sim_energy=5.0/energy_units_kev

    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()
    print len(energies), "energies available"
    
    interactions_table=table_in.get_array()
    interactions_per_tau=interactions_table.read_doubles()
    
    
    for i in xrange(len(energies)):
        if i in energy_indexes: 
            print i,":",energies[i]*energy_units_kev, "keV"
            print "  :",interactions_per_tau[i],"interactions per tau calculated"
            MC=moller_cross_section(energies[i])
            V,E=MC.integrate(lowest_sim_energy)
            print "  :",V,"+/-", E, "interactions per tau gaussian integrated"


    for i in xrange(len(energies)):

        points_table=table_in.get_array()
        points=points_table.read_doubles()

        cumquads_table=table_in.get_array()
        cumquads=cumquads_table.read_doubles()


        if i in energy_indexes:
            print energies[i]*energy_units_kev, "keV", len(cumquads), "points"
            
            MC=moller_cross_section(energies[i])
            tst_quads=[]
            quad_error=[]
            for p in points:
                Q, E=MC.integrate(lowest_sim_energy, p)
                tst_quads.append(Q)
                quad_error.append(E)
            tst_quads=np.array(tst_quads)
            tst_quads/=tst_quads[-1]
            
            plt.plot(quad_error, points*energy_units_kev, 'g-')
            plt.plot(tst_quads, points*energy_units_kev, 'ro')
            plt.plot(cumquads, points*energy_units_kev, 'b-')
            plt.show()

