////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"

#include "read_tables/ionization_table.hpp"

using namespace gsl;
using namespace std;

bool rnd_seed=true; //seed random number generators with a random seed?  If false, use a built-in seed value (for repeatability)

//inputs
double time_step=0.001; //in units of time_units

//public classes
//shielded_coulomb coulomb_scattering(average_air_atomic_number, rnd_seed);
ionization_table ionization;


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
		vector B=-1.0*B_field->get(position_, time); //-1 is becouse electron has negative charge
		double inverse_gamma=1.0/G;
		force[0]+=inverse_gamma*(momentum_[1]*B[2]-momentum_[2]*B[1]);
		force[1]+=inverse_gamma*(momentum_[2]*B[0]-momentum_[0]*B[2]);
		force[2]+=inverse_gamma*(momentum_[0]*B[1]-momentum_[1]*B[0]);

		//ionization friction
		double friction=ionization.electron_lookup(momentum_squared);
		
		print("mom. sq:", momentum_squared);
		print("table:", friction);
		print("beth:", beth_force(momentum_squared) );

		if(friction>0) //don't want weird stuff
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
        double gamma_=sqrt(gamma_squared);
        double inv_beta_squared=gamma_squared/momentum_squared;

        double term1=log(inv_I_sq*momentum_squared*(gamma_-1.0));
        double term2=(1+(2.0/gamma_)-1.0/gamma_squared)*log(2.0);
        double term3=(1.0-2.0/gamma_+1.0/gamma_squared)/8.0;
        double term4=1.0/gamma_squared;

        if(std::isnan(term1)) return 0.0;

        return inv_beta_squared*(term1 - term2 + term3 + term4);
	}

    double beth_force_minus_moller(double momentum_squared)
	{
        double gamma_squared=1+momentum_squared;
        double G=sqrt(gamma_squared);
        double beta_squared=momentum_squared/gamma_squared;

        double term1=log( 2*inv_I_sq*minimum_energy*momentum_squared );
        double term2_f1=(1+(2.0/G)-1.0/gamma_squared);
        double term2_f2=(G-1.0)/(G-minimum_energy-1.0);
        double term3=minimum_energy/(G-minimum_energy-1.0);
        double term5=minimum_energy*minimum_energy/(2*G*G);

        //don't need to check isnan, becouse these function only runs for energy>2*minimum_energy

        return ( term1 - term2_f1*log(term2_f2) + term3 - beta_squared + term5)/beta_squared;
	}

/*	void scatter()
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
	}*/
};

int main()
{
	int number_itterations=1000;

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
	particle.set_momentum(0,0, KE_to_mom(1000.0*kilo*elementary_charge/electron_rest_energy) );

	//output file
	ofstream fout("output.txt");
	cout<<"start"<<endl;
	//simulate!
	for(int i=0; i<number_itterations; i++)
	{
		particle.kunge_kutta_update(&E_field, &B_field, i*time_step);
		//particle.scatter();

		//save data
		fout<<i<<' '; //itteration number
		fout<<'1'<<' '; //number of particles
		fout<<'1'<<' '; //particle ID
		fout<<particle.position[0]<<' '<<particle.position[1]<<' '<<particle.position[2]<<' '; //position
		fout<<particle.momentum[0]<<' '<<particle.momentum[1]<<' '<<particle.momentum[2]<<' '; //position
		//cout<<"time: "<<i*time_step<<endl;
	}
	fout.close();
}
