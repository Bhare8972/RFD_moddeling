#ifndef BREMSSTRAHLUNG_SCATTERING
#define BREMSSTRAHLUNG_SCATTERING

#include "bremsstrahlung_tables.hpp"

#include <cmath>
#include <list>
#include <vector>

#include "constants.hpp"
#include "GSL_utils.hpp"

#include "solve_polynomial.hpp" //the power to solve polynomials of orders 3 and 4 is NOT insignificant to the power of the dark side of the force

//#include "binary_IO.hpp"
//#include "arrays_IO.hpp"
//#include "functor.hpp"
//#include "integrate.hpp"
#include "GSL_spline.hpp"
#include "CDF_sampling.hpp"
#include "gen_ex.hpp"
#include "rand.hpp"
//#include "root_finding.hpp"

#include "interaction_chooser.hpp"
#include "particles.hpp"

namespace brem_tools
{


    class brem_energy_sampler
    {
    public:
        CDF_sampler photon_energy_sampler;
        double rate;
        double electron_energy;

        brem_energy_sampler(double _min_photon_energy, int energy_index)
        {
            gsl::vector diff_cross(selterBerger_brem_tables::photon_reduced_energies.size());
            gsl::vector all_photon_energies(selterBerger_brem_tables::photon_reduced_energies.size());
            double electron_energy=selterBerger_brem_tables::initial_energies[energy_index]/energy_units_kev;
            std::list<double> photon_energy_init;
            photon_energy_init.push_back(_min_photon_energy);

            for( int j=0; j<selterBerger_brem_tables::photon_reduced_energies.size(); j++)
            {
                all_photon_energies[j]=electron_energy*selterBerger_brem_tables::photon_reduced_energies[j];

                // atmosphere is hard wired in. note: ATOMIC percentage
                diff_cross[j]=selterBerger_brem_tables::diff_brem_7[energy_i][j]*0.784*7.0*7.0; //nitrogen
                diff_cross[j]+=selterBerger_brem_tables::diff_brem_8[energy_i][j]*0.211*8.0*8.0; //oxygen
                diff_cross[j]+=selterBerger_brem_tables::diff_brem_18[energy_i][j]*0.005*18.0*18.0; //argon
                diff_cross[j]/=all_photon_energies[j];

                if(PE>_min_photon_energy) photon_energy_init.push_back(all_photon_energies[j]);
            }

            const double millibarn_conversion_factor=1.0E31;
            diff_cross*=1.0/(2*PI*average_air_atomic_number*electron_classical_radius*millibarn_conversion_factor*KE_to_beta(electron_energy)*electron_classical_radius);
            gsl::vector photon_energy(photon_energy_init);

            //integrate
            auto diff_CS=akima_spline(all_photon_energies, diff_cross);
            auto cross_section_spline=diff_CS.integrate()'

            //sample
            gsl::vector cross_section=cross_section_spline->callv(photon_energy);
            cross_section-=cross_section[0];

            //setup sampler
            rate=cross_section.back();
            photon_energy_sampler.set(phton_energy, cross_section);

        }

        inline double sample(double uniform_rand)
        {
            return photon_energy_sampler.sample(uniform_rand);
        }
    };


CHANGE THESE TO WORK WITH PHOTON enerGY FRACTIONS, nOT FULL ENERGy

    class PA_electron_energy_inverter
    {
    public:
        gsl::vector A_values;
        gsl::vector B_values;

        PA_electron_energy_inverter(gsl::vector electron_energies, int photon_energy_index)
        {
            //first interpolate to get the A and B values for the correct Z. Assume that Z can just be average air atomic number
            gsl::vector initial_A_vector(bremsstrahlung_distribution::initial_energies.size());
            gsl::vector initial_B_vector(bremsstrahlung_distribution::initial_energies.size());

            gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline,  bremsstrahlung_distribution::charges.size());
            for(int energy_index=0; energy_index< bremsstrahlung_distribution::initial_energies.size(); energy_index++)
            {

                //first do 'A' values
                double* YA=distribution_factor_A[energy_index][photon_energy_index];
                gsl_spline_init(spline, &bremsstrahlung_distribution::charges[0], YA, bremsstrahlung_distribution::charges.size());
                initial_A_vector[energy_index]= gsl_spline_eval(spline, average_air_atomic_number, NULL);

                //first do 'B' values
                double* YB=distribution_factor_B[energy_index][photon_energy_index];
                gsl_spline_init(spline, &bremsstrahlung_distribution::charges[0], YB, bremsstrahlung_distribution::charges.size());
                initial_B_vector[energy_index]= gsl_spline_eval(spline, average_air_atomic_number, NULL);

            }
            gsl_spline_free(spline);

