
#include <iomanip>

#include "GSL_utils.hpp"
#include "constants.hpp"
#include "gen_ex.hpp"
#include "chebyshev.hpp"
#include "arrays_IO.hpp"
#include "span_tree.hpp"

#include "../physics/relativistic_formulas.hpp"
#include "../physics/brems_pairPr.hpp"
#include "../physics/bremsstrahlung_scattering.hpp"


//This is probably not correct in magnitude, due to atmosphere implementation issues
double  bremsstrahlung_cross_section(double initial_energy, double photon_energy, double photon_theta, double final_electron_theta, double delta_electron_photon_phi)
{

    double final_electron_energy=initial_energy-photon_energy;
    double initial_electron_momentum_squared=(1+initial_energy)*(1+initial_energy) - 1.0;
    double final_electron_momentum_squared=(1+final_electron_energy)*(1+final_electron_energy) - 1.0;

    double initial_momentum=std::sqrt(initial_electron_momentum_squared);
    double final_momentum=std::sqrt(final_electron_momentum_squared);

    double cos_theta_initial=std::cos(photon_theta);
    double sin_theta_initial=std::sin(photon_theta);

    double cos_theta_final=std::cos(photon_theta)*std::cos(final_electron_theta) + std::sin(photon_theta)*std::sin(final_electron_theta)*std::cos(delta_electron_photon_phi);
    double sin_theta_final==std::sqrt( std::max(0.0, 1.0-cos_theta_final*cos_theta_final));//std::sqrt(1.0-cos_theta_final*cos_theta_final);

    double cos_phi=std::cos(final_electron_theta)-cos_theta_final*cos_theta_initial;


    double q_sq=initial_electron_momentum_squared + final_electron_momentum_squared + photon_energy*photon_energy
    -2*initial_momentum*photon_energy*cos_theta_initial + 2*final_momentum*photon_energy*cos_theta_final
    -2*final_momentum*initial_momentum*(cos_theta_final*cos_theta_initial + sin_theta_final*sin_theta_initial*cos_phi);

    double prefactor_2=final_momentum/(photon_energy*initial_momentum);
    double prefactor_3=1.0/((q_sq+K_sq)*(q_sq+K_sq));

    double total_initial_energy=initial_energy+1.0;
    double total_final_energy=final_electron_energy+1.0;


    double A_numerator=final_electron_momentum_squared*sin_theta_final*sin_theta_final*(4.0*total_initial_energy*total_initial_energy-q_sq);
    double A_denom_sqrt=total_final_energy-final_momentum*cos_theta_final;

    double B_numerator=initial_momentum*initial_momentum*sin_theta_initial*sin_theta_initial*(4.0*total_final_energy*total_final_energy-q_sq);
    double B_denom_sqrt=total_initial_energy-initial_momentum*cos_theta_initial;

    double C_numerator=2*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi*(4*total_initial_energy*total_final_energy-q_sq);

    double D_numerator=2*photon_energy*photon_energy*(final_electron_momentum_squared*sin_theta_final*sin_theta_final + initial_electron_momentum_squared*sin_theta_initial*sin_theta_initial
    - 2.0*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi);

    //return prefactor*prefactor_2*prefactor_3*( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (C_numerator + D_numerator)/(CD_denom_1*CD_denom_2));

    double ret= prefactor * prefactor_2 * prefactor_3*( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (C_numerator + D_numerator)/(A_denom_sqrt*B_denom_sqrt));

    if(ret!=ret)
    {
        throw gen_exception("warning, nan value in brem");
    }
    return ret;
}

class brem_PE_phi //integrates cross section across photon-electron phi
{
public:
    double electron_energy;
    double photon_energy;
    double photon_theta;
    double electron_theta;
    double precision;

    brem_PE_phi(double _EE, double _PE, double _PT, double _ET, double _precision)
    {
        electron_energy=_EE;
        photon_energy=_PE;
        photon_theta=_PT;
        electron_theta=_ET;
        precision=_precision;
    }

