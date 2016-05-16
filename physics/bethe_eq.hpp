
#ifndef BETH_EQUATION_HPP
#define BETH_EQUATION_HPP

#include <cmath>


#include "vector.hpp"
#include "constants.hpp"

#include "binary_IO.hpp"
#include "arrays_IO.hpp"

////equations
double bethe_formula(double mom_sq)
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double exp_term1=beta_sq*KE*gamma_sq*inv_I_sq;
    double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
    double term3=KE*KE/(8.0*gamma_sq) + 1.0/gamma_sq;

    return (std::log(exp_term1)-term2_factor*std::log(2)+term3)/beta_sq;
}

double moller_losses(double mom_sq, double min_energy)
//energy loss due to moller scattering for a particle with energy 'KE' that frees electrons to have energy at least 'min_energy', where KE must be greater than 2*min_energy
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double term_1=std::log(KE/(2.0*min_energy));
    double term_2=min_energy/(KE-min_energy);
    double term_3_factor1=1+(2/gamma)-1.0/gamma_sq;
    double term_3_factor2=log(2*(KE-min_energy)/KE);
    double term_4=KE*KE/(8*gamma_sq);
    double term_5=min_energy*min_energy/(2*gamma_sq);
    return (term_1 - term_2 - term_3_factor1*term_3_factor2 + term_4 - term_5 +1)/beta_sq;
}

double bethe_subtract_moller(double mom_sq, double minimum_energy)
{
    double gamma_sq=1.0+mom_sq;
    double gamma=std::sqrt(gamma_sq);
    double beta_sq=mom_sq/gamma;
    double KE=gamma-1;

    double exp_term1=2*minimum_energy*beta_sq*gamma_sq*inv_I_sq;
    double term2_factor=1+(2.0/gamma)-1.0/gamma_sq;
    double exp_term2_factor2=KE/(KE-minimum_energy);
    double term3=minimum_energy/(KE-minimum_energy)-beta_sq;
    double term4=minimum_energy*minimum_energy/(2*gamma_sq);

    return (std::log(exp_term1)-term2_factor*std::log(exp_term2_factor2)+term3+term4)/beta_sq;
}

//add bhaba_losses here

