
#include <iomanip>
#include <gsl/gsl_integration.h>

#include "GSL_utils.hpp"
#include "constants.hpp"
#include "gen_ex.hpp"
#include "chebyshev.hpp"
#include "arrays_IO.hpp"
#include "span_tree.hpp"
#include "functor.hpp"
#include "rand.hpp"

#include "../physics/relativistic_formulas.hpp"
#include "../physics/brems_pairPr.hpp"
//#include "../physics/bremsstrahlung_scattering.hpp"

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

class brem_PE_phi : public functor_1D //integrates cross section across photon-electron phi
{
public:
    double electron_energy;
    double photon_energy;
    double photon_theta;
    double electron_theta;

    gsl_integration_workspace* workspace;
    gsl_function func;

    brem_PE_phi()
    {
        workspace=gsl_integration_workspace_alloc(1000);
        func=get_gsl_func();
    }

    ~brem_PE_phi()
    {
        gsl_integration_workspace_free(workspace);
    }

    void set(double _EE, double _PE, double _PT, double _ET)
    {
        electron_energy=_EE;
        photon_energy=_PE;
        photon_theta=_PT;
        electron_theta=_ET;
    }

    double integrate()
    {
        double result, error;

        gsl_integration_qag(&func,0,2.0*PI,   0,1e-4,1000,    6,workspace,  &result,&error);

        return result;
    }

    double call(double PE_phi)
    {
        return bremsstrahlung_cross_section(electron_energy, photon_energy, photon_theta, electron_theta, PE_phi);
    }

};



class brem_ETheta : public functor_1D //integrates cross section across electron theta
{
public:

    brem_PE_phi phi_workspace;

    gsl_integration_workspace* workspace;
    gsl_function func;

    brem_ETheta()
    {
        workspace=gsl_integration_workspace_alloc(1000);
        func=get_gsl_func();
    }

    ~brem_ETheta()
    {
        gsl_integration_workspace_free(workspace);
    }

    void set(double _EE, double _PE, double _PT)
    {
        phi_workspace.set(_EE, _PE, _PT, 0);
    }

    double integrate()
    {
        double result, error;

        gsl_integration_qag(&func,0,PI,   0,1e-4,1000,    6,workspace,  &result,&error);

        return result;
    }

    double call(double Etheta)
    {
        phi_workspace.electron_theta=Etheta;
        return phi_workspace.integrate();
    }
};