    double integrate()
    {
        print("NOT IMPLEMENTED");
    }

    void fancy_sample(gsl::vector& EP_phi_samples, gsl::vector& CS_samples)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);

        EP_phi_samples=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();
    }

    double operator()(double PE_phi)
    {
        return bremsstrahlung_cross_section(electron_energy, photon_energy, photon_theta, electron_theta, PE_phi);
    }

};

I AM HERE. PLOT THE ABOVE!

/*

class bremsstrahlung_cross_section_across_theta //differential in photon energy. Integrates (and samples) the theta angle
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    std::shared_ptr<poly_spline> double_diff_CS;
    //AdaptiveSpline_Cheby_O3::rand_sampler theta_sampler;
    double integrated_CS;

    gsl::vector theta_samples;
    gsl::vector CS_samples;

    bremsstrahlung_cross_section_across_theta(double _electron_energy, double _photon_energy, double _precision)
    {
        electron_energy=_electron_energy;
        photon_energy=_photon_energy;
        precision=_precision;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);

        double_diff_CS=cheby_integrator.get_spline();
        //theta_sampler=cheby_integrator.inverse_transform(0.01, integrated_CS);
        integrated_CS=double_diff_CS->integrate(PI);

        theta_samples=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();
    }

    double operator()(double theta)
    {
        //std::cout<<"sample:";
        //std::cout << std::scientific << std::setprecision(15) << theta << std::endl;
        //need to include atmosphere here
        return KE_to_beta(electron_energy)*brems(electron_rest_energy*electron_energy, electron_rest_energy*photon_energy, theta)/(2*PI*average_air_atomic_number*electron_classical_radius*electron_classical_radius);
    }
};


class bremsstrahlung_cross_section_across_PE
{
public:
    double electron_energy;
    double min_photon_energy;
    double precision;

    span_tree<bremsstrahlung_cross_section_across_theta> differential_cross_sections;

    std::shared_ptr<poly_spline> diff_CS;
    //AdaptiveSpline_Cheby_O3::rand_sampler PE_sampler;
    double total_CS;

    bremsstrahlung_cross_section_across_PE(double _electron_energy, double _min_photon_energy, double _precision, double upper_level_fraction=1000.0)
    {
        electron_energy=_electron_energy;
        min_photon_energy=_min_photon_energy;
        precision=_precision;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, _min_photon_energy, (_electron_energy-_electron_energy/upper_level_fraction)); //note: our methods do not work if the photon energy gets too close to electron energy. CS goes to zero anyway

        diff_CS=cheby_integrator.get_spline();
        //PE_sampler=cheby_integrator.inverse_transform(0.01, total_CS);
        total_CS=diff_CS->integrate(_electron_energy);
    }

    double operator()(double photon_energy)
    {
        std::cout<<"sample:";
        std::cout << std::scientific << std::setprecision(15) << photon_energy*energy_units_kev << std::endl;
        auto new_point= new bremsstrahlung_cross_section_across_theta(electron_energy, photon_energy, precision);
        differential_cross_sections.insert(photon_energy, new_point);
        return new_point->integrated_CS;
    }
};

class bremsstrahlung_table
{
public:
    double min_electron_energy;
    double max_electron_energy;
    double min_photon_energy;
    double precision;


    span_tree<bremsstrahlung_cross_section_across_PE> differential_cross_sections;
    std::shared_ptr<poly_spline> total_CS;

    bremsstrahlung_table(double _min_electron_energy, double _max_electron_energy, double _min_photon_energy, double _precision)
    {
        min_electron_energy=_min_electron_energy;
        max_electron_energy=_max_electron_energy;
        min_photon_energy=_min_photon_energy;
        precision=_precision;

        if( not (min_photon_energy<min_electron_energy))
        {
            throw gen_exception("minimum photon energy must be less than ");
        }

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_electron_energy, max_electron_energy);

        total_CS=cheby_integrator.get_spline();
    }

    double operator()(double electron_energy)
    {
        auto new_point= new bremsstrahlung_cross_section_across_PE(electron_energy, min_photon_energy, precision);
        differential_cross_sections.insert(electron_energy, new_point);
        return new_point->total_CS;
    }
};

double penelope_angular_distribution(double beta, double A, double B, double theta)
{
    double beta_prime=beta*(1+B);
    double cos_t=cos(theta);

    double denom=(1-beta_prime*cos_t);
    double factor_1=(cos_t-beta_prime)/denom;
    factor_1*=factor_1;

    double factor_2=(1.0-beta_prime*beta_prime)/(denom*denom);

    return A*(3.0/8.0)*(1+factor_1)*factor_2 + (1.0-A)*(4.0/3.0)*(1.0-factor_1)*factor_2;
}*/

