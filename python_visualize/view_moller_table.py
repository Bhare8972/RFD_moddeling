
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

    def integrate(self):
        G=quad(self, minimum_energy, self.energy/2)
        return G


if __name__=="__main__":
    energy_index=1

    ##open data
    table_in=array_input( binary_input("../tables/moller") )

    energies_table=table_in.get_array()
    energies=energies_table.read_doubles()

    interactions_table=table_in.get_array()
    interactions_per_tau=interactions_table.read_doubles()

    print len(energies), "energies available"
    for i in xrange(len(energies)):
        print i,":",energies[i]*energy_units_kev, "keV"
        print "  :",interactions_per_tau[i],"interactions per tau calculated"
        MC=moller_cross_section(energies[i])
        V,E=MC.integrate()
        print "  :",V,"+/-", E, "interactions per tau gaussian integrated"


    for i in xrange(len(energies)):
        distribution_table=table_in.get_array()

        points_table=distribution_table.get_array()
        points=points_table.read_doubles()

        ##values_table=table_in.get_array()
        ##values=values_table.read_doubles()

        cumquads_table=distribution_table.get_array()
        cumquads=cumquads_table.read_doubles()


        if i==energy_index:
            print energies[i]*energy_units_kev, "keV", len(cumquads), "points"
            plt.plot(cumquads, points*energy_units_kev)
            plt.show()

