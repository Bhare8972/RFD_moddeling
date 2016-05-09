////This code is to reproduce the methods and results of Lehinen 1999, "Monte Carlo Simulation of runaway MeV electron breakdown with application to red sprites and terrestrial gamma-ray flashes"
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"

//#include "read_tables/ionization_table.hpp"
#include "read_tables/diffusion_table.hpp"
#include "read_tables/moller_table.hpp"

#include "physics/bethe_eq.hpp"

#include "particle_history.hpp"

using namespace gsl;
using namespace std;

bool rnd_seed=true; //seed random number generators with a random seed?  If false, use a built-in seed value (for repeatability)

//inputs
double RKF_kappa=0.7; //kappa factor needed in solving system of equations
double RKF_abs_pos_tol=0.00001;
double RKF_abs_mom_tol=0.0002;//the MINIMUM of these next two errors is chosen
double RKF_rel_mom_tol=0.001;
double maximum_timestep=0.1;
double minimum_timestep=0;//not implemented correctly. IF this is hit, then runge-kutta goes into infinite loop

//////// input data tables //////
//ionization_table ionization(true);// remove losses due to moller scattering
diffusion_table SHCdiffusion_scattering(rnd_seed);
moller_table moller_scattering(rnd_seed);

//////// usefull converstion functions //////////  maybe place this elsewhere???
// dimensionless units unless specificied otherwise
double KE_to_mom(double KE)
//both KE and momentum are unitless
{
	return sqrt(pow(1+KE , 2.0) - 1.0);
}

double gamma(gsl::vector &mom_)
{
    return sqrt(1+mom_[0]*mom_[0]+mom_[1]*mom_[1]+mom_[2]*mom_[2]);
}

double gamma(double momentum_squared_)
{
    return sqrt(1+momentum_squared_);
}

/////// tools for representing magnetic and electric fields /////////
class field
{
	public:
	virtual gsl::vector get(gsl::vector &position, double time)=0;//in case we want time dependance later
	virtual gsl::vector get(gsl::vector &position)=0;
	field* pntr(){ return this;}
};

class uniform_field : public field
{
public:
	gsl::vector minimum;
	gsl::vector maximum;
	gsl::vector value;

	uniform_field()
	{
		minimum=gsl::vector({0,0,0});
		maximum=gsl::vector({0,0,0});
		value=gsl::vector({0,0,0});
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

	gsl::vector get(gsl::vector &position)
	{
		if(position.vec_greaterThan(minimum).all_true() and position.vec_lessThan(maximum).all_true())
		{
			return value;
		}
		else
		{
			return gsl::vector({0,0,0});
		}
	}


	gsl::vector get(gsl::vector &position, double time)//just to satisify compiler
	{
		return get(position, 0);
	}
};

///////// particles and particle phyics ////////

class particle_T
{
    static size_t next_ID;

public:
    size_t ID;
    int charge;
    double timestep; //timestep of particle
    double next_timestep; //timestep of particle
    double current_time;
    gsl::vector position; //dimensionless, in units of distance_units
    gsl::vector momentum; //dimensionless, in units of electron-rest-mass/c

    particle_T()
    //some default values
    {
        ID=next_ID;
        next_ID++;
        charge=-1;//electron
		position=gsl::vector({0,0,0});
		momentum=gsl::vector({0,0,0});
        timestep=0.0001;
        next_timestep=0.0001;
        current_time=0;
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

    void scatter_angle(double inclination, double azimuth)
	//scatter the particle by an angle
	{
		double momentum_squared=momentum.sum_of_squares();

		//calculate the three vector magnitudes
		double A=cos(inclination); //basis vector is original momentum
		double B=sin(inclination)*cos(azimuth); //basis vector will be vector Bv below
		double C=sin(inclination)*sin(azimuth); //basis vector will be vector Cv below

		//find vector Bv, perpinduclar to momentum
		gsl::vector init({1,0,0});
		gsl::vector Bv=cross(init, momentum);
		if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
		{
			init=gsl::vector({0,1,0}); //so we try a different init. momentum cannot be parrellel to both inits
			Bv=cross(init, momentum);
		}

		//normalize Bv
		Bv/=sqrt(Bv.sum_of_squares());

		//now we find Cv
		gsl::vector Cv=cross(Bv, momentum); //Bv and momentum are garenteed to be perpindicular.

		//give Bv correct magnitude
		Bv*=sqrt(momentum_squared);

		//find new momentum
		momentum=A*momentum + B*Bv + C*Cv;
	}

	//// some static functions
	static
	particle_T make_electron(double timestep=0.0001)
	{
        particle_T out;
        out.current_time=0;
        out.charge=-1;
        out.timestep=0.0001;
        return out;
	}
};
size_t particle_T::next_ID=0;


class apply_charged_force
{
    public:

