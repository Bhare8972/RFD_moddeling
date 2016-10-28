
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

//REM: need to multiply CS by 2 pi, and by sin of each theta, and fix for Earth atmo

//assume nitrogen atmosphere
double Z=7;

const double K_sq=std::pow(Z, 2.0/3.0)/(111*111.0);
const double prefactor=Z*Z/(4*PI*PI*average_air_atomic_number*137);

//This is probably not correct in magnitude, due to atmosphere implementation issues
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
    double prefactor_2=beta*final_momentum/(photon_energy*initial_momentum);
    double prefactor_3=1.0/((q_sq+K_sq)*(q_sq+K_sq));


    double A_numerator=final_electron_momentum_squared*sin_theta_final*sin_theta_final*(4.0*total_initial_energy*total_initial_energy-q_sq);
    double A_denom_sqrt=total_final_energy-final_momentum*cos_theta_final;

    double B_numerator=initial_momentum*initial_momentum*sin_theta_initial*sin_theta_initial*(4.0*total_final_energy*total_final_energy-q_sq);
    double B_denom_sqrt=total_initial_energy-initial_momentum*cos_theta_initial;

    double C_numerator=2*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi*(4*total_initial_energy*total_final_energy-q_sq);

    double D_numerator=2*photon_energy*photon_energy*(final_electron_momentum_squared*sin_theta_final*sin_theta_final + initial_electron_momentum_squared*sin_theta_initial*sin_theta_initial
    - 2.0*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi);

    double ret= prefactor * prefactor_2 * prefactor_3*( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (D_numerator - C_numerator)/(A_denom_sqrt*B_denom_sqrt))*sin_theta_initial*std::sin(final_electron_theta);

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

    brem_PE_phi(double _EE, double _PE, double _PT, double _ET)
    {
        electron_energy=_EE;
        photon_energy=_PE;
        photon_theta=_PT;
        electron_theta=_ET;
        precision=1.0E2;
    }
/*
    void print_data()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);
        cheby_integrator.print_data();
    }
*/
    double integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);
        return cheby_integrator.integrate(0, 2*PI);
        //auto CS_spline=cheby_integrator.get_spline();
        //return CS_spline->integrate(2*PI);
    }

    double old_spline_integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);
        auto CS_spline=cheby_integrator.get_spline();
        return CS_spline->integrate(2*PI);
    }

    void fancy_sample(gsl::vector& EP_phi_samples, gsl::vector& CS_samples)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);

        EP_phi_samples=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();
    }

    gsl::vector interpolate(gsl::vector phi_space)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 2*PI);
        print( cheby_integrator.integrate(0, 2*PI) );
        auto CS_spline=cheby_integrator.get_spline();
        return CS_spline->callv(phi_space);
    }

    double operator()(double PE_phi)
    {
        return bremsstrahlung_cross_section(electron_energy, photon_energy, photon_theta, electron_theta, PE_phi);
    }

};

class brem_ETheta //integrates cross section across electron theta
{
public:
    double electron_energy;
    double photon_energy;
    double photon_theta;
    double precision;

    brem_ETheta(double _EE, double _PE, double _PT)
    {
        electron_energy=_EE;
        photon_energy=_PE;
        photon_theta=_PT;
        precision=1.0E6;
    }

    double integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        return cheby_integrator.integrate(0, PI);
        //auto CS_spline=cheby_integrator.get_spline();
        //return CS_spline->integrate(PI);
    }

    double old_spline_integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        auto CS_spline=cheby_integrator.get_spline();
        return CS_spline->integrate(PI);

    }
/*
    double weird_integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        double rate;
        auto sampler=cheby_integrator.inverse_transform(0.1, rate);
        return rate;
    }*/

    void fancy_sample(gsl::vector& ETheta_samples, gsl::vector& CS_samples)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);

        ETheta_samples=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();
    }

    gsl::vector interpolate(gsl::vector ETheta_space)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        print( cheby_integrator.integrate(0, PI) );
        auto CS_spline=cheby_integrator.get_spline();
        return CS_spline->callv(ETheta_space);
    }

    gsl::vector cumquad_interpolate(gsl::vector Etheta_space)
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        auto CS_spline=cheby_integrator.get_spline();
        auto CS_cumquad=CS_spline->integrate();
        return CS_cumquad->callv(Etheta_space);
    }

    double operator()(double Etheta)
    {
        brem_PE_phi integrator(electron_energy, photon_energy, photon_theta, Etheta);
        return integrator.integrate();
    }

    double test(double Etheta)
    {
        brem_PE_phi integrator(electron_energy, photon_energy, photon_theta, Etheta);
        return integrator.old_spline_integrate();
    }
};



