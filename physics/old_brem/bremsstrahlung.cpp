
const double integration_precision=1E7;


double  bremsstrahlung_cross_section(double initial_energy, double photon_energy, double photon_theta, double final_electron_theta, double delta_electron_photon_phi)
//not a very usefull function. Here mostly to outline how one would calculate brem cross section
{

    double final_electron_energy=initial_energy-photon_energy;
    double initial_electron_momentum_squared=(1+initial_energy)*(1+initial_energy) - 1.0;
    double final_electron_momentum_squared=(1+final_electron_energy)*(1+final_electron_energy) - 1.0;

    double initial_momentum=std::sqrt(initial_electron_momentum_squared);
    double final_momentum=std::sqrt(final_electron_momentum_squared);

    double cos_theta_initial=std::cos(photon_theta);
    double sin_theta_initial=std::sin(photon_theta);

    double cos_theta_final=std::cos(photon_theta)*std::cos(final_electron_theta) + std::sin(photon_theta)*std::sin(final_electron_theta)*std::cos(delta_electron_photon_phi);
    double sin_theta_final==std::sqrt( std::max(0.0, 1.0-cos_theta_final*cos_theta_final));//std::sqrt(1.0-cos_theta_final*cos_theta_final);

    double cos_phi=std::cos(final_electron_theta)-cos_theta_final*cos_theta_initial;


    double q_sq=initial_electron_momentum_squared + final_electron_momentum_squared + photon_energy*photon_energy
    -2*initial_momentum*photon_energy*cos_theta_initial + 2*final_momentum*photon_energy*cos_theta_final
    -2*final_momentum*initial_momentum*(cos_theta_final*cos_theta_initial + sin_theta_final*sin_theta_initial*cos_phi);

    double prefactor_2=final_momentum/(photon_energy*initial_momentum);
    double prefactor_3=1.0/((q_sq+K_sq)*(q_sq+K_sq));

    double total_initial_energy=initial_energy+1.0;
    double total_final_energy=final_electron_energy+1.0;


    double A_numerator=final_electron_momentum_squared*sin_theta_final*sin_theta_final*(4.0*total_initial_energy*total_initial_energy-q_sq);
    double A_denom_sqrt=total_final_energy-final_momentum*cos_theta_final;

    double B_numerator=initial_momentum*initial_momentum*sin_theta_initial*sin_theta_initial*(4.0*total_final_energy*total_final_energy-q_sq);
    double B_denom_sqrt=total_initial_energy-initial_momentum*cos_theta_initial;

    double C_numerator=2*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi*(4*total_initial_energy*total_final_energy-q_sq);

    //double CD_denom_1=final_electron_energy-final_momentum*cos_theta_final;
    //double CD_denom_2=initial_energy-initial_momentum*cos_theta_initial;

    double D_numerator=2*photon_energy*photon_energy*(final_electron_momentum_squared*sin_theta_final*sin_theta_final + initial_electron_momentum_squared*sin_theta_initial*sin_theta_initial
    - 2.0*final_momentum*initial_momentum*sin_theta_final*sin_theta_initial*cos_phi);

    //return prefactor*prefactor_2*prefactor_3*( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (C_numerator + D_numerator)/(CD_denom_1*CD_denom_2));

    double ret= prefactor * prefactor_2 * prefactor_3*( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (C_numerator + D_numerator)/(A_denom_sqrt*B_denom_sqrt));

    if(ret!=ret)
    {
        throw gen_exception("warning, nan value in brem");
    }
    return ret;
}

//class delta_phi_sampler


