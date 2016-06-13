#include <cmath>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"

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
		momentum=sqrt(momentum_sq);
		gamma=energy+1.0;
		beta=momentum/gamma;
    }

    void set_mom_sq(double mom_sq)
    {
		momentum=sqrt(mom_sq);
		gamma=sqrt(1+mom_sq);
		beta=momentum/gamma;
		energy=gamma-1.0;

    }

    double call(double production_energy)
    //return interactions per  production energy per tau
    {
        double term_1_sqrt=energy/(production_energy*(energy-production_energy));
        double term_2_num=2*(gamma*gamma+gamma)-1;
        double term_2_denom=production_energy*(energy-production_energy)*gamma*gamma;

        return (term_1_sqrt*term_1_sqrt - term_2_num/term_2_denom + 1.0/(gamma*gamma))/beta;
    }
};


class moller_table
{
public:
    moller_cross_section cross_section;

    gsl::vector energies;
    gsl::vector num_interactions_per_tau;
    std::vector<poly_quad_spline> production_energy_samplers;

    double lowest_energy:
    rand_gen rand;

    moller_table(double lowest_energy_, double upper_energy, int num_energies, bool write_to_table=false)
    {
        lowest_energy=lowest_energy_;

        gsl::vector energy_vector=logspace(log10(lowest_energy*2), log10(upper_energy_), num_energies+1); //we do not actually want to sample the lowest_energy!!
        energies=gsl::vector(num_energies);
        for(int i=1; i<(num_energies+1); i++) energies=energy_vector[i];

        production_energy_samplers.reserve(num_energies);
        num_interactions_per_tau=gsl::vector(num_energies);


        arrays_output tables_out;
        shared_ptr<doubles_output> energies_table=make_shared<doubles_output>( energies );
        tables_out.add_array(energies_table);

        shared_ptr<doubles_output> interactions_table=make_shared<doubles_output>(num_interactions_per_tau);
        tables_out.add_array(interactions_table);

        //this can be parrellized, but there is presently no reason
        for(size_t energy_i=0; energy_i<num_energies; energy_i++)
        {
            cross_section.set_energy(energy_vector[energy_i+1]);
            cum_adap_simps integrator(&cross_section, lowest_energy_, cross_section.energy/2.0, 1E4);
            gsl::vector points=integrator.points();
            gsl::vector values=integrator.values();
            gsl::vector cum_quads=integrator.cum_quads();

            interactions_per_tau[energy_i]=cum_quads[cum_quads.size()-1];

            cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1
            gsl::vector quad_X;
            gsl::vector quad_Y;
            make_fix_spline(cum_quads, points, quad_X, quad_Y);// hope this function will fix the function if the points are singular!

            shared_ptr<doubles_output> distribution_points_table=make_shared<doubles_output>(quad_Y);
            shared_ptr<doubles_output> distribution_cumquads_table=make_shared<doubles_output>(quad_X);

            tables_out->add_array(distribution_points_table);
            tables_out->add_array(distribution_cumquads_table);

            production_energy_samplers.emplace_back(quad_X, quad_Y);

        }

        if(write_to_table)
        {
            binary_output fout("./moller_tables");
            tables_out.write_out( &fout);
        }

    }
};

















