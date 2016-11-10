
#ifndef APPLY_FORCE_HPP
#define APPLY_FORCE_HPP

#include "vector.hpp"

#include "GSL_utils.hpp"
#include "constants.hpp"

#include "particles.hpp"
#include "quasi_static_fields.hpp"
#include "bethe_eq.hpp"

class apply_charged_force
{
    public:

    electron_ionization_table electron_table;
    field* E_field; //do not own these two fields
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

        friction*=0.2;

        if(friction>0) //don't want weird stuff
        {
            force[0]-=friction*momentum[0]/momentum_magnitude;
            force[1]-=friction*momentum[1]/momentum_magnitude;
            force[2]-=friction*momentum[2]/momentum_magnitude;
        }

        return force;
    }

    void charged_particle_RungeKuttaCK(electron_T *particle)
    //to make this depend of time, add argument double current_time
    //am using cash-karp parameters for fehlberg method
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
            if(particle->timestep != particle->timestep)
            {
                throw gen_exception("timestep is Nan");
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

            double err_f=std::min(pos_error_sq, mom_error_sq );//note the inverses

            if(err_f>1)//error is good, exit
            {
                //set timestep
                particle->next_timestep=particle->timestep*kappa*std::pow( std::sqrt(err_f), 0.25);

                particle->current_time+=particle->timestep;
                particle->position=pos_O5;
                particle->momentum=mom_O5;

                acceptable=true;
            }
            else
            {//repeat with new timestep
                particle->next_timestep=particle->timestep*kappa*std::pow( std::sqrt(err_f), 0.20);;

                acceptable=false;
            }
        }
    }
};

#endif
