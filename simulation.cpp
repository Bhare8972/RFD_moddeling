
#include <iostream>
#include <cmath>
#include <fstream>

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"
#include "rand.hpp"
#include "time_tree.hpp"

#include "read_tables/diffusion_table.hpp"

#include "physics/relativistic_equations.hpp"
#include "physics/particles.hpp"
#include "physics/bethe_eq.hpp"
#include "physics/moller_scattering.hpp"
#include "physics/interaction_chooser.hpp"

using namespace gsl;
using namespace std;



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

    double kappa;
    double pos_tol;
    double mom_tol;

    apply_charged_force(double const_min_energy_dimensionless, field* E_field_, field* B_field_) : electron_table(const_min_energy_dimensionless, true)
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


    void set_max_timestep(double max_timestep_)
    {
        maximum_timestep=max_timestep_;
    }

    void set_errorTol(double pos_tol_, double mom_tol_, double kappa_=0.5)
    {
        pos_tol=pos_tol_;
        mom_tol=mom_tol_;
        kappa=kappa_;
    }

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
        double friction=-1;
        if(charge==-1)
        {
            if(remove_moller==0 or remove_moller==1) //if not removing moller losses, or constant min_energy
            {
                friction=electron_table.electron_lookup(momentum_squared);
            }
            else if(remove_moller==2) //variable min energy
            {
                friction=electron_table.electron_lookup_variable_RML(momentum_squared, min_energy);
            }
        }
        else
        {
            throw gen_exception("positrons not implemented");
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

    void charged_particle_RungeKuttaCK(particle_T *particle)
    //to make this depend of time, add argument double current_time
    //am using cash-karp parameters for fehlberg method
    //returns a status. 0 means good. 1 means electron went below lowest energy
    {
        bool acceptable=false;
        //print("run:", particle.next_timestep);//(sqrt(particle.momentum.sum_of_squares()+1)-1)*510);
        while(not acceptable)
        {
            particle->timestep=particle->next_timestep;
            if(particle->timestep>maximum_timestep)
            {
                particle->timestep=maximum_timestep;
            }

            gsl::vector pos_step=particle->position;
            gsl::vector mom_step=particle->momentum;

            gsl::vector K_1_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_1_mom=force(pos_step, mom_step, particle->charge);
            K_1_mom*=particle->timestep;


            pos_step=K_1_pos/5.0;
            mom_step=K_1_mom/5.0;

            pos_step+=particle->position;
            mom_step+=particle->momentum;

            gsl::vector K_2_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_2_mom=force(pos_step, mom_step, particle->charge);
            K_2_mom*=particle->timestep;





            pos_step=K_1_pos*(3.0/40.0);
            mom_step=K_1_mom*(3.0/40.0);

            pos_step.mult_add( K_2_pos, (9.0/40.0) );
            mom_step.mult_add( K_2_mom, (9.0/40.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;

            gsl::vector K_3_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_3_mom=force(pos_step, mom_step, particle->charge);
            K_3_mom*=particle->timestep;




            pos_step=K_1_pos*(3.0/10.0);
            mom_step=K_1_mom*(3.0/10.0);

            pos_step.mult_add( K_2_pos, (-9.0/10.0) );
            mom_step.mult_add( K_2_mom, (-9.0/10.0) );

            pos_step.mult_add( K_3_pos, (-6.0/5.0) );
            mom_step.mult_add( K_3_mom, (-6.0/5.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;

            gsl::vector K_4_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_4_mom=force(pos_step, mom_step, particle->charge);
            K_4_mom*=particle->timestep;




            pos_step=K_1_pos*(-11.0/54.0);
            mom_step=K_1_mom*(-11.0/54.0);

            pos_step.mult_add( K_2_pos, (5.0/2.0) );
            mom_step.mult_add( K_2_mom, (5.0/2.0) );

            pos_step.mult_add( K_3_pos, (-70.0/27.0) );
            mom_step.mult_add( K_3_mom, (-70.0/27.0) );

            pos_step.mult_add( K_4_pos, (35.0/27.0) );
            mom_step.mult_add( K_4_mom, (35.0/27.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;

            gsl::vector K_5_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_5_mom=force(pos_step, mom_step, particle->charge);
            K_5_mom*=particle->timestep;





            pos_step=K_1_pos*(1631.0/55296.0);
            mom_step=K_1_mom*(1631.0/55296.0);

            pos_step.mult_add( K_2_pos, (175.0/512.0) );
            mom_step.mult_add( K_2_mom, (175.0/512.0) );

            pos_step.mult_add( K_3_pos, (575.0/13828.0) );
            mom_step.mult_add( K_3_mom, (575.0/13828.0) );

            pos_step.mult_add( K_4_pos, (44275.0/110592.0) );
            mom_step.mult_add( K_4_mom, (44275.0/110592.0) );

            pos_step.mult_add( K_5_pos, (253.0/4096.0) );
            mom_step.mult_add( K_5_mom, (253.0/4096.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;

            gsl::vector K_6_pos=mom_step*(particle->timestep/gamma(mom_step));
            gsl::vector K_6_mom=force(pos_step, mom_step, particle->charge);
            K_6_mom*=particle->timestep;





            gsl::vector pos_O4=K_1_pos*(2825.0/27648.0);
            gsl::vector mom_O4=K_1_mom*(2825.0/27648.0);

            //pos_O4.mult_add( K_2_pos, 0.0 );
            //mom_O4.mult_add( K_2_pos, 0.0 );

            pos_O4.mult_add( K_3_pos, (18575.0/48384.0) );
            mom_O4.mult_add( K_3_mom, (18575.0/48384.0) );

            pos_O4.mult_add( K_4_pos, (13525.0/55296.0) );
            mom_O4.mult_add( K_4_mom, (13525.0/55296.0) );

            pos_O4.mult_add( K_5_pos, (277.0/14336.0) );
            mom_O4.mult_add( K_5_mom, (277.0/14336.0) );

            pos_O4.mult_add( K_6_pos, (1.0/4.0) );
            mom_O4.mult_add( K_6_mom, (1.0/4.0) );

            //pos_O4+=particle.position; //don't need to do this
            //mom_O4+=particle.momentum;





            gsl::vector pos_O5=K_1_pos*(37.0/378.0);
            gsl::vector mom_O5=K_1_mom*(37.0/378.0);

            //pos_O5.mult_add( K_2_pos, 0.0 );
            //mom_O5.mult_add( K_2_pos, 0.0 );

            pos_O5.mult_add( K_3_pos, (250.0/621.0) );
            mom_O5.mult_add( K_3_mom, (250.0/621.0) );

            pos_O5.mult_add( K_4_pos, (125.0/594.0) );
            mom_O5.mult_add( K_4_mom, (125.0/594.0) );

            //pos_O5.mult_add( K_5_pos, 0.0 );
            //mom_O5.mult_add( K_5_pos, 0.0 );

            pos_O5.mult_add( K_6_pos, (512.0/1771.0) );
            mom_O5.mult_add( K_6_mom, (512.0/1771.0) );

            pos_O4-=pos_O5;//for calculating the error
            mom_O4-=mom_O5;

            pos_O5+=particle->position;
            mom_O5+=particle->momentum;

            double pos_error_sq=pos_O4.sum_of_squares() ;
            double mom_error_sq=mom_O4.sum_of_squares() ;

            pos_error_sq=pos_tol*pos_tol/pos_error_sq;
            mom_error_sq=mom_tol*mom_tol/mom_error_sq;

            double err_f=min(pos_error_sq, mom_error_sq );//note the inverses

            if(err_f>1 )//error is good, exit
            {
                //set timestep
                particle->next_timestep=particle->timestep*kappa*pow( sqrt(err_f), 0.25);

                particle->current_time+=particle->timestep;
                particle->position=pos_O5;
                particle->momentum=mom_O5;


                acceptable=true;
            }
            else
            {//repeat with new timestep
                particle->next_timestep=particle->timestep*kappa*pow( sqrt(err_f), 0.20);

                acceptable=false;
            }
        }
    }
};

int main()
{
	int number_itterations=1000000;
    double particle_removal_energy=10.0/energy_units_kev; //remove particles that have energy less than this

    double pos_tol=0.0001;
    double mom_tol=0.0001;

	//initialize electric field
	uniform_field E_field;
	E_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -Kilo/distance_units);
	E_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 400/distance_units);
	E_field.set_value(0, 0, -600e3/E_field_units); //400
	//E_field.set_value(0, 0, 0/E_field_units);

	//magnetic field is zero
	uniform_field B_field;
	B_field.set_minimum(-Kilo/distance_units, -Kilo/distance_units, -1/distance_units);
	B_field.set_maximum(Kilo/distance_units, Kilo/distance_units, 20*Kilo/distance_units);
	//B_field.set_value(0, 0.5*50*micro, 0.866*50*micro);
	B_field.set_value(0, 0, 0);


	////  initialize physics engines ////
	rand_gen rand;

    //moller scattering
    moller_table moller_engine(particle_removal_energy, 200000/energy_units_kev, 400, true);

    //shielded coulomb diffusion
    diffusion_table coulomb_scattering_engine;

    //interaction chooser
    interaction_chooser_linear<1> interaction_engine(moller_engine); //only one interaction at the moment

	//force
	apply_charged_force force_engine(particle_removal_energy, E_field.pntr(), B_field.pntr() );
    force_engine.set_max_timestep( coulomb_scattering_engine.max_timestep() );
    force_engine.set_errorTol(pos_tol, mom_tol);


	////output file////
    particle_history_out save_data;

	////initial particle////
	time_tree<particle_T> electrons;
	particle_T* new_electron;

	//list<particle_T*> electrons;
    new_electron= electrons.emplace(0);
	//electrons.emplace_back( new particle_T);
	new_electron->set_position(0,0,0);
	new_electron->set_momentum(0,0, KE_to_mom( 5000.0/energy_units_kev ) );
	new_electron->update_energy();
    save_data.new_particle(new_electron);

    new_electron= electrons.emplace(0);
	//electrons.emplace_back( new particle_T);
	new_electron->set_position(0,0,0);
	new_electron->set_momentum(0,0, KE_to_mom( 5000.0/energy_units_kev ) );
	new_electron->update_energy();
    save_data.new_particle(new_electron);

    new_electron= electrons.emplace(0);
	//electrons.emplace_back( new particle_T);
	new_electron->set_position(0,0,0);
	new_electron->set_momentum(0,0, KE_to_mom( 5000.0/energy_units_kev ) );
	new_electron->update_energy();
    save_data.new_particle(new_electron);

    new_electron= electrons.emplace(0);
	//electrons.emplace_back( new particle_T);
	new_electron->set_position(0,0,0);
	new_electron->set_momentum(0,0, KE_to_mom( 5000.0/energy_units_kev ) );
	new_electron->update_energy();
    save_data.new_particle(new_electron);

	//simulate!
    int timestep_trims=0;
    int timestep_redone=0;
    int i=0;
	for(; i<=number_itterations; i++)
	{

        auto current_electron=electrons.pop_first();
        if(not current_electron) {break; } //if current_electron is null, then tree is empty

/////solve equations of motion////

        double old_energy=current_electron->energy;
        auto old_position=current_electron->position;
        auto old_momentum=current_electron->momentum;
        force_engine.charged_particle_RungeKuttaCK(current_electron);
        current_electron->update_energy();

        //remove particle if necisary
        if(current_electron->energy < particle_removal_energy)
        {
            save_data.remove_particle(current_electron);
            delete current_electron;
            continue;
        }

        //for linear interpolation of position and momentum
        auto position_rate_of_change=current_electron->position-old_position;
        auto momentum_rate_of_change=current_electron->momentum-old_momentum;
        position_rate_of_change/=current_electron->timestep;
        momentum_rate_of_change/=current_electron->timestep;

        double energy_before_scattering=current_electron->energy;

//// scattering (moller only presently) ////

        int interaction=-1;
        double time_to_scatter=interaction_engine.sample(old_energy, current_electron->energy, current_electron->timestep, interaction);

        //check error code
        auto error_code=interaction_engine.get_error_flag();
        if(error_code==1) //need to reduce the timestep size
        {
            //print("trim timestep");
            timestep_trims+=1;
            current_electron->next_timestep*=0.5;
        }
        else if(error_code==2) //this timestep was too large
        {
            //print("timestep too large!");
            timestep_redone+=1;
            i-=1; //this itteration doesn't count
            //reset electron
            current_electron->current_time-=current_electron->timestep;
            current_electron->next_timestep=current_electron->timestep *=0.5;
            current_electron->position=old_position;
            current_electron->momentum=old_momentum;
            current_electron->energy=old_energy;

            //place back in pool
            electrons.insert(current_electron->current_time, current_electron);

            //carry on as if nothing ever happend
            continue; //probably wind up with same electron again
        }

        //do the scattering
        if(time_to_scatter <= current_electron->timestep and interaction != -1)
        {


            if(interaction==0) //moller scattering
            {
                //set electron values to time of interaction
                current_electron->current_time += time_to_scatter - current_electron->timestep;
                current_electron->timestep=time_to_scatter;
                current_electron->position = old_position + position_rate_of_change*time_to_scatter;
                current_electron->momentum = old_momentum + momentum_rate_of_change*time_to_scatter;
                current_electron->update_energy();
                energy_before_scattering=current_electron->energy;

                new_electron=moller_engine.single_interaction(energy_before_scattering, current_electron);

                if(new_electron)
                {
                    save_data.new_particle(new_electron);
                    electrons.insert(new_electron->current_time, new_electron);
                }
            }


        }

        //remove particle if necisary
        if(current_electron->energy < particle_removal_energy)
        {
            save_data.remove_particle(current_electron);
            delete current_electron;
            continue;
        }

//// shielded coulomb scattering ////
        coulomb_scattering_engine.scatter(energy_before_scattering, current_electron);

//// update electron ////
        save_data.update_particle(current_electron);
        electrons.insert(current_electron->current_time, current_electron);

	}

	print(i, "iterations of:", number_itterations);
	print(timestep_trims, "trims");
	print(timestep_redone, "re-does");
}














