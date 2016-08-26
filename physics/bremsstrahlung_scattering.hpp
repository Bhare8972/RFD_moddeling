#ifndef BREMSSTRAHLUNG_SCATTERING
#define BREMSSTRAHLUNG_SCATTERING

#include "bremsstrahlung_tables.hpp"

#include <cmath>
#include <list>
#include <vector>

#include "constants.hpp"
#include "GSL_utils.hpp"

#include "solve_polynomial.hpp" //the power to solve polynomials of orders 3 and 4 is NOT insignificant to the power of the dark side of the force

#include "binary_IO.hpp"
#include "arrays_IO.hpp"
//#include "functor.hpp"
//#include "integrate.hpp"
#include "GSL_spline.hpp"
#include "CDF_sampling.hpp"
#include "chebyshev.hpp"
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
        AdaptiveSpline_Cheby_O3::rand_sampler photon_energy_sampler;
        double rate;
        double electron_energy;

        brem_energy_sampler(double _min_photon_energy, int energy_index)
        {
            electron_energy=selterBerger_brem_tables::initial_energies[energy_index]/energy_units_kev;
            double min_reduced_photon_energy=_min_photon_energy/electron_energy;
            int lowest_photon_energy_index=search_sorted_d(selterBerger_brem_tables::photon_reduced_energies, min_reduced_photon_energy);
            int num_values=selterBerger_brem_tables::photon_reduced_energies.size()-lowest_photon_energy_index;
            
            gsl::vector diff_cross(num_values);
            gsl::vector photon_energies(num_values);

            for( int j=0; j<num_values; j++)
            {
                photon_energies[j]=electron_energy*selterBerger_brem_tables::photon_reduced_energies[j+lowest_photon_energy_index];

                // atmosphere is hard wired in. note: ATOMIC percentage
                diff_cross[j]=selterBerger_brem_tables::diff_brem_7[energy_index][j+lowest_photon_energy_index]*0.784*7.0*7.0; //nitrogen
                diff_cross[j]+=selterBerger_brem_tables::diff_brem_8[energy_index][j+lowest_photon_energy_index]*0.211*8.0*8.0; //oxygen
                diff_cross[j]+=selterBerger_brem_tables::diff_brem_18[energy_index][j+lowest_photon_energy_index]*0.005*18.0*18.0; //argon
                diff_cross[j]/=photon_energies[j];
            }

            const double millibarn_conversion_factor=1.0E31;

//for(auto v : diff_cross ) {print("A",v);}
            diff_cross*=1.0/(2*PI*average_air_atomic_number*electron_classical_radius*millibarn_conversion_factor*KE_to_beta(electron_energy)*electron_classical_radius);

//for(auto v : diff_cross ) {print("B",v);}

            //integrate
//print("A");
            auto diff_CS=akima_spline(photon_energies, diff_cross);
            
            
            AdaptiveSpline_Cheby_O3 cheby_sampler(*diff_CS,  10E4, photon_energies[0], photon_energies[photon_energies.size()-1] );
            photon_energy_sampler=cheby_sampler.inverse_transform(10E4, rate);
            
            /*
            auto cross_section_spline=diff_CS->integrate();

//gsl::vector TMP=diff_CS->callv(photon_energies);
//for(auto v : TMP ) {print("a",v);}
 
            
auto X=linspace(photon_energies[0], photon_energies[photon_energies.size()-2], 10000);
            
arrays_output tables_out;
std::shared_ptr<doubles_output> X_table=std::make_shared<doubles_output>(X);
tables_out.add_array(X_table);

std::shared_ptr<doubles_output> diffCS_table=std::make_shared<doubles_output>(diff_CS->callv(X));
tables_out.add_array(diffCS_table);

std::shared_ptr<doubles_output> CS_table=std::make_shared<doubles_output>(cross_section_spline->callv(X));
tables_out.add_array(CS_table);


binary_output fout("./out_arg");
tables_out.write_out( &fout);
fout.flush();
print("out_arg");

            //sample
            photon_energies[0]=_min_photon_energy; //is too small initially
            gsl::vector cross_section=cross_section_spline->callv(photon_energies);
for(auto v : cross_section ) {print("a",v);}
            cross_section-=cross_section[0];
for(auto v : cross_section ) {print("b",v);}

            //setup sampler
            rate=cross_section.back();
            photon_energy_sampler.set(photon_energies, cross_section);*/

        }

        inline double sample(double uniform_rand)
        {
            return photon_energy_sampler.sample(uniform_rand);
        }
    };

    class PA_electron_energy_inverter
    {
    public:
        gsl::vector A_values;
        gsl::vector B_values;

        PA_electron_energy_inverter(gsl::vector electron_energies_kev, int photon_energy_index)
        {
            //first interpolate to get the A and B values for the correct Z. Assume that Z can just be average air atomic number
            gsl::vector initial_A_vector(bremsstrahlung_distribution::initial_energies.size());
            gsl::vector initial_B_vector(bremsstrahlung_distribution::initial_energies.size());

            gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline,  bremsstrahlung_distribution::charges.size());
            for(int energy_index=0; energy_index< bremsstrahlung_distribution::initial_energies.size(); energy_index++)
            {

                //first do 'A' values
                double* YA=bremsstrahlung_distribution::distribution_factor_A[energy_index][photon_energy_index];
                gsl_spline_init(spline, &bremsstrahlung_distribution::charges[0], YA, bremsstrahlung_distribution::charges.size());
                initial_A_vector[energy_index]= gsl_spline_eval(spline, average_air_atomic_number, NULL);

                //first do 'B' values
                double* YB=bremsstrahlung_distribution::distribution_factor_B[energy_index][photon_energy_index];
                gsl_spline_init(spline, &bremsstrahlung_distribution::charges[0], YB, bremsstrahlung_distribution::charges.size());
                initial_B_vector[energy_index]= gsl_spline_eval(spline, average_air_atomic_number, NULL);

            }
            gsl_spline_free(spline);

            //now interpolate A an B values across electron energies
            gsl_spline *Avalue_spline = gsl_spline_alloc(gsl_interp_cspline,  initial_A_vector.size());
            gsl_spline *Bvalue_spline = gsl_spline_alloc(gsl_interp_cspline,  initial_B_vector.size());
            gsl_spline_init(Avalue_spline,   &bremsstrahlung_distribution::initial_energies[0],   &initial_A_vector[0],   initial_A_vector.size());
            gsl_spline_init(Bvalue_spline,   &bremsstrahlung_distribution::initial_energies[0],   &initial_B_vector[0],   initial_B_vector.size());

            A_values= gsl::vector(electron_energies_kev.size());
            B_values= gsl::vector(electron_energies_kev.size());

            for(int new_energy_i=0; new_energy_i<electron_energies_kev.size(); new_energy_i++)
            {
                A_values[new_energy_i]=gsl_spline_eval(Avalue_spline, electron_energies_kev[new_energy_i], NULL);
                B_values[new_energy_i]=gsl_spline_eval(Bvalue_spline, electron_energies_kev[new_energy_i], NULL);
            }

            gsl_spline_free(Avalue_spline);
            gsl_spline_free(Bvalue_spline);
        }
    };

    class photon_angle_sampler
    {
    public:

//        gsl::vector photon_energy_fractions;
        std::shared_ptr<poly_spline> A_spline;
        std::shared_ptr<poly_spline> B_spline;

//        double electron_energy;
//        double beta;
//        double beta_sq;
//
//        double CL_factor;
//        double NL_factor;
//        double B4;
//        double B3;
//        double B1;

        //photon_angle_sampler(double electron_energy, gsl::vector _photon_energy_fractions, gsl::vector _A_values, gsl::vector _B_values)
        photon_angle_sampler(gsl::vector photon_reduced_energies, gsl::vector A_values, gsl::vector B_values)
        {
            //can accelerate lookup by re-sampling photon energies to be exponential, then need to add exponential/linear acceleration to poly_spline

            //photon_energy_fractions=_photon_energy_fractions;
            A_spline=natural_cubic_spline(photon_reduced_energies, A_values);
            B_spline=natural_cubic_spline(photon_reduced_energies, B_values);

            //electron_energy=_electron_energy;
//            beta=KE_to_beta(electron_energy);
//            beta_sq=beta*beta;
//            double beta_tri=beta_sq*beta;
//            double beta_quad=beta_tri*beta;
//
//            double beta_p1=beta+1;
//            CL_factor=(beta_quad + 3*beta_tri + 4.0*beta_sq + 3.0*beta + 1)/(3.0*beta_sq*beta_p1*beta_p1*beta_p1);
//
//            double beta_m1=beta-1.0;
//            NL_factor=(beta_quad - 3*beta_tri + 4.0*beta_sq - 3.0*beta + 1)/(3.0*beta_sq*beta_m1*beta_m1*beta_m1);
//
//            B4=3.0*beta_sq * 6.0;
//            B3=beta_quad + 4.0*beta_sq + 4.0;
//            B1=beta_quad + beta_sq + 1.0;
        }

        void find_parameters(double photon_reduced_energy, double& return_A_value, double& return_B_value)
        {
            return_A_value=A_spline->call(photon_reduced_energy);
            return_B_value=B_spline->call(photon_reduced_energy);
        }

//            double K= 4.0/3.0 - A_value*23.0/24.0;
//            double L= A_value*41.0/21.0 -4.0/3.0;
//            double C=L*CL_factor - K/(1.0 + beta);
//            double N_bar=C-K*NL_factor + K/(1.0 - beta);
//            double Q=3.0*beta_sq*(N_bar*uniform_rand - C);
//            double inv_Q=1.0/Q;
//
//            K*=beta_sq;
//
//            double constant_term=(1.0 - 3.0*K - L*B2)*inv_Q;
//            double first_term=(9.0*K - 4.0*Q + L*B3)*inv_Q;;
//            double second_term=(6.0*Q-9.0*K-L*B4)*inv_Q;
//            double third_term=3.0*K - 4.0*Q -3*L)*inv_Q;
//
//// solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
//// return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
//// return 2: 2 real roots x[0], x[1] and complex x[2]±i*x[3],
//// return 0: two pair of complex roots: x[0]±i*x[1],  x[2]±i*x[3],
//            double poly_solution[4];
//            int solution_type=SolveP4(poly_solution, third_term, second_term, first_term, constant_term);
//
//            //assume that there is a correct real solution
//            double U;
//            if(poly_solution[0]>=-beta and poly_solution[0]<=beta) //all types have real component at zero
//            {
//                U=poly_solution[0]/beta;
//            }
//            else if(poly_solution[2]>=-beta and poly_solution[2]<=beta)
//            {
//                U=poly_solution[2]/beta;
//            }
//            else if(solution_type!=0 and poly_solution[1]>=-beta and poly_solution[1]<=beta)
//            {
//                U=poly_solution[1]/beta;
//            }
//            else if(solution_type!=0 and poly_solution[3]>=-beta and poly_solution[3]<=beta)
//            {
//                U=poly_solution[3]/beta;
//            }
//            else
//            {
//                throw gen_exception("cannot solve for photon angle in brem");
//            }
//
//            return std::acos(U);
        //}

    };

}//end brem_tools namespace

