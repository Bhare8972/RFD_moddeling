
#include "constants.hpp"
#include "gen_ex.hpp"
#include "chebyshev.hpp"

#include  "brems_pairPr.hpp"

class bremsstrahlung_cross_section_dPE //differential in photon energy
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    std::shared_ptr<poly_spline> double_diff_CS;
    rand_sampler theta_sampler;
    double integrated_CS;

    bremsstrahlung_cross_section_dPE(double _electron_energy, double _photon_energy, double _precision)
    {
        electron_energy=_electron_energy;
        photon_energy=_photon_energy;
        precision=_precision;

        AdaptiveSpline_Cheby_O3 cheby_integrator(un_normed_pdf, precision, 0, 3.1415926);

        double_diff_CS=cheby_integrator.get_spline();
        theta_sampler=cheby_integrator.rand_sampler(0.01, integrated_CS);
    }

    double operator()(double theta)
    {
        I AM HERE. Make units work.  (figure out how to turn to rate)
        return brems(electron_rest_energy*electron_energy, electron_rest_energy*photon_energy_joules, integrated_CS)/(2*PI*);
    }
};


class total_bremsstrahlung_cross_section
{
public:
    double electron_energy;
    double min_photon_energy;

    total_bremsstrahlung_cross_section(double _electron_energy, double _min_photon_energy)
    {
        electron_energy=_electron_energy;
        min_photon_energy=_min_photon_energy;
    }
};

class bremsstrahlung_table
{
public:
    double min_electron_energy;
    double max_electron_energy;
    double min_photon_energy;
    double precision;




    bremsstrahlung_table(double _min_electron_energy, double _max_electron_energy, double _min_photon_energy, double _precision)
    {
        min_electron_energy=_min_electron_energy;
        max_electron_energy=_max_electron_energy;
        min_photon_energy=_min_photon_energy;
        precision=_precision;

        if( not min_photon_energy<min_electron_energy)
        {
            throw gen_exception("minimum photon energy must be less than ");
        }
    }
};
