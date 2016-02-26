////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
#include <iostream>
#include <cmath>
#include <fstream>
#include "vector.hpp"

using namespace gsl;
using namespace std;

//inputs
double time_step=0.1; //in units of time_units

//give constants
double C=2.99792e8;  //meters per second
double electron_rest_mass=(9.1093835e-31)*C*C;// in joules   //510.998; //in keV
double elementary_charge=1.602e-19; //charge of electron in coulombs

//some units
double giga=1.0e9;
double kilo=1.0e3;
double micro=1.0e-6;
double nano=1.0e-9;

//find unit-less constant conversions
double time_units=172.0*nano; // seconds
double distance_units=C*time_units; //fundamental length scale, in  meters
double E_field_units=electron_rest_mass/(elementary_charge*C*time_units); //units of electric field in V/m
double B_field_units=E_field_units/C; //units of magnetic field in T

//various classes
class field
{
	public:
	virtual vector get(vector position, double time)=0;
};

class uniform_field : public field
{
public:
	vector minimum;
	vector maximum;
	vector value;
	
	uniform_field()
	{
		minimum=vector({0,0,0});
		maximum=vector({0,0,0});
		value=vector({0,0,0});
	}
	
	void set_minimum(double X, double Y, double Z)
	{
		minimum[0]=X;
		minimum[1]=Y;
		minimum[2]=Z;
	}
	
	void set_maximum(double X, double Y, double Z)
	{
		maximum[0]=X;
		maximum[1]=Y;
		maximum[2]=Z;
	}
	
	void set_value(double X, double Y, double Z)
	{
		value[0]=X;
		value[1]=Y;
		value[2]=Z;
	}
	
	vector get(vector position, double time)
	{
		if(position>minimum and position<maximum)
		{
			return value;
		}
		else
		{
			return vector({0,0,0});
		}
	}
	
};

class electron
{
public:
    vector position; //dimensionless, in units of distance_units
    vector momentum; //dimensionless, in units of electron-rest-mass/c
    
    electron()
    {
		position=vector({0,0,0});
		momentum=vector({0,0,0});
	}
	
	void set_position(double x, double y, double z)
	{
		position[0]=x;
		position[1]=y;
		position[2]=z;
	}
	
	void set_momentum(double x, double y, double z)
	{
		momentum[0]=x;
		momentum[1]=y;
		momentum[2]=z;
	}
	
	double gamma()
	{
		return sqrt(1+momentum[0]*momentum[0]+momentum[1]*momentum[1]+momentum[2]*momentum[2]);
	}
	
	
	double gamma(vector mom_)
	{
		return sqrt(1+mom_[0]*mom_[0]+mom_[1]*mom_[1]+mom_[2]*mom_[2]);
	}
	
	vector force(vector position, vector momentum, field* E_field, field* B_field, double time)
	{
		vector force=-1.0*E_field->get(position, time); //-1 is becouse electron has negative charge
		vector B=-1*B_field->get(position, time); //-1 is becouse electron has negative charge
		double inverse_gamma=1.0/gamma();
		force[0]+=inverse_gamma*(momentum[1]*B[2]-momentum[2]*B[1]);
		force[1]+=inverse_gamma*(momentum[2]*B[0]-momentum[0]*B[2]);
		force[2]+=inverse_gamma*(momentum[0]*B[1]-momentum[1]*B[0]);
		return force;
	}
	
	void kunge_kutta_update(field* E_field, field* B_field, double current_time)
	{
		vector pos_step=position;
		vector mom_step=momentum;
		
		vector K_1_pos=mom_step/gamma(mom_step);
		vector K_1_mom=force(pos_step, mom_step, E_field, B_field, current_time);
		
		pos_step=position+time_step*K_1_pos/2.0;
		mom_step=momentum+time_step*K_1_mom/2.0;
		
		vector K_2_pos=mom_step/gamma(mom_step);
		vector K_2_mom=force(pos_step, mom_step, E_field, B_field, current_time+time_step/2.0);
		
		pos_step=position+time_step*K_2_pos/2.0;
		mom_step=momentum+time_step*K_2_mom/2.0;
		
		vector K_3_pos=mom_step/gamma(mom_step);
		vector K_3_mom=force(pos_step, mom_step, E_field, B_field, current_time+time_step/2.0);
		
		pos_step=position+time_step*K_3_pos;
		mom_step=momentum+time_step*K_3_mom;
		
		vector K_4_pos=mom_step/gamma(mom_step);
		vector K_4_mom=force(pos_step, mom_step, E_field, B_field, current_time+time_step);
		
		position+=time_step*(K_1_pos + 2.0*K_2_pos + 2.0*K_3_pos + K_4_pos)/6.0;
		momentum+=time_step*(K_1_mom + 2.0*K_2_mom + 2.0*K_3_mom + K_4_mom)/6.0;
	}
};

int main()
{
	int number_itterations=5000;
	
	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	E_field.set_maximum(kilo/distance_units, kilo/distance_units, 20*kilo/distance_units);
	E_field.set_value(0, 0, -1.0e5/E_field_units);
	
	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	B_field.set_maximum(kilo/distance_units, kilo/distance_units, 20*kilo/distance_units);
	B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	
	//initial particle
	electron particle;
	particle.set_position(0,0,0);
	particle.set_momentum(0,0,0);
	
	//output file
	ofstream fout("output.txt");
	
	//simulate!
	for(int i=0; i<number_itterations; i++)
	{
		particle.kunge_kutta_update(&E_field, &B_field, i*time_step);
		
		//save data
		fout<<i<<' '; //itteration number
		fout<<'1'<<' '; //number of particles
		fout<<'1'<<' '; //particle ID
		fout<<particle.position[0]<<' '<<particle.position[1]<<' '<<particle.position[2]<<' '; //position
		fout<<particle.momentum[0]<<' '<<particle.momentum[1]<<' '<<particle.momentum[2]<<' '; //position
	}
	fout.close();
}
