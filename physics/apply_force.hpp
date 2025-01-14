
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
    double rel_tol;
    //double pos_tol;
    //double mom_tol;

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

    //void set_errorTol(double pos_tol_, double mom_tol_, double kappa_=0.5)
    //{
    //    pos_tol=pos_tol_;
    //    mom_tol=mom_tol_;
    //    kappa=kappa_;
    //}

    void set_errorTol(double rel_tol_, double kappa_=0.9)
    {
        rel_tol=rel_tol_;
        kappa=kappa_;
    }

    gsl::vector force(gsl::vector &position, gsl::vector &momentum, double time, int charge)
    {
        //values
        double momentum_squared=momentum.sum_of_squares();
        double momentum_magnitude=sqrt(momentum_squared);
        double G=gamma(momentum_squared);
        double inverse_gamma=1.0/G;

        //electric field
        gsl::vector force=charge*E_field->get(position, time);

        //magnetic field
        gsl::vector B=charge*B_field->get(position, time);
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

        //friction*=0.2;

        if(friction>0) //don't want weird stuff
        {
            force[0]-=friction*momentum[0]/momentum_magnitude;
            force[1]-=friction*momentum[1]/momentum_magnitude;
            force[2]-=friction*momentum[2]/momentum_magnitude;
        }

        return force;
    }






    void charged_particle_RungeKuttaDP(electron_T *particle)
    // run Dormand-Prince Runge-Kutta with continuous extension, does not rely on the FSAL property
    {
        if(particle->pos_K_interpolant.size()==0)
        //need to initialize particle
        {
            particle->pos_K_interpolant.reserve(8);
            particle->mom_K_interpolant.reserve(8);

            for(int i=0; i<8; i++)
            {
                particle->pos_K_interpolant.push_back(gsl::vector({0,0,0}));
                particle->mom_K_interpolant.push_back(gsl::vector({0,0,0}));
            }
        }


        bool acceptable=false;
        //print("run:", particle.next_timestep);//(sqrt(particle.momentum.sum_of_squares()+1)-1)*510);
        int N=0;
        while(not acceptable)
        {
            N++;

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
            double time=particle->current_time;

            gsl::vector K_1_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_1_mom=force(pos_step, mom_step, time, particle->charge); //note that this is a bit redundant. We could calculate this once at the beginning. It doesn't change if stepsize changes
            K_1_pos*=particle->timestep;
            K_1_mom*=particle->timestep;



            pos_step=K_1_pos*(1.0/5.0);
            mom_step=K_1_mom*(1.0/5.0);
            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep*(1.0/5.0);

            gsl::vector K_2_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_2_mom=force(pos_step, mom_step, time, particle->charge);
            K_2_pos*=particle->timestep;
            K_2_mom*=particle->timestep;



            pos_step=K_1_pos*(3.0/40.0);
            mom_step=K_1_mom*(3.0/40.0);

            pos_step.mult_add( K_2_pos, (9.0/40.0) );
            mom_step.mult_add( K_2_mom, (9.0/40.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep*3.0/10.0;

            gsl::vector K_3_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_3_mom=force(pos_step, mom_step, time, particle->charge);
            K_3_pos*=particle->timestep;
            K_3_mom*=particle->timestep;




            pos_step=K_1_pos*(44.0/45.0);
            mom_step=K_1_mom*(44.0/45.0);

            pos_step.mult_add( K_2_pos, -(56.0/15.0) );
            mom_step.mult_add( K_2_mom, -(56.0/15.0) );

            pos_step.mult_add( K_3_pos, (32.0/9.0) );
            mom_step.mult_add( K_3_mom, (32.0/9.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep*(4.0/5.0);

            gsl::vector K_4_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_4_mom=force(pos_step, mom_step, time, particle->charge);
            K_4_pos*=particle->timestep;
            K_4_mom*=particle->timestep;




            pos_step=K_1_pos*(19372.0/6561.0);
            mom_step=K_1_mom*(19372.0/6561.0);

            pos_step.mult_add( K_2_pos, -(25360.0/2187.0) );
            mom_step.mult_add( K_2_mom, -(25360.0/2187.0) );

            pos_step.mult_add( K_3_pos, (64448.0/6561.0) );
            mom_step.mult_add( K_3_mom, (64448.0/6561.0) );

            pos_step.mult_add( K_4_pos, -(212.0/729.0) );
            mom_step.mult_add( K_4_mom, -(212.0/729.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep*8.0/9.0;

            gsl::vector K_5_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_5_mom=force(pos_step, mom_step, time, particle->charge);
            K_5_pos*=particle->timestep;
            K_5_mom*=particle->timestep;




            pos_step=K_1_pos*(9017.0/3168.0);
            mom_step=K_1_mom*(9017.0/3168.0);

            pos_step.mult_add( K_2_pos, -(355.0/33.0) );
            mom_step.mult_add( K_2_mom, -(355.0/33.0) );

            pos_step.mult_add( K_3_pos, (46732.0/5247.0) );
            mom_step.mult_add( K_3_mom, (46732.0/5247.0) );

            pos_step.mult_add( K_4_pos, (49.0/176.0) );
            mom_step.mult_add( K_4_mom, (49.0/176.0) );

            pos_step.mult_add( K_5_pos, -(5103.0/18656.0) );
            mom_step.mult_add( K_5_mom, -(5103.0/18656.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep;

            gsl::vector K_6_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_6_mom=force(pos_step, mom_step, time, particle->charge);
            K_6_pos*=particle->timestep;
            K_6_mom*=particle->timestep;




            pos_step=K_1_pos*(35.0/384.0);
            mom_step=K_1_mom*(35.0/384.0);

            //K2 is zero


            pos_step.mult_add( K_3_pos, (500.0/1113.0) );
            mom_step.mult_add( K_3_mom, (500.0/1113.0) );

            pos_step.mult_add( K_4_pos, (125.0/192.0) );
            mom_step.mult_add( K_4_mom, (125.0/192.0) );

            pos_step.mult_add( K_5_pos, -(2187.0/6784.0) );
            mom_step.mult_add( K_5_mom, -(2187.0/6784.0) );

            pos_step.mult_add( K_6_pos, (11.0/84.0) );
            mom_step.mult_add( K_6_mom, (11.0/84.0) );

            pos_step+=particle->position;
            mom_step+=particle->momentum;
            time=particle->current_time + particle->timestep;

            gsl::vector K_7_pos=mom_step*(1.0/gamma(mom_step));
            gsl::vector K_7_mom=force(pos_step, mom_step, time, particle->charge);
            K_7_pos*=particle->timestep;
            K_7_mom*=particle->timestep;





            gsl::vector pos_O4=K_1_pos*(5179.0/57600.0);
            gsl::vector mom_O4=K_1_mom*(5179.0/57600.0);

            //pos_O4.mult_add( K_2_pos, 0.0 );
            //mom_O4.mult_add( K_2_mom, 0.0 );

            pos_O4.mult_add( K_3_pos, (7571.0/16695.0) );
            mom_O4.mult_add( K_3_mom, (7571.0/16695.0) );

            pos_O4.mult_add( K_4_pos, (393.0/640.0) );
            mom_O4.mult_add( K_4_mom, (393.0/640.0) );

            pos_O4.mult_add( K_5_pos, -(92097.0/339200.0) );
            mom_O4.mult_add( K_5_mom, -(92097.0/339200.0) );

            pos_O4.mult_add( K_6_pos, (187.0/2100.0) );
            mom_O4.mult_add( K_6_mom, (187.0/2100.0) );

            pos_O4.mult_add( K_7_pos, (1.0/40.0) );
            mom_O4.mult_add( K_7_mom, (1.0/40.0) );

            //pos_O4+=particle.position; //don't need to do this
            //mom_O4+=particle.momentum;



            gsl::vector pos_O5=K_1_pos*(35.0/384.0);
            gsl::vector mom_O5=K_1_mom*(35.0/384.0);

            //pos_O5.mult_add( K_2_pos, 0.0 );
            //mom_O5.mult_add( K_2_mom, 0.0 );

            pos_O5.mult_add( K_3_pos, (500.0/1113.0) );
            mom_O5.mult_add( K_3_mom, (500.0/1113.0) );

            pos_O5.mult_add( K_4_pos, (125.0/192.0) );
            mom_O5.mult_add( K_4_mom, (125.0/192.0) );

            pos_O5.mult_add( K_5_pos, -(2187.0/6784.0) );
            mom_O5.mult_add( K_5_mom, -(2187.0/6784.0) );

            pos_O5.mult_add( K_6_pos, (11.0/84.0) );
            mom_O5.mult_add( K_6_mom, (11.0/84.0) );

            //pos_O5.mult_add( K_7_pos, 0.0 );
            //mom_O5.mult_add( K_7_mom, 0.0 );

            pos_O4-=pos_O5;//for calculating the error
            mom_O4-=mom_O5;

            double pos_error_sq=pos_O4.sum_of_squares();
            double mom_error_sq=mom_O4.sum_of_squares();

            double max_pos_error_sq=rel_tol*rel_tol*pos_O5.sum_of_squares();
            double max_mom_error_sq=rel_tol*rel_tol*mom_O5.sum_of_squares();

            double err_f=std::min(max_pos_error_sq/pos_error_sq, max_mom_error_sq/mom_error_sq );//note the inverses

            pos_O5+=particle->position;
            mom_O5+=particle->momentum;

            //double pos_error_sq=pos_O4.sum_of_squares();
            //double mom_error_sq=mom_O4.sum_of_squares();

            //pos_error_sq=pos_tol*pos_tol/pos_error_sq;
            //mom_error_sq=mom_tol*mom_tol/mom_error_sq;



            if(err_f>1)//error is good, exit
            {
                //set timestep
                particle->interpolant_timestep=particle->timestep;
                particle->next_timestep=particle->timestep*kappa*std::pow( std::sqrt(err_f), 0.25);

                particle->pos_K_interpolant[0]=particle->position;
                particle->mom_K_interpolant[0]=particle->momentum;

                particle->pos_K_interpolant[1]=K_1_pos;
                particle->mom_K_interpolant[1]=K_1_mom;

                particle->pos_K_interpolant[2]=K_2_pos;
                particle->mom_K_interpolant[2]=K_2_mom;

                particle->pos_K_interpolant[3]=K_3_pos;
                particle->mom_K_interpolant[3]=K_3_mom;

                particle->pos_K_interpolant[4]=K_4_pos;
                particle->mom_K_interpolant[4]=K_4_mom;

                particle->pos_K_interpolant[5]=K_5_pos;
                particle->mom_K_interpolant[5]=K_5_mom;

                particle->pos_K_interpolant[6]=K_6_pos;
                particle->mom_K_interpolant[6]=K_6_mom;

                particle->pos_K_interpolant[7]=K_7_pos;
                particle->mom_K_interpolant[7]=K_7_mom;

                particle->current_time+=particle->timestep;
                particle->position=pos_O5;
                particle->momentum=mom_O5;

                acceptable=true;
            }
            else
            {//repeat with new timestep
                if(N>100)
                {

                    throw gen_exception("error in Dormand-Prince RK: ", N);
                }
                //print(pos_O4.sum_of_squares(), mom_O4.sum_of_squares(), max_pos_error_sq, max_mom_error_sq);

                particle->next_timestep=particle->timestep*kappa*std::pow( std::sqrt(err_f), 0.20);
                acceptable=false;
            }
        }
    }


};

#endif
