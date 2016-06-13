#!/usr/bin/python
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from constants import *
from read_binary import binary_input

class particle_history(object):
    def __init__(self, id, charge, creation_time, init_pos, init_mom):
        self.id=id
        self.charge=charge
        self.creation_time=creation_time
        self.pos_history=[init_pos]
        self.mom_history=[init_mom]
        self.timestep_history=[0]
        self.was_removed=False

    def update(self, timestep, position, momentum):
        self.timestep_history.append(timestep)
        self.pos_history.append(position)
        self.mom_history.append(momentum)

    #~def add_position(self, new_pos):
        #~self.pos_history.append(new_pos)
        #~
    #~def add_momentum(self, new_mom):
        #~self.mom_history.append(new_mom)

    def get_X(self):
        return np.array([p[0] for p in self.pos_history])

    def get_Y(self):
        return np.array([p[1] for p in self.pos_history])

    def get_Z(self):
        return np.array([p[2] for p in self.pos_history])

    def get_Px(self):
        return np.array([p[0] for p in self.mom_history])

    def get_Py(self):
        return np.array([p[1] for p in self.mom_history])

    def get_Pz(self):
        return np.array([p[2] for p in self.mom_history])

    def get_T(self):
        return self.creation_time+np.cumsum(self.timestep_history)

#~def read_text_A(fname):
    #~with open(fname, 'rt') as fin:
        #~data=fin.read().split()
        #~
    #~data_i=0
    #~particles={}
    #~while data_i<len(data):
        #~itter_n=int(data[data_i])
        #~num_particles=int(data[data_i+1])
        #~data_i+=2
        #~for particle_index in xrange(num_particles):
            #~particle_ID=int(data[data_i])
            #~particle_pos=np.zeros(3)
            #~particle_pos[0]=float(data[data_i+1])
            #~particle_pos[1]=float(data[data_i+2])
            #~particle_pos[2]=float(data[data_i+3])
            #~particle_mom=np.zeros(3)
            #~particle_mom[0]=float(data[data_i+4])
            #~particle_mom[1]=float(data[data_i+5])
            #~particle_mom[2]=float(data[data_i+6])
            #~data_i+=7
            #~
            #~if not particle_ID in particles:
                #~particles[particle_ID]=particle_history(particle_ID)
                #~
            #~particles[particle_ID].add_position(particle_pos)
            #~particles[particle_ID].add_momentum(particle_mom)
            #~
    #~return particles

def read_particles(fname):
    fin=binary_input(fname)
    particles={}
    while not fin.at_end():
        command=fin.in_short()
        if command==1: ## a number of new particles
            num_particles=fin.in_int()
            for x in xrange(num_particles):
                ID=fin.in_int()
                charge=fin.in_short()
                creation_time=fin.in_double()
                pos_1=fin.in_double()
                pos_2=fin.in_double()
                pos_3=fin.in_double()
                mom_1=fin.in_double()
                mom_2=fin.in_double()
                mom_3=fin.in_double()
                new_particle=particle_history(ID, charge, creation_time, np.array([pos_1, pos_2, pos_3]), np.array([mom_1, mom_2, mom_3]))
                particles[ID]=new_particle

        elif command==2: ##update existing particles
            num_particles=fin.in_int()
            for x in xrange(num_particles):
                ID=fin.in_int()
                timestep=fin.in_double()
                pos_1=fin.in_double()
                pos_2=fin.in_double()
                pos_3=fin.in_double()
                mom_1=fin.in_double()
                mom_2=fin.in_double()
                mom_3=fin.in_double()
                particles[ID].update(timestep, np.array([pos_1, pos_2, pos_3]), np.array([mom_1, mom_2, mom_3]))

        elif command==3: ##remove existing particles
            num_particles=fin.in_int()
            for x in xrange(num_particles):
                ID=fin.in_int()
                timestep=fin.in_double()
                pos_1=fin.in_double()
                pos_2=fin.in_double()
                pos_3=fin.in_double()
                mom_1=fin.in_double()
                mom_2=fin.in_double()
                mom_3=fin.in_double()
                particles[ID].update(timestep, np.array([pos_1, pos_2, pos_3]), np.array([mom_1, mom_2, mom_3]))
                particles[ID].was_removed=True
    return particles


def plot_particles_XYZ(particles):
    fig = plt.figure()
    fig.suptitle('path of particle in XYZ')
    ax = fig.gca(projection='3d')
    ax.set_xlabel('X axis')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')
    for p in particle_data.itervalues():
        ax.plot(p.get_X()*distance_units, p.get_Y()*distance_units, p.get_Z()*distance_units)
    plt.show()

def plot_particles_XZ(particles):
    fig = plt.figure()
    fig.suptitle('path of particle in XYZ')
    ax = fig.add_subplot(1,1,1)
    ax.set_xlabel('X axis')
    ax.set_ylabel('Z axis')
    for p in particle_data.itervalues():
        ax.plot(p.get_X()*distance_units, p.get_Z()*distance_units)
    plt.show()