int PTcounter=0;
class brem_PTheta //integrates cross section across photron theta
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    double integrand;

    std::shared_ptr<poly_spline>  CS_spline;

    brem_PTheta(double _EE, double _PE)
    {
        electron_energy=_EE;
        photon_energy=_PE;
        precision=1.0E7;
    }

    void setup()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);

        integrand=cheby_integrator.integrate(0, PI);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
    }

    double integrate()
    {
        return integrand;
    }

    gsl::vector interpolate(gsl::vector PTheta_space)
    {
        return CS_spline->callv(PTheta_space);
    }

    double operator()(double Ptheta)
    {
        //print("  ",PTcounter, Ptheta);
        //PTcounter+=1;

        brem_ETheta integrator(electron_energy, photon_energy, Ptheta);
        double I= integrator.integrate();
        if(I<0) { print("ERROR!"); std::cout<<std::setprecision(30)<<Ptheta<<std::endl; }
        return I;
    }
};

int PEcounter=0;
class brem_PEnergy //integrates cross section across photron energy
{
public:
    double electron_energy;
    double min_photon_energy;
    double max_photon_energy;
    double precision;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    double integrand;

    std::shared_ptr<poly_spline>  CS_spline;

    brem_PEnergy(double _EE, double min_PE)
    {
        electron_energy=_EE;
        min_photon_energy=min_PE;
        max_photon_energy=electron_energy-electron_energy/1000.0;
        precision=1.0E7;
    }

    void setup()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_photon_energy, max_photon_energy);

        integrand=cheby_integrator.integrate(min_photon_energy, max_photon_energy);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
    }

    double integrate()
    {
        return integrand;
    }

    gsl::vector interpolate(gsl::vector PEnergy_space)
    {
        return CS_spline->callv(PEnergy_space);
    }

    double operator()(double PEnergy)
    {
        print(" ",PEcounter, PEnergy);
        PEcounter+=1;

        brem_PTheta integrator(electron_energy, PEnergy);
        integrator.setup();
        return integrator.integrate();
    }
};

int EEcounter=0;
class brem_EEnergy //samples cross section across electron energy
{
public:
    double min_electron_energy;
    double max_electron_energy;
    double min_photon_energy;
    double precision;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    double integrand;

    std::shared_ptr<poly_spline>  CS_spline;

    brem_EEnergy(double _min_electron_energy, double _max_electron_energy, double min_PE)
    {
        min_electron_energy=_min_electron_energy;
        max_electron_energy=_max_electron_energy;
        min_photon_energy=min_PE;
        precision=0.5E6;
    }

    void setup()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_electron_energy, max_electron_energy);

        integrand=cheby_integrator.integrate(min_electron_energy, max_electron_energy);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
    }

    double integrate()
    {
        return integrand;
    }

    gsl::vector interpolate(gsl::vector EEnergy_space)
    {
        return CS_spline->callv(EEnergy_space);
    }

    double operator()(double EEnergy)
    {
        print(EEcounter, EEnergy);
        EEcounter+=1;

        brem_PEnergy integrator(EEnergy, min_photon_energy);
        integrator.setup();
        return integrator.integrate();
    }
};



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
    /*
    double electron_energy=6000.0/energy_units_kev;
    double photon_energy= 10.0/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    double photon_theta= 45.0/RTD;
    double electron_theta=6/RTD;
    int N_phi=1000;

    brem_PE_phi brem_phi_sampler(electron_energy, photon_energy, photon_theta, electron_theta);

    auto phi_space=linspace(0, 2.0*3.1415926, N_phi);

    //gsl::vector CS_calc(N_phi);
    //for(int i=0; i<N_phi; i++)
    //{
    //    double phi=phi_space[i];
    //    double CS= brem_phi_sampler(phi);
    //    CS_calc[i]=CS;
    //}

    auto CS_calc = brem_phi_sampler.interpolate(phi_space);

    gsl::vector phi_space_FS;
    gsl::vector CS_calc_FS;
    brem_phi_sampler.fancy_sample(phi_space_FS, CS_calc_FS);

    //brem_phi_sampler.print_data();

    arrays_output out;

    auto phi_space_out=std::make_shared<doubles_output>( phi_space);
    out.add_array(phi_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    auto phi_space_out_FS=std::make_shared<doubles_output>( phi_space_FS);
    out.add_array(phi_space_out_FS);

    auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    out.add_array(CS_calc_out_FS);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);
*/

/*
    double electron_energy=6000.0/energy_units_kev;
    double photon_energy= 10.0/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    //double photon_theta= 30/RTD;
    double photon_theta= 1.52170894158255665118417709891;
    int N_Etheta=100000;

    brem_ETheta brem_Etheta_sampler(electron_energy, photon_energy, photon_theta);


    auto Etheta_space=linspace(0, 3.1415926, N_Etheta);
    //auto Etheta_space=linspace(0.090518, 0.092018, N_Etheta);

    //gsl::vector CS_calc(N_Etheta);
    //for(int i=0; i<N_Etheta; i++)
    //{
    //    double Etheta=Etheta_space[i];
    //    double CS= brem_Etheta_sampler(Etheta);
    //    CS_calc[i]=CS;
    //}

    gsl::vector Etheta_space_FS;
    gsl::vector CS_calc_FS;
    brem_Etheta_sampler.fancy_sample(Etheta_space_FS, CS_calc_FS);

    auto CS_calc = brem_Etheta_sampler.interpolate(Etheta_space);

    arrays_output out;

    auto Etheta_space_out=std::make_shared<doubles_output>( Etheta_space );
    out.add_array(Etheta_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    auto Etheta_space_out_FS=std::make_shared<doubles_output>( Etheta_space_FS );
    out.add_array(Etheta_space_out_FS);

    auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    out.add_array(CS_calc_out_FS);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);
*/

