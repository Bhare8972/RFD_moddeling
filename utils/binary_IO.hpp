#ifndef BINARY_IO
#define BINARY_IO

#include<string>
#include<iostream>
#include<fstream>
#include<cstdint>
#include<memory>

#include "gen_ex.hpp"


class binary_output
{
   public:
    std::shared_ptr<std::ofstream> out_file;

    binary_output(std::string fname)
    {
		out_file=std::make_shared<std::ofstream>(fname.c_str(), std::ios_base::binary);
	}

    void out_short(int8_t out)
    {
        out_file->write((char*)&out,sizeof(int8_t));
    }

    void out_int(int32_t out)
    {
        out_file->write((char*)&out,sizeof(int32_t));
    }

    void out_float(float out)
    {
        out_file->write((char*)&out,sizeof(float));
    }

    void out_double(double out)
    {
        out_file->write((char*)&out,sizeof(double));
    }
};

class binary_input
{
public:
    std::shared_ptr<std::ifstream> in_file;

    binary_input()
    {}

    binary_input(const binary_input& IN)
    {
		in_file=IN.in_file;// and we pray
	}

    binary_input(std::string fname)
    {
		in_file=std::make_shared<std::ifstream>(fname.c_str(), std::ios_base::binary);
		if(not in_file->is_open()) throw gen_exception("file: ", fname, " could not be opened");
	}

    int8_t in_short()
    {
        int8_t to_read;
        in_file->read((char*)&to_read,sizeof(int8_t));
        return to_read;
    }

    int32_t in_int()
    {
        int32_t to_read;
        in_file->read((char*)&to_read,sizeof(int32_t));
        return to_read;
    }

    float in_float()
    {
        float to_read;
        in_file->read((char*)&to_read,sizeof(float));
        return to_read;
    }

    double in_double()
    {
        double to_read;
        in_file->read((char*)&to_read,sizeof(double));
        return to_read;
    }
};

#endif