def plot_particles_TIMEYZ(particles):
    fig = plt.figure()
    fig.suptitle('path of particle in time and YZ')
    ax = fig.gca(projection='3d')
    ax.set_xlabel('time')
    ax.set_ylabel('Y axis')
    ax.set_zlabel('Z axis')
    for p in particles.itervalues():
        ax.plot(p.get_T()*time_units, p.get_Y()*distance_units, p.get_Z()*distance_units)
    plt.show()

def speed_VS_time(particles):
    for p in particles.itervalues():
        Px=p.get_Px()
        Py=p.get_Py()
        Pz=p.get_Pz()
        P_sq=Px*Px+Py*Py+Pz*Pz
        vel=np.sqrt(P_sq/(1+P_sq))
        plt.plot(p.get_T()*time_units, vel)
    plt.title("velocity vs time")
    plt.show()

def KE_VS_time(particles):
    for p in particles.itervalues():
        Px=p.get_Px()
        Py=p.get_Py()
        Pz=p.get_Pz()
        P_sq=Px*Px+Py*Py+Pz*Pz
        E=np.sqrt(P_sq+1)-1
        plt.plot(p.get_T()*time_units, E*electron_rest_energy/(kilo*elementary_charge), 'o-')
    plt.title("KE(keV) vs time")
    plt.show()

def KE_VS_Z(particles):
    for p in particles.itervalues():
        Px=p.get_Px()
        Py=p.get_Py()
        Pz=p.get_Pz()
        P_sq=Px*Px+Py*Py+Pz*Pz
        E=np.sqrt(P_sq+1)-1
        plt.plot(p.get_Z()*distance_units, E*electron_rest_energy/(kilo*elementary_charge), 'o-')
    plt.title("KE(keV) vs altitude(meters)")
    plt.show()
    ##plt.savefig('KEvsZ.png')

def plot_timesteps(particles):
    fig=plt.figure()
    axis=fig.add_subplot(111)
    for p in particles.itervalues():
        axis.plot(p.get_T()*time_units, np.array(p.timestep_history), 'o-')

    y1, y2=axis.get_ylim()
    x1, x2=axis.get_xlim()
    ax2=axis.twinx()
    ax2.set_ylim(y1*time_units/nano, y2*time_units/nano)
    ax2.set_yticks( np.linspace((y1*time_units/nano), (y2*time_units/nano), 5) )
    ax2.set_ylabel('step size (ns)')
    ax2.set_xlim(x1, x2)
    axis.set_ylabel('step size (dimensionless)')

    plt.title("time step size vs time")
    plt.show()

def timesteps_vs_energy(particles):
    minimum_timestep=6e-7
    lowest_maximum_timestep=5e-6
    largest_maximum_timestep=0.001
    timestep_increase_per_KEV=0.000003

    fig=plt.figure()
    axis=fig.add_subplot(111)
    max_E=0
    for p in particles.itervalues():
        Px=p.get_Px()
        Py=p.get_Py()
        Pz=p.get_Pz()
        P_sq=Px*Px+Py*Py+Pz*Pz
        E=np.sqrt(P_sq+1)-1
        E*=electron_rest_energy/(kilo*elementary_charge)
        max_E=max(np.max(E), max_E)
        axis.plot(E, np.array(p.timestep_history), 'o')

    axis.plot((0,max_E), (minimum_timestep,minimum_timestep))
    ET=np.linspace(0, max_E, 10000)
    max_TS=lowest_maximum_timestep +ET*timestep_increase_per_KEV
    max_TS[max_TS>largest_maximum_timestep]= largest_maximum_timestep
    plt.plot(ET, max_TS)

    axis.set_ylabel('step size (dimensionless)')
    plt.title("time step size vs energy")
    plt.show()



if __name__=='__main__':
    print "A"
    particle_data=read_particles('./output')
    print "B"

    #p_final=particle_data[1].mom_history[-1][2]
    #gamma=np.sqrt(1+np.sum(p_final*p_final))
    #p_conv=p_final*electron_rest_mass/C
    #print 'momentum:', p_conv
    #print 'gamma:', gamma
    #print 'velocity:', p_conv/(gamma*electron_mass)
    #pos_final=particle_data[1].pos_history[-1][2]*distance_units
    #print 'position:', pos_final
    #time=len(particle_data[1].pos_history)*0.1*time_units
    #print 'time:',time
    #print
    #print
    #
    #accel=1.0e-4*elementary_charge/electron_mass
    #final_velocity=accel*time
    #final_position=0.5*final_velocity*time+1
    #print 'classical velocity:', final_velocity
    #print 'classical position:', final_position

    ## print "final pos", particle_data[1].pos_history[-1][2]
    print len(particle_data), "particles"

    #plot_particles_XYZ(particle_data)
    plot_particles_XZ(particle_data)
    #plot_particles_TIMEYZ(particle_data)
    #speed_VS_time(particle_data)
    KE_VS_time(particle_data)
    KE_VS_Z(particle_data)
    #3plot_timesteps(particle_data)
    ##timesteps_vs_energy(particle_data)