            //now interpolate A an B values across electron energies
            gsl_spline *Avalue_spline = gsl_spline_alloc(gsl_interp_akima,  initial_A_vector.size());
            gsl_spline *Bvalue_spline = gsl_spline_alloc(gsl_interp_akima,  initial_B_vector.size());
            gsl_spline_init(Avalue_spline,   &bremsstrahlung_distribution::initial_energies[0],   initial_A_vector,   initial_A_vector.size());
            gsl_spline_init(Bvalue_spline,   &bremsstrahlung_distribution::initial_energies[0],   initial_B_vector,   initial_B_vector.size());

            A_values= gsl::vector(electron_energies.size());
            B_values= gsl::vector(electron_energies.size());

            for(new_energy_i=0; new_energy_i<electron_energies.size(); new_energy_i++)
            {
                A_values[new_energy_i]=gsl_spline_eval(Avalue_spline, electron_energies[new_energy_i], NULL);
                B_values[new_energy_i]=gsl_spline_eval(Bvalue_spline, electron_energies[new_energy_i], NULL);
            }

            gsl_spline_free(Avalue_spline);
            gsl_spline_free(Bvalue_spline);
        }
    };

    class photon_angle_sampler
    {
    public:

        gsl::vector photon_energy_fractions;
        std::shared_ptr<poly_spline> A_spline;
        std::shared_ptr<poly_spline> B_spline;

        double electron_energy;
        double beta;
        double beta_sq;

        double CL_factor;
        double NL_factor;
        double B4;
        double B3;
        double B1;

        photon_angle_sampler(double _electron_energy, gsl::vector _photon_energy_fractions, gsl::vector _A_values, gsl::vector _B_values)
        {
            //can accelerate lookup by re-sampling photon energies to be exponential, then need to add exponential/linear acceleration to poly_spline

            photon_energy_fractions=_photon_energy_fractions;
            A_spline=akima_spline(photon_energy_fractions, _A_values);
            B_spline=akima_spline(photon_energy_fractions, _B_values);

            electron_energy=_electron_energy;
            beta=KE_to_beta(electron_energy);
            beta_sq=beta*beta;
            double beta_tri=beta_sq*beta;
            double beta_quad=beta_tri*beta;

            double beta_p1=beta+1;
            CL_factor=(beta_quad + 3*beta_tri + 4.0*beta_sq + 3.0*beta + 1)/(3.0*beta_sq*beta_p1*beta_p1*beta_p1);

            double beta_m1=beta-1.0;
            NL_factor=(beta_quad - 3*beta_tri + 4.0*beta_sq - 3.0*beta + 1)/(3.0*beta_sq*beta_m1*beta_m1*beta_m1);

            B4=3.0*beta_sq * 6.0;
            B3=beta_quad + 4.0*beta_sq + 4.0;
            B1=beta_quad + beta_sq + 1.0;
        }

        double angle_sample(double photon_energy_fraction, double uniform_rand)
        {
            double A_value=A_spline->call(photon_energy);
            double B_value=B_spline->call(photon_energy);

            double K= 4.0/3.0 - A_value*23.0/24.0;
            double L= A_value*41.0/21.0 -4.0/3.0;
            double C=L*CL_factor - K/(1.0 + beta);
            double N_bar=C-K*NL_factor + K/(1.0 - beta);
            double Q=3.0*beta_sq*(N_bar*uniform_rand - C);
            double inv_Q=1.0/Q;

            K*=beta_sq;

            double constant_term=(1.0 - 3.0*K - L*B2)*inv_Q;
            double first_term=(9.0*K - 4.0*Q + L*B3)*inv_Q;;
            double second_term=(6.0*Q-9.0*K-L*B4)*inv_Q;
            double third_term=3.0*K - 4.0*Q -3*L)*inv_Q;

// solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
// return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
// return 2: 2 real roots x[0], x[1] and complex x[2]±i*x[3],
// return 0: two pair of complex roots: x[0]±i*x[1],  x[2]±i*x[3],
            double poly_solution[4];
            int solution_type=SolveP4(poly_solution, third_term, second_term, first_term, constant_term);

            //assume that there is a correct real solution
            double U;
            if(poly_solution[0]>=-beta and poly_solution[0]<=beta) //all types have real component at zero
            {
                U=poly_solution[0]/beta;
            }
            else if(poly_solution[2]>=-beta and poly_solution[2]<=beta)
            {
                U=poly_solution[2]/beta;
            }
            else if(solution_type!=0 and poly_solution[1]>=-beta and poly_solution[1]<=beta)
            {
                U=poly_solution[1]/beta;
            }
            else if(solution_type!=0 and poly_solution[3]>=-beta and poly_solution[3]<=beta)
            {
                U=poly_solution[3]/beta;
            }
            else
            {
                throw gen_exception("cannot solve for photon angle in brem");
            }

            return std::acos(U);
        }

    };

}//end brem_tools namespace

