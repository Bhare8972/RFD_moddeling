

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"
#include "functor.hpp"
#include "integrate.hpp"

using namespace std;

class diff_cross_section : public functor_1D
{
public:
	double momentum_sq;
	double beta;
	double prefactor;
	double p_factor;

	double K;

	diff_cross_section(double timestep, double energy_kev=0)
	{
		prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926);
		p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3);

		set_energy(energy_kev);

		K=10;
	}

	void set_energy(double energy_kev)
	{
		double energy=energy_kev/(elementary_charge*electron_rest_energy);
		momentum_sq=(energy+1.0)*(energy+1.0)-1;
		beta=sqrt(momentum_sq/(1+momentum_sq));
	}

	double call(double angle)
	{

        return sin(-angle*K*2*3.1415926/100.0);


		//double S=sin(angle/2.0);
		//double numerator=1.0-beta*beta*S*S;
		//double denom=S*S+p_factor/momentum_sq;
		//return numerator*prefactor/(denom*beta*momentum_sq);
	}
};

int main()
{
	double time_step=0.001;
	double min_energy=0.02; //keV
	double max_energy=30000; //kev
	int num_energies=10; //????

	gsl::vector energy_vector=linspace(min_energy, max_energy, num_energies);
	diff_cross_section scatterer(time_step, min_energy);

    cum_adap_simps integrator(&scatterer, 0, 100, 1.0E1);
    print("total integrand:", integrator.quad());
    print("state:", integrator.info());

    auto points=integrator.points();
    auto values=integrator.values();
    auto cum_quads=integrator.cum_quads();

    I AM HERE. FIGURE OUT WHY FOLLOWING ARRAYS ARE PACKED DIFFERENTLY

    for(double p : points)
    {
        print(p);
    }
    print();

    for(double p : values)
    {
        print(p);
    }
    print();

	shared_ptr<doubles_output> points_table(new doubles_output(points));
	shared_ptr<doubles_output> values_table(new doubles_output(values));
	shared_ptr<doubles_output> cum_quads_table(new doubles_output(cum_quads));

	arrays_output array_out;
	array_out.add_array(points_table);
	array_out.add_array(values_table);
	array_out.add_array(cum_quads_table);

	binary_output fout("./tst");
	array_out.write_out( &fout);
}
