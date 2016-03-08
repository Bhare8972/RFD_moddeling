////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
#include <iostream>
#include <cmath>
#include <fstream>
#include "vector.hpp"
#include "./utils/GSL_utils.h"

using namespace gsl;
using namespace std;

//some units
double giga=1.0e9;
double kilo=1.0e3;
double micro=1.0e-6;
double nano=1.0e-9;

//inputs
double time_step=0.1; //in units of time_units

//give constants
double C=2.99792e8;  //meters per second
double electron_rest_mass=(9.1093835e-31)*C*C;// in joules   //510.998; //in keV
double elementary_charge=1.602e-19; //charge of electron in coulombs

double minimum_energy=2*kilo*elementary_charge/electron_rest_mass; //minimum energy is 2 keV.
double inv_I_sq=electron_rest_mass*electron_rest_mass/(80.5*80.5*elementary_charge*elementary_charge); //parameter necisary for beth formula



//find unit-less constant conversions
double time_units=172.0*nano; // seconds
double distance_units=C*time_units; //fundamental length scale, in  meters
double E_field_units=electron_rest_mass/(elementary_charge*C*time_units); //units of electric field in V/m
double B_field_units=E_field_units/C; //units of magnetic field in T

//various classes
class field
{
	public:
	virtual vector get(vector &position, double time)=0;
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

	vector get(vector &position, double time)
	{
		if(position.vec_greaterThan(minimum).all_true() and position.vec_lessThan(maximum).all_true())
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


	double gamma(vector &mom_)
	{
		return sqrt(1+mom_[0]*mom_[0]+mom_[1]*mom_[1]+mom_[2]*mom_[2]);
	}

	double gamma(double momentum_squared_)
	{
		return sqrt(1+momentum_squared_);
	}

	vector force(vector &position_, vector &momentum_, field* E_field, field* B_field, double time)
	{
	    //values
	    double momentum_squared=momentum_[0]*momentum_[0]+momentum_[1]*momentum_[1]+momentum_[2]*momentum_[2];
	    double momentum_magnitude=sqrt(momentum_squared);
	    double G=gamma(momentum_squared);

	    //electric field
		vector force=-1.0*E_field->get(position_, time); //-1 is becouse electron has negative charge

		//magnetic field
		vector B=-1*B_field->get(position_, time); //-1 is becouse electron has negative charge
		double inverse_gamma=1.0/G;
		force[0]+=inverse_gamma*(momentum_[1]*B[2]-momentum_[2]*B[1]);
		force[1]+=inverse_gamma*(momentum_[2]*B[0]-momentum_[0]*B[2]);
		force[2]+=inverse_gamma*(momentum_[0]*B[1]-momentum_[1]*B[0]);

		//ionization friction
		double friction=0;

		if( false)//G >= 2*minimum_energy )
		{
		    friction=beth_force_minus_moller(momentum_squared);
		}
		else
		{
		    friction=beth_force(momentum_squared);
		}
		if(friction>0) //friction is 0 IFF momentum_magnitude==0, and we don't want a divide by zero error
		{
            force[0]-=friction*momentum_[0]/momentum_magnitude;
            force[1]-=friction*momentum_[1]/momentum_magnitude;
            force[2]-=friction*momentum_[2]/momentum_magnitude;
		}


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

	double beth_force(double momentum_squared)
	{
        double gamma_squared=1+momentum_squared;
        double G=sqrt(gamma_squared);
        double inv_beta_squared=gamma_squared/momentum_squared;

        if(std::isnan(log(momentum_squared))) return 0.0;

        double exp_term1=inv_I_sq*momentum_squared*G;
        double term2=(1+(2/G)-1.0/gamma_squared)*log(2.0);
        double term3=(G-2.0-1.0/G)/8.0;
        double term4=1/gamma_squared;
        return inv_beta_squared*(log(exp_term1) + term2 + term3 + term4);
	}

    double beth_force_minus_moller(double momentum_squared)
	{
        double gamma_squared=1+momentum_squared;
        double G=sqrt(gamma_squared);
        double beta_squared=momentum_squared/gamma_squared;

        double exp_term1=2*inv_I_sq*minimum_energy*momentum_squared;
        double term3=G/(G-minimum_energy);
        double term2=(1+(2/G)-1.0/gamma_squared)*log(term3);
        double term5=minimum_energy*minimum_energy/(2*(1.0-G));
        return (log(exp_term1) + term2 + term3 + beta_squared + term5)/beta_squared;
	}
};

int main()
{
	int number_itterations=10000;

	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	E_field.set_maximum(kilo/distance_units, kilo/distance_units, 10*kilo/distance_units);
	//E_field.set_value(0, 0, -1.0e5/E_field_units);
	E_field.set_value(0, 0, -1.0e1/E_field_units);

	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	B_field.set_maximum(kilo/distance_units, kilo/distance_units, 20*kilo/distance_units);
	//B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	B_field.set_value(0, 0, 0);

	//initial particle
	electron particle;
	particle.set_position(0,0,0);
	particle.set_momentum(0,0,0);


    auto energy_test=gsl_utils::linspace(0.0, 5.0, 1000); //still in units of electron rest energy
    ofstream fout2("beth_test.txt");
    for(int i=0; i<1000; i++)
    {
        double momentum_squared=energy_test[i]*energy_test[i]-1.0;
        double beth_friction=particle.beth_force(momentum_squared);
        fout2<<(energy_test[i])<<' '<<beth_friction<<endl;
    }

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