int calling_index=0;
    double delta_phi_sampler::call(double delta_electron_photon_phi)
    {
calling_index++;
if((calling_index%100000)==0) print(calling_index);
        //a long series of equations needed to calculate brem cross section.
        //these equations are descrived elsewhere
        double cos_theta_final=std::cos(parent_electron_theta_sampler->photon_theta)*std::cos(final_electron_theta) +
        std::sin(parent_electron_theta_sampler->photon_theta)*std::sin(final_electron_theta)*std::cos(delta_electron_photon_phi);  //some of these coesines and sines are known. others can be known
//print(cos_theta_final);

        double sin_theta_final=std::sqrt( std::max(0.0, 1.0-cos_theta_final*cos_theta_final));
        //double sin_theta_final=std::sin( std::acos(cos_theta_final) ); //ow not sure which is better

//print('B', sin_theta_final);

        double cos_phi=std::cos(final_electron_theta)-cos_theta_final*parent_electron_theta_sampler->cos_theta_initial;
//print(cos_phi);

        double q_sq=parent_photon_energy_sampler->initial_electron_momentum_squared + parent_photon_theta_sampler->final_electron_momentum_squared
        + parent_photon_theta_sampler->photon_energy * parent_photon_theta_sampler->photon_energy
        - 2*parent_photon_energy_sampler->initial_electron_momentum * parent_photon_theta_sampler->photon_energy * parent_electron_theta_sampler->cos_theta_initial
        + 2*parent_photon_theta_sampler->final_electron_momentum * parent_photon_theta_sampler->photon_energy * cos_theta_final
        - 2*parent_photon_theta_sampler->final_electron_momentum * parent_photon_energy_sampler->initial_electron_momentum
        * (cos_theta_final * parent_electron_theta_sampler->cos_theta_initial + sin_theta_final * parent_electron_theta_sampler->sin_theta_initial * cos_phi);
//print('C', q_sq);
//print(parent_photon_energy_sampler->initial_electron_momentum_squared );
//print(parent_photon_theta_sampler->final_electron_momentum_squared);
//print(parent_photon_theta_sampler->photon_energy);

        double total_initial_energy=parent_photon_energy_sampler->initial_energy+1.0;
        double total_final_energy=parent_photon_theta_sampler->final_electron_energy+1.0;

        double prefactor_3=1.0/((q_sq+K_sq)*(q_sq+K_sq));

        double A_numerator=parent_photon_theta_sampler->final_electron_momentum_squared * sin_theta_final * sin_theta_final
        *(4.0* total_initial_energy * total_initial_energy - q_sq);

        double A_denom_sqrt=total_final_energy - parent_photon_theta_sampler->final_electron_momentum * cos_theta_final;

        double B_numerator=parent_photon_energy_sampler->initial_electron_momentum_squared * parent_electron_theta_sampler->sin_theta_initial * parent_electron_theta_sampler->sin_theta_initial
        *(4.0*total_final_energy * total_final_energy - q_sq);

        double B_denom_sqrt=total_initial_energy - parent_photon_energy_sampler->initial_electron_momentum * parent_electron_theta_sampler->cos_theta_initial;

        double C_numerator=2*parent_photon_theta_sampler->final_electron_momentum * parent_photon_energy_sampler->initial_electron_momentum * sin_theta_final * parent_electron_theta_sampler->sin_theta_initial * cos_phi
        *(4*total_initial_energy * total_final_energy - q_sq);

        double D_numerator=2*parent_photon_theta_sampler->photon_energy * parent_photon_theta_sampler->photon_energy *
        (parent_photon_theta_sampler->final_electron_momentum_squared * sin_theta_final*sin_theta_final
         + parent_photon_energy_sampler->initial_electron_momentum_squared * parent_electron_theta_sampler->sin_theta_initial * parent_electron_theta_sampler->sin_theta_initial
        - 2.0*parent_photon_theta_sampler->final_electron_momentum * parent_photon_energy_sampler->initial_electron_momentum * sin_theta_final * parent_electron_theta_sampler->sin_theta_initial * cos_phi);

        double ret= prefactor * parent_photon_theta_sampler->prefactor_2 * prefactor_3
        *( A_numerator/(A_denom_sqrt*A_denom_sqrt) + B_numerator/(B_denom_sqrt*B_denom_sqrt) + (C_numerator + D_numerator)/(A_denom_sqrt*B_denom_sqrt));

        if(ret!=ret)
        {
            throw gen_exception("warning, nan value in brem");
        }
        return ret;
    }


    double delta_phi_sampler::set_electron_theta(double final_electron_theta_)
    {
        //set variables
        final_electron_theta=final_electron_theta_;

        //integrate across delta_phi
//print("A", call(PI));
//throw gen_exception("pirates");
        cum_adap_simps integrator(this, 0, 2*PI, integration_precision);
//print("B");
        gsl::vector delta_phi_points=integrator.points();
        gsl::vector CS_values=integrator.cum_quads();

        double final_result=CS_values[CS_values.size()-1];



        //normalize and invert
        if(CS_values[0]!=CS_values[0])//is nan
        {
            throw gen_exception("nan value in brem");
        }

        CS_values/=CS_values[CS_values.size()-1]; //normalize to values from 0 to 1

        if(CS_values[0]!=CS_values[0])//is nan
        {
            can_be_selected=false;
            return 0;
        }
        else
        {
            can_be_selected=true;
            gsl::vector sampler_X;
            gsl::vector sampler_Y;
            make_fix_spline(CS_values, delta_phi_points, sampler_X, sampler_Y);

/*
arrays_output tables_out;
std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( CS_values );
tables_out.add_array(interaction_energies_table);
std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( delta_phi_points );
tables_out.add_array(interaction_rates_table);

binary_output fout(std::string("./test_out"));
tables_out.write_out( &fout);
fout.flush();
*/
            delta_phi_sampler=std::make_shared<poly_spline>(sampler_X, sampler_Y);


            return final_result;
        }
    }

    inline double delta_phi_sampler::sample(double U)
    //returns a sample for delta phi
    {
        if(not can_be_selected) throw general_exception("error: unselectable value was selected in brem.");
        return delta_phi_sampler->call(U);
    }

