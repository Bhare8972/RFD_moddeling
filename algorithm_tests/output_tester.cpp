

#include "GSL_utils.hpp"
#include "binary_IO.hpp"

using namespace std;

void read_new_electron(binary_input& fin)
{
    int charge=fin.in_short();
    double current_time=fin.in_double();

    double pos_0=fin.in_double();
    double pos_1=fin.in_double();
    double pos_2=fin.in_double();

    double mom_0=fin.in_double();
    double mom_1=fin.in_double();
    double mom_2=fin.in_double();
}

void read_update_electron(binary_input& fin)
{
    double timestep=fin.in_double();

    double pos_0=fin.in_double();
    double pos_1=fin.in_double();
    double pos_2=fin.in_double();

    double mom_0=fin.in_double();
    double mom_1=fin.in_double();
    double mom_2=fin.in_double();
}

void read_remove_electron(binary_input& fin)
{
    int reason=fin.in_short();

    double timestep=fin.in_double();

    double pos_0=fin.in_double();
    double pos_1=fin.in_double();
    double pos_2=fin.in_double();

    double mom_0=fin.in_double();
    double mom_1=fin.in_double();
    double mom_2=fin.in_double();
}

int main()
{
    string fname="output";

    binary_input fin(fname);

    int n=0;
    while( not fin.at_end())
    {
        int command=fin.in_short();
        print(command, fin.in_file->good(), fin.in_file->eof(), fin.in_file->fail(), fin.in_file->bad());
        int ID=fin.in_int();
        if(command==1)
        {
            read_new_electron(fin);
        }
        else if(command==2)
        {
            read_update_electron(fin);
        }
        else if(command==3)
        {
            read_remove_electron(fin);
        }
        else if(command==4)
        {
            print("done reading!");
            break;
        }
        else
        {
            print("error!:", command, n);
            print("good:", fin.in_file->good(), fin.in_file->eof(), fin.in_file->fail(), fin.in_file->bad() );

            fin.in_file->seekg(-40, ios_base::cur);
            char to_read;
            fin.in_file->read(&to_read, 1);
            int num=(int8_t)(to_read);

            ID=fin.in_int();
            print( to_read, num, fin.in_file->tellg());
            break;
        }
        print("S:", fin.in_file->good(), fin.in_file->eof(), fin.in_file->fail(), fin.in_file->bad());
        n+=1;
    }
}
