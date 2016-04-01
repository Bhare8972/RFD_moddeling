

// generate the tables for diffusion due to elastic scattering shielded coulomb scattering

#include <cmath>

#include "constants.h"
#include "GSL_utils.h"
#include "binary_IO.h"
#include "arrays_IO.h"
#include "functor.h"
#include "integrate.h"

using namespace std;

class diff_cross_section : public functor_1D
{
public:
	double momentum_sq;
	double beta;
	double prefactor;
	double p_factor;
	
	diff_cross_section(double timestep, double energy_kev=0)
	{
		prefactor=timestep*average_air_atomic_number*average_air_atomic_number/(8*3.1415926);
		p_factor=pow(average_air_atomic_number, 2.0/3.0)/(4*183.3*183.3);
		
		set_energy(energy_kev);
	}
	
	void set_energy(double energy_kev)
	{
		double energy=energy_kev/(elementary_charge*electron_rest_energy);
		momentum_sq=(energy+1.0)*(energy+1.0)-1;
		beta=sqrt(momentum_sq/(1+momentum_sq));
	}
	
	double call(double angle)
	{
		double S=sin(angle/2.0);
		double numerator=1.0-beta*beta*S*S;
		double denom=S*S+p_factor/momentum_sq;
		return numerator*prefactor/(denom*beta*momentum_sq);
	}
};

int main()
{
	double time_step=0.001;
	double min_energy=0.02; //keV
	double max_energy=30000; //kev
	int num_energies=10; //????
	
	gsl::vector energy_vector=linspace(min_energy, max_energy, num_energies);
}
