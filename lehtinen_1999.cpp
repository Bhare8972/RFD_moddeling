////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "SC_elastic_scatt.hpp"

using namespace gsl;
using namespace std;

//some units
double giga=1.0e9;
double kilo=1.0e3;
double micro=1.0e-6;
double nano=1.0e-9;

//inputs
double time_step=0.0001; //in units of time_units

//give constants
double C=2.99792e8;  //meters per second
double electron_rest_mass=(9.1093835e-31)*C*C;// in joules   //510.998; //in keV
double elementary_charge=1.602e-19; //charge of electron in coulombs
double average_air_atomic_number=14.5;

double minimum_energy=2*kilo*elementary_charge/electron_rest_mass; //minimum energy is 2 keV.
double inv_I_sq=electron_rest_mass*electron_rest_mass/(85.7*80.5*elementary_charge*elementary_charge); //parameter necisary for beth formula



//find unit-less constant conversions
double time_units=172.0*nano; // seconds
double distance_units=C*time_units; //fundamental length scale, in  meters
double E_field_units=electron_rest_mass/(elementary_charge*C*time_units); //units of electric field in V/m
double B_field_units=E_field_units/C; //units of magnetic field in T



//public classes
shielded_coulomb coulomb_scattering(average_air_atomic_number);


//conversion_functions
double KE_to_mom(double KE)
//both KE and momentum are unitless
{
	return sqrt(pow(1+KE , 2.0) - 1.0);
}

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

		//HERE FIX FRICTION
		if( false)//G >= 2*minimum_energy )
		{
		    friction=beth_force_minus_moller(momentum_squared);
		}
		else
		{
		    friction=beth_force(momentum_squared);
		}
		if(friction>0) //don't want weird stuff
		{
			//cout<<time<<": "<<friction<<" "<<force[2]<<endl;
			//cout<<momentum_[2]<<endl;
            force[0]-=friction*momentum_[0]/momentum_magnitude;
            force[1]-=friction*momentum_[1]/momentum_magnitude;
            force[2]-=friction*momentum_[2]/momentum_magnitude;
			//cout<<time<<": "<<force[2]<<endl;
			//cout<<endl;
		}
		else
		{
			friction=0.0;
			//cout<<time<<": NF "<<force[2]<<endl;
			//cout<<endl;
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
        double gamma_=sqrt(gamma_squared);
        double inv_beta_squared=gamma_squared/momentum_squared;

        double term1=log(inv_I_sq*momentum_squared*(gamma_-1.0));
        double term2=(1+(2.0/gamma_)-1.0/gamma_squared)*log(2.0);
        double term3=(1.0-2.0/gamma_+1.0/gamma_)/8.0;
        double term4=1.0/gamma_squared;
        
        if(std::isnan(term1)) return 0.0;
        
        return inv_beta_squared*(term1 - term2 + term3 + term4);
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
	
	void scatter()
	{
		double momentum_squared=momentum.sum_of_squares();
		////change angle of mementum according to shielded coulomb scattering
		double inclination=coulomb_scattering.sample_inclination(momentum_squared);
		double azimuth=coulomb_scattering.sample_azimuth();
		
		//calculate the three vector magnitudes
		double A=cos(inclination); //basis vector is original momentum
		double B=sin(inclination)*cos(azimuth); //basis vector will be vector Bv below
		double C=sin(inclination)*sin(azimuth); //basis vector will be vector Cv below
		
		//find vector Bv, perpinduclar to momentum
		vector init({1,0,0});
		vector Bv=cross(init, momentum);
		if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
		{
			init=vector({0,1,0}); //so we try a different init. momentum cannot be parrellel to both inits
			Bv=cross(init, momentum);
		}
		
		//normalize Bv
		Bv/=sqrt(Bv.sum_of_squares());
		
		//now we find Cv
		vector Cv=cross(Bv, momentum); //Bv and momentum are garenteed to be perpindicular.
		
		//give Bv correct magnitude
		Bv*=sqrt(momentum_squared);
		
		//find new momentum
		momentum=A*momentum + B*Bv + C*Cv;
	}
};

int main()
{
	int number_itterations=100;

	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	E_field.set_maximum(kilo/distance_units, kilo/distance_units, 10*kilo/distance_units);
	//E_field.set_value(0, 0, -1.7e5/E_field_units);
	E_field.set_value(0, 0, 0/E_field_units);

	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-kilo/distance_units, -kilo/distance_units, -1/distance_units);
	B_field.set_maximum(kilo/distance_units, kilo/distance_units, 20*kilo/distance_units);
	//B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	B_field.set_value(0, 0, 0);

	//initial particle
	electron particle;
	particle.set_position(0,0,0);
	particle.set_momentum(0,0, KE_to_mom(1000.0*kilo*elementary_charge/electron_rest_mass) );
	
    /*
    double KE_test_j=2000.0*1000.0*elementary_charge;
	double mom=KE_to_mom(KE_test_j/electron_rest_mass);
	double F=particle.beth_force(mom*mom);
	cout<<(F*electron_rest_mass/(time_units*C))<<endl;
    
    return 1.0;*/

	//output file
	ofstream fout("output.txt");
	cout<<"start"<<endl;
	//simulate!
	for(int i=0; i<number_itterations; i++)
	{
		particle.kunge_kutta_update(&E_field, &B_field, i*time_step);
		particle.scatter();

		//save data
		fout<<i<<' '; //itteration number
		fout<<'1'<<' '; //number of particles
		fout<<'1'<<' '; //particle ID
		fout<<particle.position[0]<<' '<<particle.position[1]<<' '<<particle.position[2]<<' '; //position
		fout<<particle.momentum[0]<<' '<<particle.momentum[1]<<' '<<particle.momentum[2]<<' '; //position
		cout<<"time: "<<i*time_step<<endl;
	}
	fout.close();
}