//class electron_theta_sampler
    double electron_theta_sampler::call(double electron_theta)
    {
        auto new_delta_phi_sampler=new delta_phi_sampler;
        new_delta_phi_sampler->parent_photon_energy_sampler=parent_photon_energy_sampler;
        new_delta_phi_sampler->parent_photon_theta_sampler=parent_photon_theta_sampler;
        new_delta_phi_sampler->parent_electron_theta_sampler=this;

        double result = new_delta_phi_sampler->set_electron_theta(electron_theta);
        samplers_per_electron_theta.insert(electron_theta, new_delta_phi_sampler);

        return result*std::sin(electron_theta);
    }

    double electron_theta_sampler::set_photon_theta(double photon_theta_)
    {
        photon_theta=photon_theta_;
        cos_theta_initial=std::cos(photon_theta);
        sin_theta_initial=std::sin(photon_theta);

        //integrate across electron theta
        cum_adap_simps integrator(this, 0, PI, integration_precision);
        gsl::vector electron_theta_points=integrator.points();
        gsl::vector CS_values=integrator.cum_quads();

        double final_result=CS_values[CS_values.size()-1];

/*
arrays_output tables_out;
std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( CS_values );
tables_out.add_array(interaction_energies_table);
std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( electron_theta_points );
tables_out.add_array(interaction_rates_table);

binary_output fout(std::string("./test_out"+std::to_string(calling_index1)));
tables_out.write_out( &fout);
fout.flush();
calling_index1++;*/

        //normalize and invert
        CS_values/=CS_values[CS_values.size()-1]; //normalize to values from 0 to 1
        gsl::vector sampler_X;
        gsl::vector sampler_Y;
        make_fix_spline(CS_values, electron_theta_points, sampler_X, sampler_Y);
//electron_theta_sampler=std::make_shared<poly_spline>(sampler_X, sampler_Y);

        return final_result;
    }

    inline double electron_theta_sampler::sample(rand_gen& rand, double& delta_phi)
    {
        //sample elecron theta
        double electron_theta=electron_theta_sampler->call( rand.uniform() );

        //sample delta_phi interpolate to nearist delta_phi
        auto delta_phi_result=samplers_per_electron_theta.lookup(electron_theta);
        double delta0=std::abs( delta_phi_result.left_key - electron_theta);
        double delta1=std::abs( delta_phi_result.right_key - electron_theta);

        if(delta0<delta1)
        {
            delta_phi=delta_phi_result.left_data->sample(rand.uniform());
        }
        else
        {
            delta_phi=delta_phi_result.right_data->sample(rand.uniform());
        }

        return electron_theta;
    }

//class photon_theta_sampler

int calling_index1=0;
    double photon_theta_sampler::call(double photon_theta)
    {
        auto new_electron_theta_sampler=new electron_theta_sampler;
        new_electron_theta_sampler->parent_photon_energy_sampler=parent_photon_energy_sampler;
        new_electron_theta_sampler->parent_photon_theta_sampler=this;

        double result=new_electron_theta_sampler->set_photon_theta(photon_theta);
        samplers_per_photon_theta.insert(photon_theta, new_electron_theta_sampler);
        return result*std::sin(photon_theta);
    }

    double photon_theta_sampler::set_photon_energy(double photon_energy_)
    {
        //set variables
        photon_energy=photon_energy_;
        final_electron_energy=parent_photon_energy_sampler->initial_energy - photon_energy;
        final_electron_momentum_squared=(1+final_electron_energy)*(1+final_electron_energy) - 1.0;
        final_electron_momentum=std::sqrt(final_electron_momentum_squared);
        prefactor_2=final_electron_momentum/(photon_energy*parent_photon_energy_sampler->initial_electron_momentum);
//print("final electron energy", final_electron_energy);

        //integrate across photon theta
        cum_adap_simps integrator(this, 0, PI, integration_precision);
        gsl::vector photon_theta_points=integrator.points();
        gsl::vector CS_values=integrator.cum_quads();

print(photon_energy, calling_index1);
/*
gsl::vector vals=integrator.values();
arrays_output tables_out;
std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( vals );
tables_out.add_array(interaction_energies_table);
std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( photon_theta_points );
tables_out.add_array(interaction_rates_table);

binary_output fout(std::string("./test_outV"+std::to_string(calling_index1)));
tables_out.write_out( &fout);
fout.flush();*/
calling_index1++;

        double final_result=CS_values[CS_values.size()-1];

        //normalize and invert
        CS_values/=CS_values[CS_values.size()-1]; //normalize to values from 0 to 1
        gsl::vector sampler_X;
        gsl::vector sampler_Y;
        make_fix_spline(CS_values, photon_theta_points, sampler_X, sampler_Y);
        photon_theta_sampler=std::make_shared<poly_spline>(sampler_X, sampler_Y);

        return final_result;
    }

    inline double photon_theta_sampler::sample(rand_gen& rand, double& electron_theta, double& delta_phi)
    {
        //sample photon theta
        double photon_theta=photon_theta_sampler->call( rand.uniform() );

        //sample delta_phi interpolate to nearist delta_phi
        auto electron_theta_result=samplers_per_photon_theta.lookup(photon_theta);
        double delta0=std::abs( electron_theta_result.left_key - photon_theta);
        double delta1=std::abs( electron_theta_result.right_key - photon_theta);

        if(delta0<delta1)
        {
            electron_theta=electron_theta_result.left_data->sample(rand, delta_phi);
        }
        else
        {
            electron_theta=electron_theta_result.right_data->sample(rand, delta_phi);
        }

        return photon_theta;
    }

