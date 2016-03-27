#ifndef BINARY_IO
#define BINARY_IO

#include<string>
#include<iostream>
#inclue<cstdint>
#include <memory>

class binary_output
{
    public:
    std::shared_ptr<std::ofstream> out_file;

    binary_output(std::string fname) : out_file( new std::ofstream(fname.c_str(), mode = ios_base::binary))
    {}

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
    std::shared_ptr<std::ofstream> in_file;

    binary_output(std::string fname) : in_file( new std::ifstream(fname.c_str(), mode = ios_base::binary))
    {}

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
