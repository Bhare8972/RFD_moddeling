#ifndef MOLLER_SCATTERING
#define MOLLER_SCATTERING

#include <cmath>

#include "constants.hpp"
#include "GSL_utils.hpp"

#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "spline.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"
#include "interaction_chooser.hpp"

#include "particles.hpp"


class moller_cross_section : public functor_1D
{
public:
    double energy;
    double momentum;
    double gamma;
    double beta;

    moller_cross_section(double energy=lowest_physical_energy)
    {
        set_energy(energy);
    }

    void set_energy(double energy_)
    {
        energy=energy_;
		double momentum_sq=(energy+1.0)*(energy+1.0)-1;
		momentum=std::sqrt(momentum_sq);
		gamma=energy+1.0;
		beta=momentum/gamma;
    }

    void set_momsq(double mom_sq_)
    {
		momentum=std::sqrt(mom_sq_);
		gamma=std::sqrt(1.0+mom_sq_);
		beta=momentum/gamma;
		energy=gamma-1.0;
    }

    double call(double production_energy)
    //return cross section per tau
    {
        double term_1_sqrt=energy/(production_energy*(energy-production_energy));
        double term_2_num=2*(gamma*gamma+gamma)-1;
        double term_2_denom=production_energy*(energy-production_energy)*gamma*gamma;

        return (term_1_sqrt*term_1_sqrt - term_2_num/term_2_denom + 1.0/(gamma*gamma))/beta;

    }

/*
    double integral(double production_energy)
    {
        double term_2_F1=(2*gamma-1)/(energy*gamma*gamma);
        double term_2_F2=std::log(production_energy/(energy-production_energy));
        double term_3=energy/(production_energy*(energy-production_energy));

        return (1.0/gamma-term_2_F1*term_2_F2-term_3 )/beta;
    }

    double rate(double lower_production_energy)
    //equivalent to integral(energy/2.0)- integral(lower_production_energy)
    {
        double term_1_F1=(2*gamma-1)/(energy*gamma*gamma);
        double term_1_F2=std::log(lower_production_energy/(energy-lower_production_energy));
        double term_2=energy/(lower_production_energy*(energy-lower_production_energy));
        double term_3=4.0/energy;

        return (term_1_F1*term_1_F2+term_2-term_3 )/beta;
    }*/
};

class moller_table : public physical_interaction
{
    public:

    rand_threadsafe rand;

    gsl::vector energies;
    gsl::vector num_interactions_per_tau; //maybe turn this into a spline
    std::vector<poly_spline> sampler_splines;

    double lowest_sim_energy;
    bool lowest_energy_constant;

    moller_cross_section cross_section;

    moller_table(double lowest_sim_energy_, double upper_energy, size_t num_energies,bool save_tables=false)
    {
        lowest_sim_energy=lowest_sim_energy_;
        lowest_energy_constant=true;

        energies=logspace(std::log10(lowest_sim_energy*2), std::log10(upper_energy), num_energies+1);//we do not want to do the lower_energy*2 energy
        energies=energies.clone(1,num_energies ); //remove the lowest energy

        sampler_splines.reserve(num_energies);
        num_interactions_per_tau=gsl::vector(num_energies);


        //make tables to save output
        arrays_output tables_out;
        std::shared_ptr<doubles_output> energies_table=std::make_shared<doubles_output>( energies );
        tables_out.add_array(energies_table);

        std::shared_ptr<doubles_output> interactions_table=std::make_shared<doubles_output>(num_interactions_per_tau);
        tables_out.add_array(interactions_table);//I THINK this will work..


        //loop over energies, making table
        for(int energy_i=0; energy_i<num_energies; energy_i++)
        {
            double energy=energies[energy_i]; //do not want to sample the precise lowest energy
            cross_section.set_energy(energy);

            //sample
            cum_adap_simps integrator(&cross_section, lowest_sim_energy, energy/2.0, 1E4);
            gsl::vector points=integrator.points();
            gsl::vector cum_quads=integrator.cum_quads();

            num_interactions_per_tau[energy_i]=cum_quads[cum_quads.size()-1];

            //normalize
            cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

            //invert
            gsl::vector sampler_X;
            gsl::vector sampler_Y;
            make_fix_spline(cum_quads, points, sampler_X, sampler_Y);

            //save to external table
            std::shared_ptr<doubles_output> distribution_points_table=std::make_shared<doubles_output>(sampler_Y);
            std::shared_ptr<doubles_output> distribution_values_table=std::make_shared<doubles_output>(sampler_X);

            tables_out.add_array(distribution_points_table);
            tables_out.add_array(distribution_values_table);

            //make spline
            sampler_splines.emplace_back(sampler_X, sampler_Y);
        }


        if(save_tables)
        {
            binary_output fout("./moller_tables_output");
            tables_out.write_out( &fout);
        }
    }