int PTcounter=0;
class brem_PTheta //integrates cross section across photron theta
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    brem_ETheta Etheta_workspace;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    double integrand;

    std::shared_ptr<poly_spline>  CS_spline;
    std::shared_ptr<poly_spline>  inverse_spline;

    brem_PTheta()
    {
        electron_energy=0.0;
        photon_energy=0.0;
        precision=1.0E6;
    }

    void setup(double _EE, double _PE)
    {
        reset(_EE, _PE);
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);

        integrand=cheby_integrator.integrate(0, PI);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
        inverse_spline=cheby_integrator.get_inverse_spline();
    }

    void reset(double _EE, double _PE)
    {
        electron_energy=_EE;
        photon_energy=_PE;
    }

    double integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);
        return cheby_integrator.integrate(0, PI);
    }

    gsl::vector interpolate(gsl::vector PTheta_space)
    {
        return CS_spline->callv(PTheta_space);
    }

    double operator()(double Ptheta)
    {
        //print("  ",PTcounter, Ptheta);
        //PTcounter+=1;

        Etheta_workspace.set(electron_energy, photon_energy, Ptheta);
        return Etheta_workspace.integrate();
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

    brem_PTheta Ptheta_workspace;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    double integrand;

    std::shared_ptr<poly_spline>  CS_spline;
    std::shared_ptr<poly_spline>  inverse_spline;
    CDF_sampler sampler;

    brem_PEnergy()
    {
        electron_energy=0;
        min_photon_energy=0;
        max_photon_energy=0;
        precision=1.0E7;
    }

    void setup(double _EE, double min_PE)
    {
        reset(_EE, min_PE);

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_photon_energy, max_photon_energy);

        //integrand=cheby_integrator.integrate(min_photon_energy, max_photon_energy);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
        inverse_spline=cheby_integrator.get_inverse_spline();
        sampler=cheby_integrator.inverse_transform(1.0, integrand);
    }

    void reset(double _EE, double min_PE)
    {
        electron_energy=_EE;
        min_photon_energy=min_PE;
        max_photon_energy=electron_energy-electron_energy/1000.0;
    }

    double integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_photon_energy, max_photon_energy);
        return cheby_integrator.integrate(min_photon_energy, max_photon_energy);
    }

    gsl::vector interpolate(gsl::vector PEnergy_space)
    {
        return CS_spline->callv(PEnergy_space);
    }


    double operator()(double PEnergy)
    {
        print(" ",PEcounter, PEnergy);
        PEcounter+=1;

        Ptheta_workspace.reset(electron_energy, PEnergy);
        return Ptheta_workspace.integrate();
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

    brem_PEnergy Penergy_workspace;

    gsl::vector sample_space;
    gsl::vector CS_samples;

    std::shared_ptr<poly_spline>  CS_spline;

    brem_EEnergy(double _min_electron_energy, double _max_electron_energy, double min_PE)
    {
        min_electron_energy=_min_electron_energy;
        max_electron_energy=_max_electron_energy;
        min_photon_energy=min_PE;
        precision=0.5E7;
    }

    void setup()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_electron_energy, max_electron_energy);

        sample_space=cheby_integrator.get_points();
        CS_samples=cheby_integrator.get_values();

        CS_spline=cheby_integrator.get_spline();
    }
    gsl::vector interpolate(gsl::vector EEnergy_space)
    {
        return CS_spline->callv(EEnergy_space);
    }

    double operator()(double EEnergy)
    {
        print(EEcounter, EEnergy);
        EEcounter+=1;

        Penergy_workspace.reset(EEnergy, min_photon_energy);
        return Penergy_workspace.integrate();
    }
};

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
    double photon_theta= 30/RTD;
    int N_Etheta=1000;

    brem_ETheta brem_Etheta_sampler(electron_energy, photon_energy, photon_theta);


    auto Etheta_space=linspace(0, 3.1415926, N_Etheta);
    //auto Etheta_space=linspace(0.090518, 0.092018, N_Etheta);

    gsl::vector CS_calc(N_Etheta);
    for(int i=0; i<N_Etheta; i++)
    {
        print(i);
        double Etheta=Etheta_space[i];
        double CS= brem_Etheta_sampler.call(Etheta);
        CS_calc[i]=CS;
    }

    //gsl::vector Etheta_space_FS;
    //gsl::vector CS_calc_FS;
    //brem_Etheta_sampler.fancy_sample(Etheta_space_FS, CS_calc_FS);

    //auto CS_calc = brem_Etheta_sampler.interpolate(Etheta_space);

    arrays_output out;

    auto Etheta_space_out=std::make_shared<doubles_output>( Etheta_space );
    out.add_array(Etheta_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);

    //auto Etheta_space_out_FS=std::make_shared<doubles_output>( Etheta_space_FS );
    //out.add_array(Etheta_space_out_FS);

    //auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    //out.add_array(CS_calc_out_FS);

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
//
//    gsl::vector CS_calc(N_Ptheta);
//    for(int i=0; i<N_Ptheta; i++)
//    {
//        double Ptheta=Ptheta_space[i];
//        print(i, Ptheta);