    electron_ionization_table electron_table;
    field* E_field; //do not own these two vectors
    field* B_field;
    unsigned int remove_moller; //0 for not remove moller, 1 for constant_min_energy , 2 for variable min energy
    double min_energy;

    //runge kutta variables
    double maximum_timestep;
    double minimum_timestep; //currently always zero. As it is not implemented correctly

    apply_charged_force(double const_min_energy_dimensionless, field* E_field_, field* B_field_) : electron_table(const_min_energy_dimensionless)
    //use this constructor if the minimum_energy is constant
    {
        E_field=E_field_;
        B_field=B_field_;
        remove_moller=1;
        min_energy=const_min_energy_dimensionless;
    }

    apply_charged_force(field* E_field_, field* B_field_, bool do_moller=false) : electron_table()
    //use this constructor if the minimum_energy is variable or not doing moller scattering
    {
        E_field=E_field_;
        B_field=B_field_;
        if(do_moller) //variable min_energy
        {
            remove_moller=2;
        }
        else
        {
            remove_moller=0;
        }
        min_energy=lowest_physical_energy;
    }

    void set_min_energy(double min_energy_)
    //set min energy. Only usefull if min_energy is variable
    {
        min_energy=min_energy_;
    }


    void set_minmax_timestep(double min_timestep_, double max_timestep_)
    {
        maximum_timestep=max_timestep_;
        minimum_timestep=0;//not implemented correctly. IF this is hit, then runge-kutta goes into infinite loop
    }

    I AM HERE. IMPLEMENT ERROR TOL

double RKF_kappa=0.7; //kappa factor needed in solving system of equations
double RKF_abs_pos_tol=0.00001;
double RKF_abs_mom_tol=0.0002;
//double RKF_rel_mom_tol=0.001;

    gsl::vector force(gsl::vector &position, gsl::vector &momentum, int charge)
    //to make this depend on time, add double current_time as paramenter
    {
        //values
        double momentum_squared=momentum.sum_of_squares();
        double momentum_magnitude=sqrt(momentum_squared);
        double G=gamma(momentum_squared);
        double inverse_gamma=1.0/G;

        //electric field
        gsl::vector force=charge*E_field->get(position); //to depend on time, pass current_time to electric field

        //magnetic field
        gsl::vector B=charge*B_field->get(position); //to depend on time, pass current_time to electric field
        force[0]+=inverse_gamma*(momentum[1]*B[2]-momentum[2]*B[1]);
        force[1]+=inverse_gamma*(momentum[2]*B[0]-momentum[0]*B[2]);
        force[2]+=inverse_gamma*(momentum[0]*B[1]-momentum[1]*B[0]);

        //ionization friction
        double friction;
        if(charge==-1)
        {
            if(remove_moller==0 or remove_moller==1)
            {
                friction=electron_table.electron_lookup(momentum_squared);
            }
            else if(remove_moller==2)
            {
                friction=electron_table.electron_lookup(momentum_squared, min_energy);
            }
        }
        else
        {
            ////positrons not implemented
            //friction=ionization.positron_lookup(momentum_squared);
        }

        if(friction>0) //don't want weird stuff
        {
            force[0]-=friction*momentum[0]/momentum_magnitude;
            force[1]-=friction*momentum[1]/momentum_magnitude;
            force[2]-=friction*momentum[2]/momentum_magnitude;
        }

        return force;
    }

