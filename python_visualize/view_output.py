#!/usr/bin/python
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from constants import *
from read_binary import binary_input

class electron_history(object):
    def __init__(self, id, file_in):
        
        charge=file_in.in_short()
        creation_time=file_in.in_double()
        pos_1=file_in.in_double()
        pos_2=file_in.in_double()
        pos_3=file_in.in_double()
        mom_1=file_in.in_double()
        mom_2=file_in.in_double()
        mom_3=file_in.in_double()
        
        self.id=id
        self.charge=charge
        self.creation_time=creation_time
        self.pos_history=[ np.array([pos_1, pos_2, pos_3]) ]
        self.mom_history=[ np.array([mom_1, mom_2, mom_3]) ]
        self.timestep_history=[0]
        self.removal_reason=None
        
        self.X=None
        self.Y=None
        self.Z=None
        self.Px=None
        self.Py=None
        self.Pz=None
        self.T=None
        
    def update(self, file_in):
        timestep=file_in.in_double()
        pos_1=file_in.in_double()
        pos_2=file_in.in_double()
        pos_3=file_in.in_double()
        mom_1=file_in.in_double()
        mom_2=file_in.in_double()
        mom_3=file_in.in_double()
        
        self.timestep_history.append(timestep)
        self.pos_history.append(  np.array([pos_1, pos_2, pos_3])  )
        self.mom_history.append(  np.array([mom_1, mom_2, mom_3]) )
        
    def remove(self, file_in):
        reason_removed=file_in.in_short()
        timestep=file_in.in_double()
        pos_1=file_in.in_double()
        pos_2=file_in.in_double()
        pos_3=file_in.in_double()
        mom_1=file_in.in_double()
        mom_2=file_in.in_double()
        mom_3=file_in.in_double()
        
        self.timestep_history.append(timestep)
        self.pos_history.append(  np.array([pos_1, pos_2, pos_3])  )
        self.mom_history.append(  np.array([mom_1, mom_2, mom_3]) )
        self.removal_reason=reason_removed
        

    def get_X(self):
        if self.X is None:
            self.X=np.array([p[0] for p in self.pos_history])
        return self.X

    def get_Y(self):
        return np.array([p[1] for p in self.pos_history])

    def get_Z(self):
        return np.array([p[2] for p in self.pos_history])

    def get_Px(self):
        if self.Px is None:
            self.Px=np.array([p[0] for p in self.mom_history])
        return self.Px

    def get_Py(self):
        if self.Py is None:
            self.Py=np.array([p[1] for p in self.mom_history])
        return self.Py

    def get_Pz(self):
        if self.Pz is None:
            self.Pz=np.array([p[2] for p in self.mom_history])
        return self.Pz

    def get_T(self):
        if(self.T is None):
            self.T=self.creation_time+np.cumsum(self.timestep_history)
        return self.T

def read_particles(fname):
    fin=binary_input(fname)
    particles={}
    while not fin.at_end():
        command=fin.in_short()
        print(command)
        if command==1: ## a  new particle
            ID=fin.in_int()
            particles[ID]=electron_history(ID, fin)

        elif command==2: ##update existing particle
            ID=fin.in_int()
            particles[ID].update(fin)

        elif command==3: ##remove existing particle
            ID=fin.in_int()
            particles[ID].remove(fin)
        else:
			print("error")
			exit()
            
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

def plot_particles_ZvsTIME(particles):
    fig = plt.figure()
    fig.suptitle('location of particle in Z vs time')
    ax = fig.add_subplot(1,1,1)
    ax.set_xlabel('time (ns)')
    ax.set_ylabel('Z axis (m)')
    creation_Z=[]
    creation_T=[]
    for p in particle_data.itervalues():
        T=p.get_T()*time_units*10**9
        Z=p.get_Z()*distance_units
        ax.plot(T, Z, '.-')
        
        
        creation_Z.append(Z[0])
        creation_T.append(T[0])
        
    plt.plot(creation_T, creation_Z, 'ro')
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
        plt.plot(p.get_T()*time_units*(10**9), E*energy_units_kev, 'o-')
    plt.title("KE(keV) vs time(ns)")
    plt.show()

def KE_VS_Z(particles):
    for p in particles.itervalues():
        Px=p.get_Px()
        Py=p.get_Py()
        Pz=p.get_Pz()
        P_sq=Px*Px+Py*Py+Pz*Pz
        E=np.sqrt(P_sq+1)-1
        plt.plot(p.get_Z()*distance_units, E*energy_units_kev, 'o-')
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

def num_electrons_per_T(electrons, num_seeds=1.0, max_T=None):
    if max_T is None:
        max_T=0
        for e in electrons.itervalues():
            etm=np.max(e.get_T())
            if etm>max_T:
                max_T=etm
                
    time_checks=np.linspace(0, max_T, 1000)
    electrons_each_T=np.zeros(len(time_checks))
    for e in electrons.itervalues():
        start_T=np.min(e.get_T())
        end_T=np.max(e.get_T())
        for i in xrange(len(electrons_each_T)):
            if time_checks[i]>=start_T and time_checks[i]<=end_T:
                electrons_each_T[i]+=1.0
    
    
    plt.plot(time_checks, electrons_each_T/num_seeds)
    plt.show()
    return time_checks, electrons_each_T

def ave_kev_per_T(time_checks, num_electrons, electrons):

    kev_each_T=np.zeros(len(time_checks))
    for e in electrons.itervalues():
        times=e.get_T()
        start_T=np.min(times)
        end_T=np.max(times)
        for i in xrange(len(time_checks)):
            if time_checks[i]>=start_T and time_checks[i]<=end_T:
                t_index=np.searchsorted(times, time_checks[i])-1
                Px=e.get_Px()[t_index]
                Py=e.get_Py()[t_index]
                Pz=e.get_Pz()[t_index]
                P_sq=Px*Px+Py*Py+Pz*Pz
                E=np.sqrt(P_sq+1)-1
                E*=electron_rest_energy/(kilo*elementary_charge)
                
                kev_each_T[i]+=E
    plt.plot(time_checks, kev_each_T/num_electrons)
    plt.show()
    
if __name__=='__main__':
    print( "A" )
    particle_data=read_particles('./output')
    print( "B" )

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
    print( len(particle_data), "particles")

    #plot_particles_XYZ(particle_data)
    #plot_particles_XZ(particle_data)
    #plot_particles_TIMEYZ(particle_data)
    #plot_particles_ZvsTIME(particle_data)
    #speed_VS_time(particle_data)
    #KE_VS_time(particle_data)
    #KE_VS_Z(particle_data)
    #plot_timesteps(particle_data)
    #timesteps_vs_energy(particle_data)
    
    
    time_checks, num_electrons = num_electrons_per_T(particle_data, 100.0)
    ave_kev_per_T(time_checks, num_electrons, particle_data)

