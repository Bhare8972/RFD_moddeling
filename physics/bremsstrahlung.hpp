#ifndef BREMSSTRALUNG_HPP
#define BREMSSTRALUNG_HPP

#include "relativistic_formulas.hpp"

double  bremsstrahlung_cross_section(double initial_energy, double photon_energy, double photon_theta, double final_electron_theta, double delta_electron_photon_phi)
{

    double final_electron_energy=initial_energy-photon_energy;
    double total_initial_energy=initial_energy+1.0;
    double total_final_energy=final_electron_energy+1.0;

    double initial_electron_momentum_squared=total_initial_energy*total_initial_energy - 1.0;
    double final_electron_momentum_squared=total_final_energy*total_final_energy - 1.0;

    double initial_momentum=std::sqrt(initial_electron_momentum_squared);
    double final_momentum=std::sqrt(final_electron_momentum_squared);

    double cos_theta_initial=std::cos(photon_theta);
    double sin_theta_initial=std::sin(photon_theta);

    double cos_theta_final=std::cos(photon_theta)*std::cos(final_electron_theta) + std::sin(photon_theta)*std::sin(final_electron_theta)*std::cos(delta_electron_photon_phi);
    double sin_theta_final=std::sqrt( std::max(0.0, 1.0-cos_theta_final*cos_theta_final));//std::sqrt(1.0-cos_theta_final*cos_theta_final);

    double cos_phi=std::cos(final_electron_theta)-cos_theta_final*cos_theta_initial;

    double q_sq=initial_electron_momentum_squared + final_electron_momentum_squared + photon_energy*photon_energy
    -2*initial_momentum*photon_energy*cos_theta_initial + 2*final_momentum*photon_energy*cos_theta_final
    -2*final_momentum*initial_momentum*(cos_theta_final*cos_theta_initial + sin_theta_final*sin_theta_initial*cos_phi);

    double beta=KE_to_beta(initial_energy);
    double prefactor=beta*final_momentum*sin_theta_initial*std::sin(final_electron_theta)/(photon_energy*initial_momentum*4*PI*average_air_atomic_number*137);

    double K_sq_N=std::pow(7.0, 2.0/3.0)/(111*111.0);
    double K_sq_O=std::pow(8.0, 2.0/3.0)/(111*111.0);
    double K_sq_Ar=std::pow(18.0, 2.0/3.0)/(111*111.0);
    double N_factor=1.0/(q_sq+K_sq_N);
    double O_factor=1.0/(q_sq+K_sq_O);
    double Ar_factor=1.0/(q_sq+K_sq_Ar);

    N_factor=N_factor*N_factor*7.0*7.0*0.784;
    O_factor=O_factor*O_factor*8.0*8.0*0.211;
    Ar_factor=Ar_factor*Ar_factor*18.0*18.0*0.005;


    double A_numerator=final_electron_momentum_squared*sin_theta_final*sin_theta_final*(4.0*total_initial_energy*total_initial_energy-q_sq);
    double A_denom_sqrt=total_final_energy-final_momentum*cos_theta_final;

    double B_numerator=initial_momentum*initial_momentum*sin_theta_initial*sin_theta_initial*(4.0*total_final_energy*total_final_energy-q_sq);
    double B_denom_sqrt=total_initial_energy-initial_momentum*cos_theta_initial;

    double C_numerator=2*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi*(4*total_initial_energy*total_final_energy-q_sq);

    double D_numerator=2*photon_energy*photon_energy*(final_electron_momentum_squared*sin_theta_final*sin_theta_final + initial_electron_momentum_squared*sin_theta_initial*sin_theta_initial
    - 2.0*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi);

    double ret= prefactor * (N_factor+O_factor+Ar_factor) *( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (D_numerator - C_numerator)/(A_denom_sqrt*B_denom_sqrt));

    if(ret!=ret)
    {
        throw gen_exception("warning, nan value in brem");
    }
    return ret;
}

#endif
