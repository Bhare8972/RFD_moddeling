#include <list>
#include <string>

#include "GSL_utils.hpp"
#include "binary_IO.hpp"
#include "arrays_IO.hpp"

using namespace std;

class energy_info
{
public:

    list< gsl::vector > X_table;
    list< gsl::vector > Y_table;

    energy_info(int N_timesteps, array_input &tables_in)
    {
        for(int i=0; i<N_timesteps; i++)
        {
            array_input dist_X_table=tables_in.get_array();
            auto X=dist_X_table.read_doubles();

            array_input dist_Y_table=tables_in.get_array();
            auto Y=dist_Y_table.read_doubles();

            X_table.emplace_back(X);
            Y_table.emplace_back(Y);
        }
    }

    void output(arrays_output& tables_out)
    {
        auto Y_iter=Y_table.begin();
        for(auto& X_vector : X_table)
        {

            shared_ptr<doubles_output> X_dist_table=make_shared<doubles_output>(X_vector);
            tables_out.add_array(X_dist_table);

            shared_ptr<doubles_output> Y_dist_table=make_shared<doubles_output>(*Y_iter);
            tables_out.add_array(Y_dist_table);
            Y_iter++;
        }
    }
};

int main()
{
    string file_location="./";
    list<string> fnames({"diffusion_table_TRANSFORM", "diffusion_table_MONTECARLO"});
    list<int> first_energy({0, 1});

    list< gsl::vector > energy_vectors;
    gsl::vector timesteps;
    list< energy_info > energies_info;

    int N_energies=0;
    bool first_file=true;
    auto first_energy_iter=first_energy.begin();
    for(auto fname : fnames)
    {
        binary_input fin(file_location+fname);
        array_input table_in(fin);

        array_input energy_table=table_in.get_array();
        auto file_energies=energy_table.read_doubles();

        int first_energy_index=*first_energy_iter;
        if(first_energy_index!=0)
        {
            file_energies=file_energies.clone(first_energy_index, file_energies.size()-first_energy_index );
        }

        array_input timesteps_table=table_in.get_array();
        auto file_timesteps=timesteps_table.read_doubles();

        //// we are going to assume that everything is okay!!
        energy_vectors.emplace_back(file_energies);
        N_energies+=file_energies.size();
        if(first_file)
        {
            timesteps=file_timesteps;
            first_file=false;
        }

        for(int i=0; i<file_energies.size(); i++)
        {
            energies_info.emplace_back(timesteps.size(), table_in);
        }
        first_energy_iter++;
    }

    gsl::vector energies(N_energies);
    int current_index=0;
    for(auto& EV : energy_vectors)
    {
        for(auto E: EV)
        {
            energies[current_index]=E;
            current_index++;
        }
    }

    //output
    arrays_output tables_out;
    shared_ptr<doubles_output> energies_table=make_shared<doubles_output>(energies);
    tables_out.add_array(energies_table);
    shared_ptr<doubles_output> timesteps_table=make_shared<doubles_output>(timesteps);
    tables_out.add_array(timesteps_table);

    for(auto& W : energies_info)
    {
        W.output(tables_out);
    }

    print("saving out");
	binary_output fout("./shielded_coulomb_diffusion");
	tables_out.write_out( &fout);
}
