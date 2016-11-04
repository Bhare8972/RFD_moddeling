
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
#include "../physics/bremsstrahlung.hpp"


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



class brem_PTheta //integrates cross section across photon theta
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    brem_ETheta Etheta_workspace;
    std::shared_ptr<arrays_output> out;

    brem_PTheta()
    {
        electron_energy=0.0;
        photon_energy=0.0;
        precision=1.0E6;
    }


    void reset(double _EE, double _PE)
    {
        electron_energy=_EE;
        photon_energy=_PE;
    }

    double integrate()
    {
        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, PI);

        double ret;
        auto sampler=cheby_integrator.inverse_transform(1.0, ret);
        auto sampler_out=std::make_shared<arrays_output>();
        sampler.binary_save(*sampler_out);
        out->add_array(sampler_out);

        return ret;
    }

    double operator()(double Ptheta)
    {

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

    std::shared_ptr<arrays_output> out;
    std::list<double> photon_energy_list;

    brem_PEnergy()
    {
        electron_energy=0;
        min_photon_energy=0;
        max_photon_energy=0;
        precision=1.0E7;
    }

    void reset(double _EE, double min_PE)
    {
        electron_energy=_EE;
        min_photon_energy=min_PE;
        max_photon_energy=electron_energy-electron_energy/1000.0;
    }

    double integrate()
    {
        auto PT_out=std::make_shared<arrays_output>();
        Ptheta_workspace.out=PT_out;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_photon_energy, max_photon_energy);

        out->add_doubles( make_vector(photon_energy_list) );
        out->add_array(PT_out);

        double ret;
        auto sampler=cheby_integrator.inverse_transform(1.0, ret);
        sampler.binary_save(*out);

        return ret;
    }


    double operator()(double PEnergy)
    {
        print(" ",PEcounter, PEnergy);
        PEcounter+=1;

        photon_energy_list.push_back(PEnergy);
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
    std::list<double> electron_energy_list;

    brem_EEnergy(double _min_electron_energy, double _max_electron_energy, double min_PE)
    {
        min_electron_energy=_min_electron_energy;
        max_electron_energy=_max_electron_energy;
        min_photon_energy=min_PE;
        precision=0.5E7;
    }

    void run()
    {
        arrays_output out;
        auto PE_out=std::make_shared<arrays_output>();
        Penergy_workspace.out=PE_out;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, min_electron_energy, max_electron_energy);

        out.add_doubles( make_vector(electron_energy_list) );
        out.add_array(PE_out);

        auto CS_spline=cheby_integrator.get_spline();
        CS_spline->binary_save(out);
        out.to_file("./bremsstrahlung_table");
    }


    double operator()(double EEnergy)
    {
        print(EEcounter, EEnergy);
        EEcounter+=1;

        Penergy_workspace.reset(EEnergy, min_photon_energy);

        electron_energy_list.push_back(EEnergy);
        double I=Penergy_workspace.integrate();

        return I;
    }
};

int main()
{
    double min_electron_energy=2.0/energy_units_kev;
    double max_electron_energy=50000.0/energy_units_kev;
    double min_photon_energy= 1.0/energy_units_kev;


    brem_EEnergy brem_sampler(min_electron_energy, max_electron_energy, min_photon_energy);
    brem_sampler.run();
}