/*
    double electron_energy=6000.0/energy_units_kev;
    double photon_energy= 10.0/energy_units_kev; //note that min_photon_energy must be high enough to avoid numerical issues
    //auto PE_space=linspace(2.0/energy_units_kev, electron_energy-electron_energy/500.0, 200);
    //double photon_energy=PE_space[54];

    int N_Ptheta=1000;

    brem_PTheta brem_Ptheta_sampler(electron_energy, photon_energy);
    brem_Ptheta_sampler.setup();

    auto Ptheta_space=linspace(0, 3.1415, N_Ptheta);

//    gsl::vector CS_calc(N_Ptheta);
//    for(int i=0; i<N_Ptheta; i++)
//    {
//        double Ptheta=Ptheta_space[i];
//        print(i, Ptheta);

//        double CS= brem_Ptheta_sampler(Ptheta);
        //double CSB=brem_Ptheta_sampler.test(Ptheta);

//    print(" C", CS);//, CSB);
//        CS_calc[i]=CS;

//    }

    gsl::vector Ptheta_space_FS=brem_Ptheta_sampler.sample_space;
    gsl::vector CS_calc_FS=brem_Ptheta_sampler.CS_samples;

    auto CS_calc = brem_Ptheta_sampler.interpolate(Ptheta_space);

    print("saving");
    arrays_output out;

    auto Ptheta_space_out=std::make_shared<doubles_output>( Ptheta_space );
    out.add_array(Ptheta_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    auto Ptheta_space_out_FS=std::make_shared<doubles_output>( Ptheta_space_FS );
    out.add_array(Ptheta_space_out_FS);

    auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    out.add_array(CS_calc_out_FS);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);
*/



/*
    double electron_energy=6000.0/energy_units_kev;
    double min_photon_energy= 5.0/energy_units_kev;
    int N_PEnergy=10000;

    brem_PEnergy brem_Penergy_sampler(electron_energy, min_photon_energy);
    brem_Penergy_sampler.setup();

    auto PEnergy_space=linspace(min_photon_energy, electron_energy-electron_energy/500.0, N_PEnergy);

    //gsl::vector CS_calc(N_PEnergy);
    //for(int i=0; i<N_PEnergy; i++)
    //{
    //    double Penergy=PEnergy_space[i];
    //    print(i, Penergy);

    //    double CS= brem_Penergy_sampler(Penergy);

    //    print(" C", CS);
    //    CS_calc[i]=CS;

    //}


    gsl::vector PEnergy_space_FS=brem_Penergy_sampler.sample_space;
    gsl::vector CS_calc_FS=brem_Penergy_sampler.CS_samples;

    //print(brem_Ptheta_sampler.integrate());

    auto CS_calc = brem_Penergy_sampler.interpolate(PEnergy_space);

    print("saving");
    arrays_output out;

    auto PEnergy_space_out=std::make_shared<doubles_output>( PEnergy_space );
    out.add_array(PEnergy_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    auto Ptheta_space_out_FS=std::make_shared<doubles_output>( PEnergy_space_FS );
    out.add_array(Ptheta_space_out_FS);

    auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    out.add_array(CS_calc_out_FS);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);
*/





    double min_electron_energy=5.0/energy_units_kev;
    double max_electron_energy=50000.0/energy_units_kev;
    double min_photon_energy= 2.0/energy_units_kev;
    int N_EEnergy=10000;


    brem_EEnergy brem_sampler(min_electron_energy, max_electron_energy, min_photon_energy);
    brem_sampler.setup();

    auto EEnergy_space=linspace(min_electron_energy, max_electron_energy*0.99, N_EEnergy);

    //gsl::vector CS_calc(N_PEnergy);
    //for(int i=0; i<N_PEnergy; i++)
    //{
    //    double Penergy=PEnergy_space[i];
    //    print(i, Penergy);

    //    double CS= brem_Penergy_sampler(Penergy);

    //    print(" C", CS);
    //    CS_calc[i]=CS;

    //}


    gsl::vector PEnergy_space_FS=brem_sampler.sample_space;
    gsl::vector CS_calc_FS=brem_sampler.CS_samples;

    //print(brem_Ptheta_sampler.integrate());

    auto CS_calc = brem_sampler.interpolate(EEnergy_space);

    print("saving");
    arrays_output out;

    auto PEnergy_space_out=std::make_shared<doubles_output>( EEnergy_space );
    out.add_array(PEnergy_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    auto Ptheta_space_out_FS=std::make_shared<doubles_output>( PEnergy_space_FS );
    out.add_array(Ptheta_space_out_FS);

    auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    out.add_array(CS_calc_out_FS);

    binary_output fout("./brem_test_out");
    out.write_out(&fout);



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















