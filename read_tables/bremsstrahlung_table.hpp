#ifndef BREMSSTRAHLUNG_TABLE_HPP
#define BREMSSTRAHLUNG_TABLE_HPP

#include <vector>

#include "arrays_IO.hpp"
#include "GSL_utils.hpp"
#include "gen_ex.hpp"
#include "spline.hpp"
#include "CDF_sampling.hpp"
#include "rand.hpp"
#include "span_tree.hpp"

#include "../physics/interaction_chooser.hpp"
#include "../physics/particles.hpp"

class bremsstrahlung_table : public physical_interaction
{
private:

    class PT_sampler
    {
    public:
        CDF_sampler photon_theta_sampler;

        PT_sampler(array_input& input)
        {
            auto sampler_table=input.get_array();
            photon_theta_sampler=CDF_sampler(sampler_table);
        }

        void sample(double U, double& PT)
        {
            PT=photon_theta_sampler.sample(U);
        }

    };

    class PE_sampler
    {

    public:

        span_tree<PT_sampler> photon_theta_samplers;
        CDF_sampler photon_energy_sampler;

        PE_sampler(array_input& input)
        {
            auto sampled_photon_energies=input.read_doublesArray();
            auto theta_sampler_table=input.get_array();

            for(int PTS_i=0; PTS_i<sampled_photon_energies.size(); PTS_i++)
            {
                photon_theta_samplers.insert( sampled_photon_energies[PTS_i], new PT_sampler(theta_sampler_table) );
            }

            photon_energy_sampler=CDF_sampler(input);
        }

        void sample(double U1, double U2, double& PE, double& PT)
        {
            PE=photon_energy_sampler.sample(U1);

            auto result=photon_theta_samplers.lookup(PE);
            auto theta_sampler=result.closest(PE);
            theta_sampler->sample(U2, PT);
        }
    };

public:

    std::shared_ptr<poly_spline> rate_vs_electron_energy;
    span_tree<PE_sampler> photon_energy_samplers;

    rand_threadsafe rand;

    bremsstrahlung_table()
    {
        binary_input fin("./tables/shielded_coulomb_diffusion");
        array_input table_in(fin);

        auto sampled_electron_energies=table_in.read_doublesArray();
        auto photon_sampler_table=table_in.get_array();

        for(int PES_i=0; PES_i<sampled_electron_energies.size(); PES_i++)
        {
            photon_energy_samplers.insert( sampled_electron_energies[PES_i], new PE_sampler(photon_sampler_table) );
        }

        rate_vs_electron_energy=std::make_shared<poly_spline>(table_in);
        rate_vs_electron_energy->lower_fill=0;
    }

    double rate(double energy)
    {
        double R=rate_vs_electron_energy->call(energy);
        if( R!=R ) //R is nan, energy is too high
        {
            throw gen_exception("energy too high for bremsstrahlung table:", energy);
        }
        return R;
    }

    void sample_photon_params(double initial_energy, double& PE, double& PT)
    {
            auto result=photon_energy_samplers.lookup(initial_energy);
            auto energy_sampler=result.closest(initial_energy);
            energy_sampler->sample(rand.uniform(), rand.uniform(), PE, PT);
    }

    photon_T* single_interaction(double initial_energy, electron_T *electron)
    {
        if(initial_energy< rate_vs_electron_energy->x_vals[0]) return NULL;

        //sample the distributions
        double azimuth_angle=rand.uniform()*2*PI;
        double photon_energy;
        double photon_theta;
        sample_photon_params(initial_energy, photon_energy, photon_theta);


        double final_energy=initial_energy-photon_energy;
        double final_momentum=std::sqrt((final_energy+1)*(final_energy+1)-1);

        //normalize electron momentum. Assume that direction isn't affected
        normalize(electron->momentum);

        //make new photon
        photon_T* new_photon= new photon_T;
        new_photon->position.clone_from( electron->position);
        new_photon->travel_direction.clone_from( electron->momentum); //electron momentum is normalized
        new_photon->scatter_angle(photon_theta, azimuth_angle);

        //fix electron
        electron->momentum*=final_momentum;

        return new_photon;
    }

};


#endif // BREMSSTRAHLUNG_TABLE_HPP