////tables
namespace bethe_table{

double density=1.205E-3; // g cm^-3
double air_molecular_density=2.688E25; // m^-3
double electron_classical_radius=2.8179E-15;

//energy of electrons in keV
//gsl::vector electron_energy({ 0.02, 0.04, 0.06, 0.1, 0.2, 0.4, 0.6, 1, 2,
//4, 6, 10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0,
//60.0, 70.0, 80.0, 90.0, 100, 125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
//550, 600, 700, 800, 900, 1000, 1250, 1500, 1750, 2000, 2500, 3000, 3500,
//4000, 4500, 5000, 5500, 6000, 7000, 8000, 9000, 10000, 12500, 15000, 17500,
//20000, 25000, 30000, 35000});

gsl::vector electron_energy({
0.0105017788, 0.0108428395, 0.0110485221, 0.0112077337, 0.0114219641, 0.011639461, 0.0118585668,
0.0121889084, 0.0124644763, 0.0127229898, 0.0129925858, 0.0132927717, 0.013602637, 0.0139941002,
0.0144500295, 0.0149019025, 0.0153628296, 0.0159076487, 0.0164161624, 0.016833719, 0.017207197,
0.0176079697, 0.0185464884, 0.0193591398, 0.020128537 , 0.0207851995, 0.021368771, 0.0219671635,
0.0225855278, 0.023324006 , 0.0238681697, 0.0245382997, 0.0252254488, 0.026167326, 0.0271424401,
0.0282844541, 0.0293384616, 0.0307059101, 0.0321370946, 0.0334797241, 0.035195176, 0.0369958921,
0.0388832034, 0.0404961412, 0.0429424048, 0.0449247343, 0.0478424098, 0.0507324419, 0.054023458,
0.05882405, 0.0631908064, 0.067563569, 0.0735359389, 0.0800248508, 0.0859164929, 0.092643282,
0.0998896312, 0.108649879, 0.1176496214, 0.1227477194, 0.1275653721, 0.1320767768, 0.1357984782,
0.1397583326, 0.1438922781, 0.1482899207, 0.1526480294, 0.165968767, 0.1796524264, 0.1987888754,
0.2151784607, 0.2339162138, 0.2509411853, 0.2703959616, 0.2926268553, 0.3166629446, 0.3381904309,
0.3643575256, 0.3942574387, 0.426610999, 0.4575956009, 0.4951468185, 0.533382294, 0.5746112313,
0.6216764781, 0.6696826348, 0.7245865634, 0.7736804181, 0.8333057527, 0.9015601459, 0.9668336493,
1.0202113581, 1.0795497173, 1.1414921791, 1.2182841366, 1.3069844101, 1.3838050243, 1.4615077172,
1.5445778846, 1.6251128802, 1.7226624996, 1.8386097134, 1.9882205004, 2.1412541075, 2.3084638322,
2.4692114892, 2.6381905421, 2.8004975986, 2.9619290742, 3.1555278221, 3.3618092746, 3.6099251038,
3.8686049608, 4.2121704726, 4.5678467852, 4.9923926896, 5.3604064207, 5.7329330926, 6.1072570885,
6.4804580801, 6.9031451416, 7.412488196, 7.9599264485, 8.4463399727, 8.962477153, 9.5464374364,
10.1297991429,
12.5, 15.0, 17.5, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0,
60.0, 70.0, 80.0, 90.0, 100, 125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
550, 600, 700, 800, 900, 1000, 1250, 1500, 1750, 2000, 2500, 3000, 3500,
4000, 4500, 5000, 5500, 6000, 7000, 8000, 9000, 10000, 12500, 15000, 17500,
20000, 25000, 30000, 35000});

//stopping power of electrons for energies in electron_energy, in units of MeV cm^2 g^-1
gsl::vector electron_SP({
10.2181796366, 10.9082117296, 11.6981237233, 12.4911033579, 13.5136247561, 14.5558880806,
15.4736566794, 16.447311695, 16.9967274122, 17.5553802666, 18.0344911759, 18.4716847744,
18.8503919913, 19.1000018782, 19.0967213746, 18.9782769685, 18.9495008892, 19.0228172633,
19.30633574, 19.709533512, 20.2026842843, 20.7379467215, 22.1323483477, 23.6234223952, 25.6625487172,
27.6368292094, 29.7647873415, 31.9163450993, 34.5248597244, 37.3443196403, 39.6965404084,
42.7530623898, 45.843482019, 49.8021721698, 53.866005709, 59.0293120686, 63.8460758306, 69.6557248612,
75.9940206633, 81.8305683359, 89.27133565, 96.9626098896, 104.3970336012, 110.4606257451, 119.438208086,
126.9231024547, 136.0322607091, 147.0881533704, 156.9548466353, 169.6499210824, 180.2163598329, 188.1239554748,
198.0604795365, 206.7012717496, 212.0069466433, 216.4844424108, 220.0893817056, 222.7486113708, 224.4671852355,
224.8054363918, 225.1159671422, 225.268326864, 225.1829627735, 225.1621566595, 225.0052248002, 224.9128565035,
224.2783292475, 221.4651640238, 218.3344529607, 214.2418026202, 211.213203587, 206.3969950786, 201.7270329863,
197.1508665324, 191.8239974198, 185.8245041823, 180.8363172122, 175.1910199546, 168.9692536166, 162.968448241,
157.8904487025, 152.283098057, 146.2411146397, 141.0559709291, 134.8586839237, 129.5080314721, 124.3621855211,
119.4423704105, 113.7019195197, 108.7064313905, 103.9429323027, 100.6384975781, 96.9250617102, 93.7572721446,
89.8331355069, 85.7256043451, 82.3985166712, 79.2788056617, 76.4012882447, 73.7477460764, 70.8069236555,
67.5433219855, 64.0048359273, 60.7825299106, 57.6610937507, 55.1426388696, 52.5203880584, 50.1212927087,
48.2414184852, 46.136903342, 44.1473487117, 41.9306913146, 39.8264200543, 37.5199754307, 35.3083915974, 32.8296443645,
31.0218293829, 29.4319417068, 28.0363005705, 26.8146901457, 25.4418094538, 24.1365838331, 22.9895350961, 21.9878246291,
21.0297611457, 19.8738941735, 19.0079398282,
16.63, 14.45, 12.83, 11.57, 9.753, 8.492, 7.563, 6.848,
6.281, 5.819, 5.435, 5.111, 4.593, 4.198, 3.886, 3.633, 3.172, 2.861, 2.637,
2.470, 2.236, 2.084, 1.978, 1.902, 1.845, 1.802, 1.769, 1.743, 1.706, 1.683,
1.669, 1.661, 1.655, 1.661, 1.672, 1.684, 1.712, 1.740, 1.766, 1.790, 1.812,
1.833, 1.852, 1.870, 1.902, 1.931, 1.956, 1.979, 2.029, 2.069, 2.104, 2.134,
2.185, 2.226, 2.257});

//gsl::vector electron_SP({26.0, 104, 170, 218, 216, 170, 139, 97.9, 65.8,
//40.5, 30.1, 20.4, 16.63, 14.45, 12.83, 11.57, 9.753, 8.492, 7.563, 6.848,
//6.281, 5.819, 5.435, 5.111, 4.593, 4.198, 3.886, 3.633, 3.172, 2.861, 2.637,
//2.470, 2.236, 2.084, 1.978, 1.902, 1.845, 1.802, 1.769, 1.743, 1.706, 1.683,
//1.669, 1.661, 1.655, 1.661, 1.672, 1.684, 1.712, 1.740, 1.766, 1.790, 1.812,
//1.833, 1.852, 1.870, 1.902, 1.931, 1.956, 1.979, 2.029, 2.069, 2.104, 2.134,
//2.185, 2.226, 2.257});

//energy of positrons in keV
gsl::vector positron_energy({10, 12.5, 15.0, 17.5, 20.0, 25.0, 30.0,
35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 70.0, 80.0, 90.0, 100.0, 125.0, 150.0,
175.0, 200.0, 250.0, 300.0, 350.0, 400.0, 450.0, 500.0, 550.0, 600.0, 700.0,
800.0, 900.0, 1000.0, 1250.0, 1500.0, 1750.0, 2000.0, 2500.0, 3000.0, 3500.0,
4000.0, 4500.0, 5000.0, 5500.0, 6000.0, 7000.0, 8000.0, 9000.0, 10000.0,
12500.0, 15000.0, 17500.0, 20000.0, 25000.0, 30000.0, 35000.0});

//stopping power of position for energies in positron_energy, in units of MeV cm^2 g^-1
gsl::vector positron_SP({21.8, 18.25, 15.78, 13.96, 12.56, 10.53, 9.126,
8.096, 7.306, 6.680, 6.171, 5.749, 5.393, 4.826, 4.393, 4.052, 3.776, 3.274,
2.935, 2.692, 2.510, 2.257, 2.091, 1.976, 1.893, 1.831, 1.784, 1.748, 1.719,
1.678, 1.652, 1.636, 1.626, 1.617, 1.621, 1.630, 1.642, 1.668, 1.695, 1.720,
1.744, 1.766, 1.786, 1.805, 1.823, 1.854, 1.883, 1.908, 1.931, 1.980, 2.020,
2.055, 2.085, 2.136, 2.176, 2.208});

//positrons not implemented

}


