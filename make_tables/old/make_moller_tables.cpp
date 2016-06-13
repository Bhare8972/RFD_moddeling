#include <cmath>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"
#include "gen_ex.hpp"

using namespace std;

class moller_cross_section : public functor_1D
{
public:
    double energy;
    double momentum;
    double gamma;
    double beta;

    moller_cross_section(double energy=0)
    {
        set_energy(energy);
    }

    void set_energy(double energy_)
    {
        energy=energy_;
		double momentum_sq=(energy+1.0)*(energy+1.0)-1;
		momentum=sqrt(momentum_sq);
		gamma=sqrt(1+momentum_sq);
		beta=momentum/gamma;
    }

    double call(double production_energy)
    //return cross section per tau
    {
        double term_1_sqrt=energy/(production_energy*(energy-production_energy));
        double term_2_num=2*(gamma*gamma+gamma)-1;
        double term_2_denom=production_energy*(energy-production_energy)*gamma*gamma;

        return (term_1_sqrt*term_1_sqrt - term_2_num/term_2_denom + 1.0/(gamma*gamma))/beta;

    }
};

int main()
{
	double max_energy=100000; //kev
	size_t num_energies=100; //????


	gsl::vector energy_vector=logspace(log10(minimum_energy*2), log10(max_energy/energy_units_kev), num_energies+1); //we do not actually want to sample the lowest energy!

	gsl::vector interactions_per_tau(num_energies);
	moller_cross_section cross_section(minimum_energy*2);

    arrays_output tables_out;
    shared_ptr<doubles_output> energies_table=make_shared<doubles_output>( energy_vector.subvector(1,num_energies ) );
    tables_out.add_array(energies_table);

    shared_ptr<doubles_output> interactions_table=make_shared<doubles_output>(interactions_per_tau);
    tables_out.add_array(interactions_table);//I THINK this will work...

    //this can be parrellized, but there is presently no reason
	for(size_t energy_i=0; energy_i<num_energies; energy_i++)//we dont want to sample the precise minimum*2 energy
	{
        print("energy:", energy_vector[energy_i+1]*energy_units_kev, "kev");
        cross_section.set_energy(energy_vector[energy_i+1]);//we don't want to sample the energy minimum_energy*2
        cum_adap_simps integrator(&cross_section, minimum_energy, cross_section.energy/2.0, 1E4);
        gsl::vector points=integrator.points();
		gsl::vector values=integrator.values();
		gsl::vector cum_quads=integrator.cum_quads();

		interactions_per_tau[energy_i]=cum_quads[cum_quads.size()-1];
		print("  :", interactions_per_tau[energy_i], "interactions per tau");
		cum_quads/=cum_quads[cum_quads.size()-1]; //normalize to values from 0 to 1

		gsl::vector quad_X;
		gsl::vector quad_Y;
        make_fix_spline(cum_quads, points, quad_X, quad_Y);// hope this function will fix the function if the points are singular!

        shared_ptr<arrays_output> distribution_table=make_shared<arrays_output>();

        shared_ptr<doubles_output> distribution_points_table=make_shared<doubles_output>(quad_Y);
        //shared_ptr<doubles_output> distribution_values_table=make_shared<doubles_output>(values);
        shared_ptr<doubles_output> distribution_cumquads_table=make_shared<doubles_output>(quad_X);

        distribution_table->add_array(distribution_points_table);
        //distribution_table->add_array(distribution_values_table);
        distribution_table->add_array(distribution_cumquads_table);

        tables_out.add_array(distribution_table);
	}

	binary_output fout("../tables/moller");
	tables_out.write_out( &fout);
}