//class photon_energy_sampler
    double photon_energy_sampler::call(double photon_energy)
    {
        auto new_photon_theta_sampler=new photon_theta_sampler;
        new_photon_theta_sampler->parent_photon_energy_sampler=this;
//print("initial electron energy", initial_energy, "photon energy", photon_energy);
        double result=new_photon_theta_sampler->set_photon_energy(photon_energy);
        //samplers_per_photon_energy.insert(photon_energy, new_photon_theta_sampler);
//if(calling_index1==99)
//{
//    print(photon_energy*energy_units_kev, initial_energy*energy_units_kev);
//    print(new_photon_theta_sampler->prefactor_2, new_photon_theta_sampler->final_electron_momentum);
//    print();
//}
        delete new_photon_theta_sampler;

        return result;
    }

    double photon_energy_sampler::set_initial_energy(double initial_energy_)
    {
        //set variables
        initial_energy=initial_energy_;
        initial_electron_momentum_squared=(1+initial_energy)*(1+initial_energy) - 1.0;
        initial_electron_momentum=std::sqrt(initial_electron_momentum_squared);

//print("initial energy A", lowest_physical_energy, initial_energy);

        //integrate across photon energy. assume that photon energy spans from lowest physical energy up to initial energy
        cum_adap_simps integrator(this, lowest_physical_energy, initial_energy*0.9999, 7E6);//integration_precision); //need to insure that photon energy is always smaller than initial energy
        gsl::vector photon_energy_points=integrator.points();
        gsl::vector CS_values=integrator.cum_quads();
gsl::vector temp_values=integrator.values();

//if(calling_index1==99)
{
arrays_output tables_out;
std::shared_ptr<doubles_output> interaction_energies_table=std::make_shared<doubles_output>( CS_values );
tables_out.add_array(interaction_energies_table);
std::shared_ptr<doubles_output> interaction_rates_table=std::make_shared<doubles_output>( photon_energy_points );
tables_out.add_array(interaction_rates_table);

binary_output fout(std::string("./test_out_int"));
tables_out.write_out( &fout);
fout.flush();

arrays_output tables_out2;
std::shared_ptr<doubles_output> interaction_energies_table2=std::make_shared<doubles_output>( temp_values );
tables_out2.add_array(interaction_energies_table2);
std::shared_ptr<doubles_output> interaction_rates_table2=std::make_shared<doubles_output>( photon_energy_points );
tables_out2.add_array(interaction_rates_table2);

binary_output fout2(std::string("./test_out_val"));
tables_out2.write_out( &fout2);
fout2.flush();
print("saved");
}

        double final_result=CS_values[CS_values.size()-1];

        //normalize and invert
        CS_values/=CS_values[CS_values.size()-1]; //normalize to values from 0 to 1
        gsl::vector sampler_X;
        gsl::vector sampler_Y;
        make_fix_spline(CS_values, photon_energy_points, sampler_X, sampler_Y);
        photon_energy_sampler=std::make_shared<poly_spline>(sampler_X, sampler_Y);

        return final_result;
    }

    inline double photon_energy_sampler::sample(rand_gen& rand, double& photon_theta, double& electron_theta, double& delta_phi)
    {
        //sample photon theta
        double photon_energy=photon_energy_sampler->call( rand.uniform() );

        //sample delta_phi interpolate to nearist delta_phi
        auto photon_theta_result=samplers_per_photon_energy.lookup(photon_energy);
        double delta0=std::abs( photon_theta_result.left_key - photon_energy);
        double delta1=std::abs( photon_theta_result.right_key - photon_energy);

        if(delta0<delta1)
        {
            photon_theta=photon_theta_result.left_data->sample(rand, electron_theta, delta_phi);
        }
        else
        {
            photon_theta=photon_theta_result.right_data->sample(rand, electron_theta, delta_phi);
        }

        return photon_energy;
    }