class bremsstrahlung_scattering : public physical_interaction
{
public:
    double min_photon_energy;
    //sampling photon energy
    gsl::vector PE_initial_electron_energies;
    std::vector<brem_tools::brem_energy_sampler> PE_samplers;
    size_t PE_index;

    //sampling photon angle
    gsl::vector PA_initial_electron_energies;
    std::vector<brem_tools::photon_angle_sampler> PA_param_samplers;

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
            PE_initial_electron_energies[energy_i]=PE_samplers.back().electron_energy;
        }




        //set up photon angle samplers
        PA_initial_electron_energies=logspace(std::log10(_min_photon_energy*energy_units_kev), std::log10(bremsstrahlung_distribution::initial_energies.back() ), 100 ); //notice that this is in Kev!!
        //note that PA_electron_energy_inverter works in inherent units of the table (Kev)
        brem_tools::PA_electron_energy_inverter PA_inverter_0(PA_initial_electron_energies, 0); //technically a loop across bremsstrahlung_distribution::photon_energies. But there are only 4 of these guys(gals) presently
        brem_tools::PA_electron_energy_inverter PA_inverter_1(PA_initial_electron_energies, 1);
        brem_tools::PA_electron_energy_inverter PA_inverter_2(PA_initial_electron_energies, 2);
        brem_tools::PA_electron_energy_inverter PA_inverter_3(PA_initial_electron_energies, 3);

        PA_initial_electron_energies/=energy_units_kev; //convert back to dimensionless units
        PA_param_samplers.reserve(PA_initial_electron_energies.size());
        for(size_t initial_energy_i=0; initial_energy_i<PA_initial_electron_energies.size(); initial_energy_i++)
        {
            gsl::vector energy_A_values(4);
            gsl::vector energy_B_values(4);

            //technically should be a loop.but again, there are only 4
            energy_A_values[0]=PA_inverter_0.A_values[initial_energy_i];
            energy_B_values[0]=PA_inverter_0.B_values[initial_energy_i];
            energy_A_values[1]=PA_inverter_1.A_values[initial_energy_i];
            energy_B_values[1]=PA_inverter_1.B_values[initial_energy_i];
            energy_A_values[2]=PA_inverter_2.A_values[initial_energy_i];
            energy_B_values[2]=PA_inverter_2.B_values[initial_energy_i];
            energy_A_values[3]=PA_inverter_3.A_values[initial_energy_i];
            energy_B_values[3]=PA_inverter_3.B_values[initial_energy_i];

            PA_param_samplers.emplace_back(bremsstrahlung_distribution::photon_reduced_energies, energy_A_values, energy_B_values);
        }
        PE_index=0;
    }

    double rate(double initial_electron_energy)
    {
        if(initial_electron_energy<PE_initial_electron_energies[0]) return 0;

        PE_index = search_sorted_d(PE_initial_electron_energies, initial_electron_energy); //make this perfect exponential for faster sampler

        double low_value=PE_samplers[PE_index].rate;
        double high_value=PE_samplers[PE_index+1].rate;

        return linear_interpolate(PE_initial_electron_energies[PE_index],low_value,     PE_initial_electron_energies[PE_index+1],high_value,     initial_electron_energy);
    }

    double sample_photon_energy(double initial_electron_energy)
    {
        //assume that initial_electron_energy isn't too large
        if(initial_electron_energy<PE_initial_electron_energies[0]) return 0;

        if(not( initial_electron_energy>=PE_initial_electron_energies[PE_index] and initial_electron_energy<PE_initial_electron_energies[PE_index+1] ))
        {
            PE_index = search_sorted_d(PE_initial_electron_energies, initial_electron_energy); //make this perfect exponential for faster sampler
        }

        double R=rand.uniform();
        double low_value=PE_samplers[PE_index].sample(R);
        double high_value=PE_samplers[PE_index+1].sample(R);

        return linear_interpolate(PE_initial_electron_energies[PE_index],low_value,     PE_initial_electron_energies[PE_index+1],high_value,     initial_electron_energy);
    }

    double sample_photon_angle(double initial_electron_energy, double photon_energy)
    {
        size_t sampler_index;
        if(initial_electron_energy >= PA_initial_electron_energies.back())
        {
            sampler_index=PA_initial_electron_energies.size()-1; //assume that A and B parameters do not change significantly if energy is too high....for now
        }
        else
        {
            sampler_index=search_sorted_exponential(PA_initial_electron_energies, initial_electron_energy);
        }
        double reduced_photon_energy=photon_energy/initial_electron_energy;

        double A_low;
        double B_low;
        double A_high;
        double B_high;
        PA_param_samplers[sampler_index].find_parameters(reduced_photon_energy, A_low, B_low);
        PA_param_samplers[sampler_index+1].find_parameters(reduced_photon_energy, A_high, B_high);

        double A_param = linear_interpolate(PA_initial_electron_energies[sampler_index],A_low,     PA_initial_electron_energies[sampler_index+1],A_high,     initial_electron_energy);
        double B_param = linear_interpolate(PA_initial_electron_energies[sampler_index],B_low,     PA_initial_electron_energies[sampler_index+1],B_high,     initial_electron_energy);

        //maybe turn the following equation into a spline?....entire point is to avoid excess splining
        double beta_prime=KE_to_beta(initial_electron_energy)*(1.0-B_param);
        double beta_pSQ=beta_prime*beta_prime;
        double beta_pTRI=beta_pSQ*beta_prime;
        double beta_pFOURTH=beta_pTRI*beta_prime;

        double R=rand.uniform();

        double K=4.0/3.0 - A_param*23.0/24.0;
        double L=A_param*41.0/21.0 - 4.0/3.0;
        double B1=beta_pFOURTH + beta_pSQ +1;
        double B2= 3.0*(beta_pSQ + 1);
        double betap1=beta_prime+1;
        double nbetam1=1 - beta_prime;
        double C=L*(B1 + 3*beta_prime*(beta_pSQ + beta_prime +1.0))/(3.0*beta_pSQ*betap1*betap1*betap1)  - K/betap1;
        double N_bar=L*(B1 - B2*beta_prime + 3.0*beta_pSQ)/(3*beta_pSQ*nbetam1*nbetam1*nbetam1) + C + K/nbetam1;
        double Q=3*beta_pSQ*(N_bar*R - C);

        double zeroth_term=3.0*beta_pSQ*K - Q + L*B1;
        double first_term=3.0*Q - 6.0*beta_pSQ*K - L*B2;
        double second_term=3.0*beta_pSQ*K + 3*L -3*Q;

        zeroth_term/=Q;
        first_term/=Q;
        second_term/=Q;

// solve cubic equation x^3 + a*x^2 + b*x + c
// x - array of size 3
// In case 3 real roots: => x[0], x[1], x[2], return 3
//         2 real roots: x[0], x[1],          return 2
//         1 real root : x[0], x[1] ± i*x[2], return 1
        double output[3];
        int solution_type= SolveP3(output, second_term, first_term, zeroth_term);

        //assume that there is a correct real solution
        double U;
        if(output[0]>=-beta_prime and output[0]<=beta_prime) //all types have real component at zero
        {
            U=output[0]/beta_prime;
        }
        else if(output[1]>=-beta_prime and output[1]<=beta_prime)
        {
            U=output[1]/beta_prime;
        }
        else if(solution_type==3 and output[2]>=-beta_prime and output[2]<=beta_prime)
        {
            U=output[2]/beta_prime;
        }
        else
        {
            throw gen_exception("cannot solve for photon angle in brem");
        }

        return std::acos(U);
    }

    //eventually return photon
    photon_T* single_interaction(double initial_energy, electron_T *electron)
    {
        if(initial_energy< min_photon_energy) return NULL;

        //sample the distributions
        double azimuth_angle=rand.uniform()*2*PI;
        double photon_energy=sample_photon_energy(initial_energy);
        double photon_angle=sample_photon_angle(initial_energy, photon_energy);
        
        double final_energy=initial_energy-photon_energy;
        double final_momentum=std::sqrt((final_energy+1)*(final_energy+1)-1);

        //normalize electron momentum. Assume that direction isn't affected
        normalize(electron->momentum);
        
        //make new photon
        photon_T* new_photon= new photon_T;
        new_photon->position.clone_from( electron->position);
        new_photon->travel_direction.clone_from( electron->momentum); //electron momentum is normalized
        new_photon->scatter_angle(photon_angle, azimuth_angle);
        
        //fix electron
        electron->momentum*=final_momentum;
        
        return new_photon;
    }
};

#endif // BREMSSTRAHLUNG_SCATTERING
