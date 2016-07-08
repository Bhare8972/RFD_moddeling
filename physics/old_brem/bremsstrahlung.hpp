#ifndef BREMSSTRAHLUNG_HPP
#define BREMSSTRAHLUNG_HPP


#include <cmath>
#include <list>

#include <string>

#include "constants.hpp"
#include "GSL_utils.hpp"
#include "integrate.hpp"
#include "span_tree.hpp"
#include "rand.hpp"
#include "gen_ex.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"

#include "relativistic_formulas.hpp"
#include "interaction_chooser.hpp"
#include "particles.hpp"


namespace bremsstrahlung_utils
{
const double K_sq=std::pow(average_air_atomic_number, 2.0/3.0)/(111*111.0);
const double prefactor=average_air_atomic_number*average_air_atomic_number/(8*PI*PI*137);

double  bremsstrahlung_cross_section(double initial_energy, double photon_energy, double photon_theta, double final_electron_theta, double delta_electron_photon_phi);

class photon_energy_sampler;
class photon_theta_sampler;
class electron_theta_sampler;

class delta_phi_sampler : public functor_1D
//samples delta_phi given all other variables are set
{
public:
    std::shared_ptr<poly_spline> delta_phi_sampler;
    bool can_be_selected;

    //parents that has some variables
    photon_energy_sampler* parent_photon_energy_sampler;
        //double initial_energy;
        //double initial_electron_momentum_squared;
        //double initial_electron_momentum;

    photon_theta_sampler* parent_photon_theta_sampler;
        //double photon_energy;
        //double final_electron_energy
        //double final_electron_momentum_squared
        //double final_electron_momentum
        //double prefactor_2;

    electron_theta_sampler* parent_electron_theta_sampler;
        //double photon_theta;
        //double cos_theta_initial;
        //double sin_theta_initial;

    //data
    double final_electron_theta;


    double call(double delta_electron_photon_phi);

    double set_electron_theta(double final_electron_theta_);

    inline double sample(double U);

    virtual ~delta_phi_sampler(){}
};

class electron_theta_sampler : public functor_1D
{
public:
    span_tree<delta_phi_sampler> samplers_per_electron_theta;
    std::shared_ptr<poly_spline> electron_theta_sampler;

    //parents that has some variables
    photon_energy_sampler* parent_photon_energy_sampler;
        //double initial_energy;
        //double initial_electron_momentum_squared;
        //double initial_electron_momentum;

    photon_theta_sampler* parent_photon_theta_sampler;
        //double photon_energy;
        //double final_electron_energy
        //double final_electron_momentum_squared
        //double final_electron_momentum
        //double prefactor_2;

    //data
    double photon_theta;
    double cos_theta_initial;
    double sin_theta_initial;

    double call(double electron_theta);

    double set_photon_theta(double photon_theta_);

    inline double sample(rand_gen& rand, double& delta_phi);

    virtual ~electron_theta_sampler(){}
};

class photon_theta_sampler : public functor_1D
{
public:
    span_tree<electron_theta_sampler> samplers_per_photon_theta;
    std::shared_ptr<poly_spline> photon_theta_sampler;

    //parent that has some variables
    photon_energy_sampler* parent_photon_energy_sampler;
        //double initial_energy;
        //double initial_electron_momentum_squared;
        //double initial_electron_momentum;

    //data
    double photon_energy;
    double final_electron_energy;
    double final_electron_momentum_squared;
    double final_electron_momentum;
    double prefactor_2;

    double call(double photon_theta);

    double set_photon_energy(double photon_energy_);

    inline double sample(rand_gen& rand, double& electron_theta, double& delta_phi);

    virtual ~photon_theta_sampler(){}
};

class photon_energy_sampler : public functor_1D
{
public:
    span_tree<photon_theta_sampler> samplers_per_photon_energy;
    std::shared_ptr<poly_spline> photon_energy_sampler;

    //data
    double initial_energy;
    double initial_electron_momentum_squared;
    double initial_electron_momentum;

    double call(double photon_energy);

    double set_initial_energy(double initial_energy_);

    inline double sample(rand_gen& rand, double& photon_theta, double& electron_theta, double& delta_phi);

    virtual ~photon_energy_sampler(){}