//        double CS= brem_Ptheta_sampler(Ptheta);
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




    double electron_energy=6000.0/energy_units_kev;
    double min_photon_energy= 5.0/energy_units_kev;
    int N_PEnergy=1000;

    brem_PEnergy brem_Penergy_sampler;
    brem_Penergy_sampler.setup(electron_energy, min_photon_energy);

    auto PEnergy_space=linspace(min_photon_energy, electron_energy-electron_energy/500.0, N_PEnergy);

    //gsl::vector CS_calc(N_PEnergy);
    //for(int i=0; i<N_PEnergy; i++)
    //{
    //    double Penergy=PEnergy_space[i];
    //    print(i, Penergy);

    //    double CS= brem_Penergy_sampler(Penergy);

    //    CS_calc[i]=CS;

    //}







    print("saving");
    arrays_output out;

    auto CS_calc = brem_Penergy_sampler.interpolate(PEnergy_space);
    auto PEnergy_space_out=std::make_shared<doubles_output>( PEnergy_space );
    out.add_array(PEnergy_space_out);

    auto CS_calc_out=std::make_shared<doubles_output>( CS_calc );
    out.add_array(CS_calc_out);



    //gsl::vector PEnergy_space_FS=brem_Penergy_sampler.sample_space;
    //gsl::vector CS_calc_FS=brem_Penergy_sampler.CS_samples;
    //auto Ptheta_space_out_FS=std::make_shared<doubles_output>( PEnergy_space_FS );
    //out.add_array(Ptheta_space_out_FS);

    //auto CS_calc_out_FS=std::make_shared<doubles_output>( CS_calc_FS );
    //out.add_array(CS_calc_out_FS);




    //auto inverse_space=linspace(0.0, brem_Penergy_sampler.integrand-brem_Penergy_sampler.integrand/500.0, N_PEnergy);
    //auto inverse_CS=brem_Penergy_sampler.inverse_spline->callv(inverse_space);
    //auto Ptheta_space_out_FS=std::make_shared<doubles_output>( inverse_space );
    //out.add_array(Ptheta_space_out_FS);

    //auto CS_calc_out_FS=std::make_shared<doubles_output>( inverse_CS );
    //out.add_array(CS_calc_out_FS);

    CDF_sampler sampler;

    sampler.splines=std::make_shared< std::vector<polynomial> >( brem_Penergy_sampler.inverse_spline->splines );

    gsl::vector weights( sampler.splines->size() );
    for(int i=0; i<sampler.splines->size(); i++)
    {
        weights[i]=brem_Penergy_sampler.inverse_spline->x_vals[i+1] - brem_Penergy_sampler.inverse_spline->x_vals[i];
    }

    //fix all splines to have ranges between 0 and 1
    for(int i=0; i<sampler.splines->size(); i++)
    {
        double Xmin=brem_Penergy_sampler.inverse_spline->x_vals[i];
        double Xrange=brem_Penergy_sampler.inverse_spline->x_vals[i+1] - brem_Penergy_sampler.inverse_spline->x_vals[i];

        polynomial& poly= (*sampler.splines)[i];

        double Xrange_factor=1;
        for(int factor_j=0; factor_j<poly.weights.size(); factor_j++)
        {
            double new_factor=0;
            double Xmin_factor=1;
            for(int iter_i=factor_j; iter_i<poly.weights.size(); iter_i++)
            {
                new_factor+=poly.weights[iter_i]*Xmin_factor*Xrange_factor*gsl_sf_choose(iter_i, factor_j);

                Xmin_factor*=Xmin;
            }
            Xrange_factor*=Xrange;

            poly.weights[factor_j]=new_factor;
        }
    }
    sampler.set(weights);


    int Nsamples=10000000;
    rand_gen rand;
    gsl::vector samples(Nsamples);
    for(int i=0; i<Nsamples; i++)
    {
        samples[i]=brem_Penergy_sampler.sampler.sample(  rand.uniform() );
    }

    auto samples_out=std::make_shared<doubles_output>( samples );
    out.add_array(samples_out);


    binary_output fout("./brem_test_out");
    out.write_out(&fout);




/*
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
*/

}















