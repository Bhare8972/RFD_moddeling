
#include "GSL_utils.hpp"
#include "constants.hpp"
#include "gen_ex.hpp"
#include "chebyshev.hpp"
#include "arrays_IO.hpp"
#include "span_tree.hpp"

#include "../physics/relativistic_formulas.hpp"
#include "../physics/brems_pairPr.hpp"
#include "../physics/bremsstrahlung_scattering.hpp"

class bremsstrahlung_cross_section_across_theta //differential in photon energy. Integrates (and samples) the theta angle
{
public:
    double electron_energy;
    double photon_energy;
    double precision;

    std::shared_ptr<poly_spline> double_diff_CS;
    //AdaptiveSpline_Cheby_O3::rand_sampler theta_sampler;
    double integrated_CS;

    bremsstrahlung_cross_section_across_theta(double _electron_energy, double _photon_energy, double _precision)
    {
        electron_energy=_electron_energy;
        photon_energy=_photon_energy;
        precision=_precision;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, 0, 3.1415926);

        //double_diff_CS=cheby_integrator.get_spline();
        auto theta_sampler=cheby_integrator.inverse_transform(0.01, integrated_CS);
    }

    double operator()(double theta)
    {
        //need to include atmospher here
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

    bremsstrahlung_cross_section_across_PE(double _electron_energy, double _min_photon_energy, double _precision)
    {
        electron_energy=_electron_energy;
        min_photon_energy=_min_photon_energy;
        precision=_precision;

        AdaptiveSpline_Cheby_O3 cheby_integrator(*this, precision, _min_photon_energy, _electron_energy);

        //diff_CS=cheby_integrator.get_spline();
        auto PE_sampler=cheby_integrator.inverse_transform(0.01, total_CS);
    }

    double operator()(double photon_energy)
    {
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

int main()
{
    double min_electron_energy=2.0/energy_units_kev;
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
    out.write_out(&fout);
}