    void set_lowest_energy(double lowest_energy_)
    {
        lowest_energy_constant=false;
        lowest_sim_energy=lowest_energy_;
    }

    double lowest_scatterer_energy()
    {
        return energies[0];
    }

    double rate(double energy)
    {
        if(energy < energies[0])
        {
            return 0; //do not scatter if energy is too low
        }
        else if( (energy >= energies[energies.size()-1]) or (not lowest_energy_constant))
        {
            cross_section.set_energy(energy);

            cum_adap_simps integrator(&cross_section, lowest_sim_energy, energy/2.0, 1E4);
            gsl::vector cum_quads=integrator.cum_quads();

            return cum_quads[cum_quads.size()-1];
        }
        else
        {
            size_t index=search_sorted_exponential(energies, energy);
            double R=num_interactions_per_tau[index];
            double factor=(energy - energies[index])/(energies[index+1] - energies[index]);
            return R + (num_interactions_per_tau[index+1] - R)*factor; //do linear interpolation
        }
    }

    double sample_production_energy(double energy)
    {
        double U=rand.uniform();

        if( (energy >= energies[energies.size()-1]) or (not lowest_energy_constant))
        {
            cross_section.set_energy(energy);

            //sample
            cum_adap_simps integrator(&cross_section, lowest_sim_energy, energy/2.0, 1E4);
            gsl::vector points=integrator.points();
            gsl::vector cum_quads=integrator.cum_quads();

            //normalize
            cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

            return linear_interpolate(cum_quads, points, U);

        }
        else
        {
            size_t index=search_sorted_exponential(energies, energy);
            double factor=(energy - energies[index])/(energies[index+1] - energies[index]);

            double lower=sampler_splines[index].call(U);
            double upper=sampler_splines[index+1].call(U);
            return lower + (upper-lower)*factor;
        }
    }

    double sample_azimuth()
    {
        return rand.uniform()*2*PI;
    }

    electron_T* single_interaction(double initial_energy, electron_T *electron)
    {
        if(initial_energy< energies[0]) return NULL;

        double initial_momentum=sqrt((initial_energy+1)*(initial_energy+1)-1);;

        double azimuth_angle=sample_azimuth();


        //calculate energy and momentum
        double production_energy=sample_production_energy(initial_energy);
        double new_energy=initial_energy-production_energy;

        double production_mom=sqrt((production_energy+1)*(production_energy+1)-1);
        double new_momentum=sqrt((new_energy+1)*(new_energy+1)-1);

        //calculate relavent angles
        double old_inclination_scatter=acos( ((initial_energy+1)*(new_energy+1)-(production_energy+1))/(initial_momentum*new_momentum) );
        double new_inclination_scatter=acos( ((initial_energy+1)*(production_energy+1)-(new_energy+1))/(initial_momentum*production_mom) );

        //make new electron
        electron_T* new_electron=new electron_T;

        new_electron->position=electron->position.clone();
        new_electron->momentum=electron->momentum*(production_mom/initial_momentum);
        new_electron->timestep=electron->timestep;
        new_electron->charge=-1;//set_electron
        new_electron->current_time=electron->current_time;

        //adjust energy of old electron
        electron->momentum*=(new_momentum/initial_momentum);

        //scatter both particles
        electron->scatter_angle(old_inclination_scatter, azimuth_angle);
        new_electron->scatter_angle(new_inclination_scatter, azimuth_angle+3.1415926);

        electron->update_energy();
        new_electron->update_energy();

        return new_electron;
    }
};


#endif
