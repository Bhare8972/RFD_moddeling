#ifndef BREMSSTRAHLUNG_SCATTERING
#define BREMSSTRAHLUNG_SCATTERING

#include "bremsstrahlung_tables.hpp"

#include <cmath>

#include "constants.hpp"
#include "GSL_utils.hpp"

//#include "binary_IO.hpp"
//#include "arrays_IO.hpp"
//#include "functor.hpp"
//#include "integrate.hpp"
//#include "spline.hpp"
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
        brem_energy_sampler(double _min_photon_energy, int energy_index)
        {
            gsl::vector diff_cross(selterBerger_brem_tables::photon_reduced_energies.size());

            for( int j=0; j<selterBerger_brem_tables::photon_reduced_energies.size(); j++)
            {
                // atmosphere is hard wired in. note: ATOMIC percentage
                diff_cross[j]=diff_brem_7[energy_i][j]*0.784; //nitrogen
                diff_cross[j]+=diff_brem_8[energy_i][j]*0.211; //oxygen
                diff_cross[j]+=diff_brem_18[energy_i][j]*0.005; //argon
            }

            AM HERE
            need to integrate, sample, invert
        }
    };

}//end brem_tools namespace

class bremsstrahlung_scattering : public physical_interaction
{




public:

    double min_photon_energy;

    bremsstrahlung_scattering(double _min_photon_energy)
    {
        min_photon_energy=_min_photon_energy;
    }


};

#endif // BREMSSTRAHLUNG_SCATTERING