    void charged_particle_RungeKuttaFehlberg(particle_T &particle)
    //to make this depend of time, add argument double current_time
    //every time you multiply or add on a vector, it makes a new vector. This can be DRASTICLY improved! make a new multiply and add opperator
    {
        bool acceptable=false;
        while(not acceptable)
        {
            particle.timestep=particle.next_timestep;

            gsl::vector pos_step=particle.position;
            gsl::vector mom_step=particle.momentum;

            gsl::vector K_1_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_1_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //to make this depend on time, pass current_time

            pos_step=particle.position+K_1_pos/5.0;
            mom_step=particle.momentum+K_1_mom/5.0;

            gsl::vector K_2_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_2_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //pass , current_time+time_step/5.0

            pos_step=particle.position+K_1_pos*(3.0/40.0)+K_2_pos*(9.0/40.0);
            mom_step=particle.momentum+K_1_mom*(3.0/40.0)+K_2_mom*(9.0/40.0);

            gsl::vector K_3_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_3_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //pass , current_time+time_step*3/10.0

            pos_step=particle.position+K_1_pos*(3.0/10.0)+K_2_pos*(-9.0/10.0)+K_3_pos*(6.0/5.0);
            mom_step=particle.momentum+K_1_mom*(3.0/10.0)+K_2_mom*(-9.0/10.0)+K_3_mom*(6.0/5.0);

            gsl::vector K_4_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_4_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //, current_time+time_step*3/5

            pos_step=particle.position+K_1_pos*(-11.0/54.0)+K_2_pos*(5.0/2.0)+K_3_pos*(-70.0/27.0)+K_4_pos*(35.0/27.0);
            mom_step=particle.momentum+K_1_mom*(-11.0/54.0)+K_2_mom*(5.0/2.0)+K_3_mom*(-70.0/27.0)+K_4_mom*(35.0/27.0);

            gsl::vector K_5_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_5_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //, current_time+time_step*1

            pos_step=particle.position+K_1_pos*(1631.0/55296.0)+K_2_pos*(175.0/512.0)+K_3_pos*(575.0/13828.0)+K_4_pos*(44275.0/110592.0)+K_5_pos*(253.0/4096.0);
            mom_step=particle.momentum+K_1_mom*(1631.0/55296.0)+K_2_mom*(175.0/512.0)+K_3_mom*(575.0/13828.0)+K_4_mom*(44275.0/110592.0)+K_5_mom*(253.0/4096.0);

            gsl::vector K_6_pos=particle.timestep*mom_step/gamma(mom_step);
            gsl::vector K_6_mom=particle.timestep*force(pos_step, mom_step, particle.charge, E_field, B_field); //, current_time+time_step*7/8

            gsl::vector pos_O4=particle.position+K_1_pos*(37.0/378.0)+K_3_pos*(250.0/621.0)+K_4_pos*(125.0/594.0)+K_5_pos*(512.0/1771.0);
            gsl::vector mom_O4=particle.momentum+K_1_mom*(37.0/378.0)+K_3_mom*(250.0/621.0)+K_4_mom*(125.0/594.0)+K_5_mom*(512.0/1771.0);

            gsl::vector pos_O5=particle.position+K_1_pos*(2825.0/27648.0)+K_3_pos*(18575.0/48384.0)+K_4_pos*(13525.0/55296.0)+K_5_pos*(277.0/14336.0)+K_6_pos*(1.0/4.0);
            gsl::vector mom_O5=particle.momentum+K_1_mom*(2825.0/27648.0)+K_3_mom*(18575.0/48384.0)+K_4_mom*(13525.0/55296.0)+K_5_mom*(277.0/14336.0)+K_6_mom*(1.0/4.0);

            double pos_error_sq=(pos_O4-pos_O5).sum_of_squares() ;
            double mom_error_sq=(mom_O4-mom_O5).sum_of_squares() ;
            //double rel_mom_error_sq=mom_error_sq/ mom_O5.sum_of_squares();
            pos_error_sq=RKF_abs_pos_tol*RKF_abs_pos_tol/pos_error_sq;
            mom_error_sq=RKF_abs_mom_tol*RKF_abs_mom_tol/mom_error_sq;
            //rel_mom_error_sq=RKF_rel_mom_tol*RKF_rel_mom_tol/rel_mom_error_sq;


            double err_f=min(pos_error_sq, mom_error_sq);//note the inverses

            particle.next_timestep=particle.timestep*RKF_kappa*pow( sqrt(err_f), 0.2);
            if(particle.next_timestep>maximum_timestep)
            {
                particle.next_timestep=maximum_timestep;
            }
            else if(particle.next_timestep<minimum_timestep)
            {
                particle.next_timestep=minimum_timestep;
            }

            if(err_f>1)//error is good, exit
            {
                particle.current_time+=particle.timestep;
                particle.position=pos_O5;
                particle.momentum=mom_O5;
                acceptable=true;
            }
            else
            {//repeat with new timestep
                acceptable=false;
            }
        }
    }

}


void shielded_coulomb_scattering(particle_T &particle) //should this be a class?
{
    double I=particle.momentum.sum_of_squares();
    //print(I, (sqrt(I+1)-1)*electron_rest_energy/(Kilo*elementary_charge));
    if((sqrt(I+1)-1)*energy_units_kev>0.02) //just for now
    {
        I=SHCdiffusion_scattering.sample_inclination(I, particle.timestep);
        particle.scatter_angle(I, SHCdiffusion_scattering.sample_azimuth() );
    }
}

void do_moller_scattering(particle_T &electron, list<particle_T> *new_particles) //should this be a class?
{
    double energy_I1=sqrt(1+electron.momentum.sum_of_squares())-1;
    if(energy_I1< moller_scattering.lowest_energy()) return;

    double momentum_I1=sqrt(electron.momentum.sum_of_squares());
    moller_scattering.set_energy(energy_I1);
    size_t num_interactions=moller_scattering.num_interactions(electron.timestep);

    while( (energy_I1> moller_scattering.lowest_energy()) and num_interactions>0)
    {
        //sample
        moller_scattering.set_energy(energy_I1);
        double energy_F2=moller_scattering.sample_new_energy();
	    double azimuth_angle=moller_scattering.sample_azimuth();

        //calculate energy and momentum
	    double energy_F1=energy_I1-energy_F2;
	    double momentum_F1=sqrt((energy_F1+1)*(energy_F1+1)-1);
	    double momentum_F2=sqrt((energy_F2+1)*(energy_F2+1)-1);

        //calculate relavent angles
	    double old_inclination_scatter=acos( ((energy_I1+1)*(energy_F1+1)-(energy_F2+1))/(momentum_I1*momentum_F1) );
	    double new_inclination_scatter=acos( ((energy_I1+1)*(energy_F2+1)-(energy_F1+1))/(momentum_I1*momentum_F2) );

	    //make new electron
	    new_particles->emplace_back();
	    new_particles->back().position=electron.position.clone();
	    new_particles->back().momentum=electron.momentum*(momentum_F2/momentum_I1);
	    new_particles->back().timestep=electron.timestep;
	    new_particles->back().charge=-1;//set_electron
        new_particles->back().current_time=electron.current_time;

	    //adjust energy of old electron
	    electron.momentum*=(momentum_F1/momentum_I1);

	    //scatter both particles
	    electron.scatter_angle(old_inclination_scatter, azimuth_angle);
	    new_particles->back().scatter_angle(new_inclination_scatter, azimuth_angle+3.1415926);

        //update to loop
	    energy_I1=energy_F1;
	    momentum_I1=momentum_F1;
        num_interactions--;
    }
}

void simple_particle_remove(list<particle_T> &current_particles, list<particle_T> &removal_particles, double energy_threshold)
{
    double gamma_sq_threshold=(energy_threshold+1)*(energy_threshold+1); //comparing gamma is faster
    for(auto iter=current_particles.begin(); iter!=current_particles.end(); ++iter)
    {
        double gamma=iter->momentum.sum_of_squares()+1;
        if(gamma<gamma_sq_threshold)
        {
            auto loc=iter--;
            removal_particles.splice(removal_particles.end(), current_particles, loc);
        }
    }
}

template< typename...arg_Ts>
void particle_apply( void(*FUNC)(particle_T&, arg_Ts...),  list<particle_T> &particles, arg_Ts... arguments)
//use this to apply a function that works on one particle to a list of particles
{
    for(particle_T &part :  particles)
    {
        FUNC(part, arguments...);
    }
}

template< typename...arg_Ts>
void particle_apply( list<particle_T> &particles, void(particle_T::*FUNC)( arg_Ts...), arg_Ts... arguments)
//use this to apply a method to a list of particles
{
    for(particle_T &part :  particles)
    {
        (part.*FUNC)(arguments...);
    }
}

int main()
{
	int number_itterations=10000;
    double particle_removal_energy=1/energy_units_kev; //remove particles that have energy less than this

	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -Kilo/distance_units);
	E_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 300/distance_units);
	E_field.set_value(0, 0, -400e3/E_field_units);
	//E_field.set_value(0, 0, 0/E_field_units);

	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -1/distance_units);
	B_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 20*Kilo/distance_units);
	//B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	B_field.set_value(0, 0, 0);

	//initial particle
	list<particle_T> electrons;
	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	electrons.emplace_back();
	electrons.back().set_position(0,0,0);
	electrons.back().set_momentum(0,0, KE_to_mom(7000.0*Kilo*elementary_charge/electron_rest_energy) );

	//output file
    particle_history_out save_data;
    save_data.new_particles(electrons);

	//simulate!
	for(int i=1; i<=number_itterations; i++)
	{
	    //solve equations of motion
	    I AM HERE. FIX SOLVING EQUATIONS OF MOTION
	    particle_apply( charged_particle_RungeKuttaF, electrons , E_field.pntr(), B_field.pntr() );


        list<particle_T> removal_particles;
        simple_particle_remove(electrons, removal_particles, particle_removal_energy);

        //print("B");
	    //diffusion scattering
        //shielded_coulomb_scattering(particle);
	    particle_apply( shielded_coulomb_scattering, electrons);

        //print("C");
        //moller scattering
        list<particle_T> new_electrons;
        particle_apply( do_moller_scattering, electrons, &new_electrons);

        //print("D");
        //remove too-low energy electrons
        simple_particle_remove(electrons, removal_particles, particle_removal_energy);

		//save data
		save_data.update_particles(electrons);
		save_data.new_particles(new_electrons);
        save_data.remove_particles(removal_particles);

		//add particles to simulation
		electrons.splice(electrons.end(),  new_electrons );
	}
}