    void output_table(arrays_output& tables_out)
    {

    }
};

#include "bremsstrahlung.cpp" //not the NORMAL way to do this. However, every other way is complicated or doesn't work
//PROBABLY shouldn't have made a pure-header project in the first place

} //bremsstrahlung_utils namespace

class bremsstrahlung_table : public functor_1D, public physical_interaction
{
public:

    span_tree<bremsstrahlung_utils::photon_energy_sampler> samplers_per_initial_energy;
    std::shared_ptr<poly_spline> interaction_rate_vs_energy;
    rand_gen rand; //consider making this thread-safe
    double min_energy;
    double max_energy;

    bremsstrahlung_table(double min_energy_, double max_energy_)
    {
        min_energy=min_energy_;
        max_energy=max_energy_;
        //gsl::vector energy_samples;
        //gsl::vector interaction_rate=adaptive_sample(this, 1.0E13 , min_energy, max_energy, energy_samples, 1);

        gsl::vector energy_samples=logspace(std::log10(min_energy), std::log10(max_energy), 2);
        gsl::vector interaction_rate=callv(energy_samples);

        interaction_rate*=2*PI; //include bit of integration due to integrating over phi of final electron
        interaction_rate_vs_energy=std::make_shared<poly_spline>(energy_samples, interaction_rate);


        arrays_output tables_out;
        std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( energy_samples );
        tables_out.add_array(interaction_energies_table);
        std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( interaction_rate );
        tables_out.add_array(interaction_rates_table);
        binary_output fout("./bremsstrahlung_table");
        tables_out.write_out( &fout);
    }

    void output_table(std::string fname="./bremsstrahlung_table")
    {
//
//        print("A");
//        arrays_output tables_out;
//
//        //output interaction rate vs energy
//        gsl::vector interaction_points;
//        gsl::vector interaction_values;
//        test_spline(interaction_rate_vs_energy, 3, interaction_points, interaction_values );
//
//        std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( interaction_points );
//        tables_out.add_array(interaction_energies_table);
//        std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( interaction_values );
//        tables_out.add_array(interaction_rates_table);
//
//        size_t num_samples=samplers_per_initial_energy.size();
//        print(num_samples);
//
//        gsl::vector energies(num_samples);
//
//        int index=0;
//
//
//        for(auto val : samplers_per_initial_energy)
//        {
//            energies[index]=val.key;
//            index++;
//        }
//
///*
//        std::shared_ptr<doubles_output> energies_table=std::make_shared<doubles_output>( energies );
//        tables_out.add_array(energies_table);
//
//        for(auto PES : sampler_list)
//        {
//            PES->output_table(tables_out);
//        }*/
//
//        binary_output fout(fname);
//        tables_out.write_out( &fout);
//
//        print("C");
    }

    double call(double energy)
    //call this to add an energy to the table
    {
        auto new_PE_sampler= new bremsstrahlung_utils::photon_energy_sampler;
        double rate_at_energy = new_PE_sampler->set_initial_energy(energy);
        //samplers_per_initial_energy.insert(energy, new_PE_sampler);
        delete new_PE_sampler;
        return rate_at_energy;
    };

    double rate(double energy)
    {
        if(energy<min_energy)
        {
            return 0.0; //assume bremsstrahlung is negligable below minimum energy
        }
        else if(energy>=max_energy)
        {
            throw gen_exception("energy: ", energy, " is above maximum energy: ", max_energy, " in bremsstrahlung.");
        }
        else
        {
            return interaction_rate_vs_energy->call(energy);
        }
    }

    double sample_electron_phi()
    {
        return rand.uniform()*2*PI;
    }

    void sample_parameters(double initial_energy, double& photon_energy, double& photon_theta, double& electron_theta, double& delta_phi)
    {
        //sample delta_phi interpolate to nearist delta_phi
        auto photon_energy_result=samplers_per_initial_energy.lookup(initial_energy);
        double delta0=std::abs( photon_energy_result.left_key - initial_energy);
        double delta1=std::abs( photon_energy_result.right_key - initial_energy);

        if(delta0<delta1)
        {
            photon_energy=photon_energy_result.left_data->sample(rand, photon_theta, electron_theta, delta_phi);
        }
        else
        {
            photon_energy=photon_energy_result.right_data->sample(rand, photon_theta, electron_theta, delta_phi);
        }
    }

    //photon_T* single_interaction(double initial_energy, electron_T *electron)

};
#endif
