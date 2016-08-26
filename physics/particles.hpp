
#ifndef PARTICLES
#define PARTICLES


#include<string>
#include<list>
#include <cmath>

#include "binary_IO.hpp"

#include "relativistic_formulas.hpp"

class particle_ID_T
{
public:
    static size_t next_ID;
    size_t ID;
};
size_t particle_ID_T::next_ID=0;


class electron_T : public particle_ID_T
{
public:
    int charge;
    double timestep; //timestep of particle
    double energy;
    double next_timestep; //timestep of particle
    double current_time;
    gsl::vector position; //dimensionless, in units of distance_units
    gsl::vector momentum; //dimensionless, in units of electron-rest-mass/c

    electron_T()
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
        energy=0;
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

	void update_energy()
	{
        energy=mom_to_KE(momentum);
	}

    void scatter_angle(double inclination, double azimuth)
	//scatter the particle by an angle. Inclination is degrees from current angle, azimuth is degrees from a vector perpindicular to current travel direction and perpindicular to absolute Y.
	{
		double momentum_squared=momentum.sum_of_squares();

		//calculate the three vector magnitudes
		double A=std::cos(inclination); //basis vector is original momentum
		double B=std::sin(inclination)*cos(azimuth); //basis vector will be vector Bv below
		double C=std::sin(inclination)*sin(azimuth); //basis vector will be vector Cv below

		//find vector Bv, perpinduclar to momentum
		gsl::vector init({0,1,0});
		gsl::vector Bv=cross(init, momentum);
		if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
		{
			init=gsl::vector({0,0,1}); //so we try a different init. momentum cannot be parrellel to both inits
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
};

class photon_T : public particle_ID_T
{
public:
    //stuff
    double energy; //in units of electron mass
    double current_time;
    gsl::vector position; //dimensionless, in units of distance_units
    gsl::vector travel_direction;
    
    photon_T()
    //some default values
    {
        ID=next_ID;
        next_ID++;
        current_time=0;
        energy=0;
        
        position=gsl::vector({0,0,0});
        travel_direction=gsl::vector({0,0,0});
    }
    
    void propagate(double time)
    {
        position+=time*travel_direction;
    }
    
    void scatter_angle(double inclination, double azimuth)
    //rotate direction by an angle. Inclination is degrees from current angle, azimuth is degrees from a vector perpindicular to current travel direction and perpindicular to absolute Y.
    {
        //calculate the three vector magnitudes
        double A=std::cos(inclination); //basis vector is original direction
        double B=std::sin(inclination)*cos(azimuth); //basis vector will be vector Bv below
        double C=std::sin(inclination)*sin(azimuth); //basis vector will be vector Cv below
        
        //find vector Bv, perpinduclar to travel_direction
        gsl::vector init({0,1,0});
        gsl::vector Bv=cross(init, travel_direction);
        if(Bv.sum_of_squares()<0.1) //init and momentum are close to parellel. Which would cause errors below
        {
            init=gsl::vector({0,0,1}); //so we try a different init. momentum cannot be parrellel to both inits
            Bv=cross(init, travel_direction);
        }

        //normalize Bv
        Bv/=sqrt(Bv.sum_of_squares());

        //now we find Cv
        gsl::vector Cv=cross(Bv, travel_direction); //Bv and momentum are garenteed to be perpindicular.

        //find new momentum
        travel_direction=A*travel_direction + B*Bv + C*Cv;
        travel_direction/=std::sqrt(travel_direction.sum_of_squares()); //insure direction is normalized
    }
};


class particle_history_out
{
public:
    binary_output out;
    //update so that only works on one particle at a time. Not on lists

    /*
    commands:
        1 : add electron
            each electron needs
                int32: ID
                int8: charge
                double: creation time
                3 doubles: position
                3 doubles: momentum

        2: update electron
            each electron needs
                int32 ID
                double timestep
                3 doubles: position
                3 doubles: momentum


        3: remove electron
            each electron needs
                int32 ID
                int8 reason_removed:
                double timestep
                3 doubles: position
                3 doubles: momentum
    */

    //reasons to remove particles
    static const int TOO_LOW_ENERGY=0;
    static const int OUT_OF_BOUNDS=1;
    static const int EVOLVED_INTO_HIGHER_LIFEFORM=2;  //don't use this one

    particle_history_out() : out("output")
    {}

    particle_history_out(std::string fname) : out(fname)
    {}

    void new_electron(electron_T *particle)
    {
        out.out_short(1); //command, update electron

        out.out_int(particle->ID);
        out.out_short(particle->charge);

        out.out_double(particle->current_time);

        out.out_double(particle->position[0]);
        out.out_double(particle->position[1]);
        out.out_double(particle->position[2]);

        out.out_double(particle->momentum[0]);
        out.out_double(particle->momentum[1]);
        out.out_double(particle->momentum[2]);
    }

    void update_electron(electron_T *particle)
    {
        out.out_short(2); //command, update electron

        out.out_int(particle->ID);
        out.out_double(particle->timestep);

        out.out_double(particle->position[0]);
        out.out_double(particle->position[1]);
        out.out_double(particle->position[2]);

        out.out_double(particle->momentum[0]);
        out.out_double(particle->momentum[1]);
        out.out_double(particle->momentum[2]);
    }

    void remove_electron(int reason, electron_T *particle)
    {
        out.out_short(3); //command, remove_electron

        out.out_int(particle->ID);
        out.out_short(reason);
        out.out_double(particle->timestep);

        out.out_double(particle->position[0]);
        out.out_double(particle->position[1]);
        out.out_double(particle->position[2]);

        out.out_double(particle->momentum[0]);
        out.out_double(particle->momentum[1]);
        out.out_double(particle->momentum[2]);
    }
};

#endif