class bremsstrahlung_scattering : public physical_interaction
{
public:
    double min_photon_energy;
    //sampling photon energy
    gsl::vector PE_initial_electron_energies;
    std::vector<brem_energy_sampler> PE_samplers;

    //sampling photon angle
    gsl::vector PA_initial_electron_energies;

    rand_threadsafe rand;

    bremsstrahlung_scattering(double _min_photon_energy)
    {
        //one way to improve this would be to sample electron_energies in an exponential space. Like we will do below for angle sampling

        //photon energy samplers
        min_photon_energy=_min_photon_energy;
        if( min_photon_energy*energy_units_kev < selterBerger_brem_tables::initial_energies[0] )
        {
            print("Bremsstrahlung warning: smallest allowed discrete photon energy is", selterBerger_brem_tables::initial_energies[0], "Kev");
            min_photon_energy = selterBerger_brem_tables::initial_energies[0]/energy_units_kev;
        }

        size_t first_electron_energy_index = search_sorted_d(selterBerger_brem_tables::initial_energies, min_photon_energy*energy_units_kev);
        first_electron_energy_index+=1; //because first_electron_energy_index cannot be smaller than min_photon_energy

        size_t num_energies= selterBerger_brem_tables::initial_energies.size()-first_electron_energy_index;
        PE_samplers.reserve(num_energies);
        PE_initial_electron_energies=gsl::vector(num_energies);

        for(size_t energy_i=first_electron_energy_index; energy_i<selterBerger_brem_tables::initial_energies.size(); energy_i++)
        {
            PE_samplers.emplace_back(min_photon_energy, energy_i);
            PE_initial_electron_energies[energy_i]=energy_samplers.back().electron_energy;
        }



CHANGE BELOW TO WORK WITH PHOTON enerGY FRACTIONS, nOT FULL ENERGy

        //set up photon angle samplers
        PA_initial_electron_energies=logspace(std::log10(_min_photon_energy*energy_units_kev), std::log10(bremsstrahlung_distribution::initial_energies.back() ), 100 ); //notice that this is in Kev!!
        //note that PA_electron_energy_inverter works in inherent units of the table (Kev)
        PA_electron_energy_inverter PA_inverter_0(PA_initial_electron_energies, 0); //technically a loop across bremsstrahlung_distribution::photon_energies. But there are only 4 of these guys(gals) presently
        PA_electron_energy_inverter PA_inverter_1(PA_initial_electron_energies, 1);
        PA_electron_energy_inverter PA_inverter_2(PA_initial_electron_energies, 2);
        PA_electron_energy_inverter PA_inverter_3(PA_initial_electron_energies, 3);

        PA_initial_electron_energies/=energy_units_kev; //convert back to dimensionless units

    }


};

#endif // BREMSSTRAHLUNG_SCATTERING