class electron_ionization_table
//class to calculate stopping powers for electrons
// has three modes:
//    1) do not remove moller losses: use default constructor and electron_lookup
//    2) remove moller losses, and minimum energy is constant:  use non_default constructor and electron_lookup
//    3) remove moller losses, and minimum energy is variable:  use default constructor and electron_lookup_variable_RML
{
public:
    const size_t table_size=100;

    gsl::vector electron_mom_sq;
    gsl::vector electron_interp_powers;
    gsl::vector electron_interp_factors;
    //gsl::vector electron_stopping_power;
    bool moller_removed;
    double min_mom_sq_for_moller;
    double min_energy;

    electron_ionization_table( bool save_output_table=false)
    //use this constructor if minimum energy is a variable
    {
        set_tables(save_output_table);
        min_mom_sq_for_moller=0.0;
        moller_removed=false;
        min_energy=0.0;
    }

    electron_ionization_table(double const_min_energy_dimensionless, bool save_output_table=false)
    //use this if the minimum energy is a constant or not removing moller losses. const_min_energy_dimensionless should be negative is not removing moller losses
    {
        set_tables(save_output_table, const_min_energy_dimensionless);

        //for(size_t i=0; i<bethe_table::electron_energy.size(); i++)
       // {
           // if(bethe_table::electron_energy[i]>(const_min_energy_dimensionless*2.0*energy_units_kev))
          //  {
          //      electron_stopping_power[i]-=moller_losses(electron_mom_sq[i], const_min_energy_dimensionless);
          //  }
      //  }

        min_mom_sq_for_moller=const_min_energy_dimensionless*2+1;
        min_mom_sq_for_moller*=min_mom_sq_for_moller;
        min_mom_sq_for_moller-=1.0;
        min_energy=const_min_energy_dimensionless;
        moller_removed=true;
    }

    void set_tables(bool save_output_table, double const_min_energy_dimensionless=-1)
    //if const_min_energy_dimensionless is positive, remove moller losses
    {
        double min_moller_removal_mom_sq=(const_min_energy_dimensionless*2+1)*(const_min_energy_dimensionless*2+1) -1;//convert min energy to min momentum sqaured

        ///// convert electron tables
        //convert electron energy to electron momentum_sq
        gsl::vector raw_electron_mom_sq=bethe_table::electron_energy.clone();
        raw_electron_mom_sq/=energy_units_kev;
        raw_electron_mom_sq+=1.0; //now gamma
        raw_electron_mom_sq*=raw_electron_mom_sq; //square
        raw_electron_mom_sq-=1.0; //subtract one

        //convert electron stopping power
        double conversion_factor=elementary_charge*1.0E8*bethe_table::density/(2.0*3.1415926*average_air_atomic_number*bethe_table::electron_classical_radius*electron_rest_energy*
                                                                                                    bethe_table::electron_classical_radius*bethe_table::air_molecular_density);
        gsl::vector raw_electron_stopping_power=bethe_table::electron_SP*conversion_factor; //multiplication makes a new vector

        if(save_output_table)
        {
            arrays_output output_table;

            std::shared_ptr<doubles_output> electron_mom_sq_raw=std::make_shared<doubles_output>(raw_electron_mom_sq);
            std::shared_ptr<doubles_output> electron_SP_raw=std::make_shared<doubles_output>(raw_electron_stopping_power);

            output_table.add_array(electron_mom_sq_raw);
            output_table.add_array(electron_SP_raw);
            binary_output fout("./tables/bethe_info");
            output_table.write_out(&fout);
        }

        ////// re-interpolation electron table!!
        //first, we need a linear log-log interpolant of the raw table
        gsl::vector powers(raw_electron_stopping_power.size()-1);
        gsl::vector factors(raw_electron_stopping_power.size()-1);
        for(size_t i=0; i<(raw_electron_stopping_power.size()-1); i++)
        {
            powers[i]=std::log(raw_electron_stopping_power[i+1]/raw_electron_stopping_power[i]) / std::log(raw_electron_mom_sq[i+1]/raw_electron_mom_sq[i]);
            factors[i]=raw_electron_stopping_power[i]/std::pow(raw_electron_mom_sq[i], powers[i]);
        }


        //now we use the previous interpolant to sample the energies at a predetermined number of points, with perfect log density
        electron_mom_sq =logspace( std::log10(raw_electron_mom_sq[0]), std::log10(raw_electron_mom_sq[raw_electron_mom_sq.size()-1]), table_size );
        gsl::vector electron_stopping_power(table_size);

        //get zeroth bit
        electron_stopping_power[0] = raw_electron_stopping_power[0];
        if(const_min_energy_dimensionless>0 and electron_mom_sq[0]> min_moller_removal_mom_sq )
        {
            electron_stopping_power[0]-=moller_losses(electron_mom_sq[0], const_min_energy_dimensionless);
        }

        //do bits larger than zero, except last one
        for(size_t i=1; i<table_size-1; i++)
        {
            size_t loc=search_sorted_d(raw_electron_mom_sq, electron_mom_sq[i]);
            electron_stopping_power[i] = factors[loc] * std::pow(electron_mom_sq[i], powers[loc]);
            if(const_min_energy_dimensionless>0 and electron_mom_sq[i]> min_moller_removal_mom_sq )
            {
                electron_stopping_power[i]-=moller_losses(electron_mom_sq[i], const_min_energy_dimensionless);
            }
        }

        //get last bit
        electron_stopping_power[table_size-1]=raw_electron_stopping_power[raw_electron_stopping_power.size()-1];
        if(const_min_energy_dimensionless>0 and electron_mom_sq[table_size-1]> min_moller_removal_mom_sq )
        {
            electron_stopping_power[table_size-1]-=moller_losses(electron_mom_sq[table_size-1], const_min_energy_dimensionless);
        }

        //finally, we interpolate the previous samples
        electron_interp_powers=gsl::vector(table_size-1);
        electron_interp_factors=gsl::vector(table_size-1);
        for(size_t i=0; i<(table_size-1); i++)
        {
            electron_interp_powers[i]=std::log(electron_stopping_power[i+1]/electron_stopping_power[i]) / std::log(electron_mom_sq[i+1]/electron_mom_sq[i]);
            electron_interp_factors[i]=electron_stopping_power[i]/std::pow(electron_mom_sq[i], electron_interp_powers[i]);
        }
        //now have interpolants of electron stopping power that is in log space and interpolants are linear in log-log
        //which has the extra bennifit that they all(namly the first) intercept (0,0)
    }

    double electron_lookup(double electron_mom_sq_)
    //give stopping power. Use if minimum energy is a constant, or not subtracting moller losses
	{
        if(electron_mom_sq_<electron_mom_sq[0])
        {
            return electron_interp_factors[0]*std::pow(electron_mom_sq_, electron_interp_powers[0]); //use first interpolant to extrapolate to very low energy
        }
        else if(electron_mom_sq_>electron_mom_sq[electron_mom_sq.size()-1])
        {
            if(moller_removed and electron_mom_sq_>=min_mom_sq_for_moller)
            {
                return bethe_subtract_moller(electron_mom_sq_, min_energy);
            }
            else
            {
                return bethe_formula(electron_mom_sq_);
            }

        }
        else
        {
            size_t index=search_sorted_exponential(electron_mom_sq, electron_mom_sq_);
            return electron_interp_factors[index]* std::pow(electron_mom_sq_, electron_interp_powers[index]);
        }
	}

	double electron_lookup_variable_RML(double electron_mom_sq_, double min_energy_)
	//use this if minimum energy can vary, and used default constructor
	{
	    min_energy=min_energy_;
	    double min_mom_sq_for_moller__=min_energy*2.0+1.0;
	    min_mom_sq_for_moller__*=min_mom_sq_for_moller__;
	    min_mom_sq_for_moller__-=1.0;

        if(electron_mom_sq_<electron_mom_sq[0])
        {
            return electron_interp_factors[0]*std::pow(electron_mom_sq_, electron_interp_powers[0]); //use first interpolant to extrapolate to very low energy
        }
        else if(electron_mom_sq_>electron_mom_sq[electron_mom_sq.size()-1])
        {
            if(electron_mom_sq_>=min_mom_sq_for_moller__)
            {
                return bethe_subtract_moller(electron_mom_sq_, min_energy);
            }
            else
            {
                return bethe_formula(electron_mom_sq_);
            }

        }
        else
        {
            size_t index=search_sorted_exponential(electron_mom_sq, electron_mom_sq_);
            double SP= electron_interp_factors[index]* std::pow(electron_mom_sq_, electron_interp_powers[index]);
            if( electron_mom_sq_>=min_mom_sq_for_moller__)
            {
                return SP-=moller_losses(electron_mom_sq_, min_energy);
            }
            else
            {
                return SP;
            }
        }
	}
};


////positrons not implemented yet

#endif