int main()
{
    /*double min_electron_energy=2.0/energy_units_kev;
    double minimum_photon_energy=1.1/energy_units_kev;
    double max_electron_energy=1.00E7/energy_units_kev;
    int num_electron_energies=10000;

    auto electron_energies=logspace( std::log(min_electron_energy), std::log(max_electron_energy),  num_electron_energies);

    bremsstrahlung_table brem_table(min_electron_energy, max_electron_energy, minimum_photon_energy, 10E20);
    auto calculated_rates=brem_table.total_CS->callv(electron_energies);


    bremsstrahlung_scattering seltzer_berger_rates(minimum_photon_energy);
    gsl::vector tabled_rates(num_electron_energies);
    LOOP(tabled_rates, R, electron_energies, seltzer_berger_rates.rate(R));


    arrays_output out;

    auto electron_energies_out=std::make_shared<doubles_output>( electron_energies );
    out.add_array(electron_energies_out);

    auto calc_rates_out=std::make_shared<doubles_output>( calculated_rates );
    out.add_array(calc_rates_out);

    auto tabled_rates_out=std::make_shared<doubles_output>( tabled_rates );
    out.add_array(tabled_rates_out);

    binary_output fout("brem_test");
    out.write_out(&fout);*/

/*
    double electron_energy=6000.0/energy_units_kev;
    double min_photon_energy=1.0/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    int N_energies=10000;


    auto energy_space=linspace(min_photon_energy, 20.0/energy_units_kev, N_energies); //electron_energy-electron_energy/500.0, N_energies);


    //bremsstrahlung_cross_section_across_PE CS_PE_diff(electron_energy, min_photon_energy, 0.1);
    //auto CS_calc=CS_PE_diff.diff_CS->callv(energy_space);

    gsl::vector CS_calc(N_energies);
    for(int i=0; i<N_energies; i++)
    {
        double PE=energy_space[i];
        bremsstrahlung_cross_section_across_theta new_point(electron_energy, PE, 0.01);
        CS_calc[i]=new_point.integrated_CS;
    }



    arrays_output out;

    auto energy_space_out=std::make_shared<doubles_output>( energy_space*energy_units_kev );
    out.add_array(energy_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);*/



/*
    double electron_energy=6000.0/energy_units_kev;
    //double photon_energy=59.35778763843703/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    double photon_energy=  2.0/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    int N_theta=10000;


    //bremsstrahlung_cross_section_across_theta  CS_theta_diff(electron_energy, photon_energy, 0.1);
    auto theta_space=linspace(0, 3.1415926, N_theta);
    //auto CS_calc=CS_theta_diff.double_diff_CS->callv(theta_space);

    gsl::vector CS_calc(N_theta);
    for(int i=0; i<N_theta; i++)
    {
        double theta=theta_space[i];
        double CS= KE_to_beta(electron_energy)*brems(electron_rest_energy*electron_energy, electron_rest_energy*photon_energy, theta)/(2*PI*average_air_atomic_number*electron_classical_radius*electron_classical_radius);
        CS_calc[i]=CS;
    }


//CS_theta_diff.theta_samples
//CS_theta_diff.CS_samples

    arrays_output out;

    auto energy_space_out=std::make_shared<doubles_output>( theta_space);
    out.add_array(energy_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);*/


}















