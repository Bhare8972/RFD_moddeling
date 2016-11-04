#ifndef TABLE_IO
#define TABLE_IO

#include <list>
#include <memory>

#include "binary_IO.hpp"
#include "vector.hpp"
#include "vector_float.hpp"
#include "vector_int.hpp"
#include "vector_long.hpp"
#include "gen_ex.hpp"


//// this is a few utilities for inputing and output arrays of numerical data in binary format

//note that, while rather functional, this API is a bit ugly and needs to be improved

//need to fix wierdness that ints are actually longs


////output

class array_output
{
public:
    virtual void write_out(binary_output* fout)=0;
};

typedef std::shared_ptr<array_output> AO_pntr;

class ints_output : public array_output
{
private:
    gsl::vector_long data;

public:
    ints_output(gsl::vector_long data_)
    {
        data=data_;
    }

    void write_out(binary_output* fout)
    {
        fout->out_short(1);
        fout->out_int(data.size());
        for( int out : data)
        {
            fout->out_int(out);
        }
    }
};

class floats_output : public array_output
{
private:
    gsl::vector_float data;

public:
    floats_output(gsl::vector_float data_)
    {
        data=data_;
    }

    void write_out(binary_output* fout)
    {
        fout->out_short(2);
        fout->out_int(data.size());
        for( float out : data)
        {
            fout->out_float(out);
        }
    }
};

class doubles_output : public array_output
{
private:
    gsl::vector data;

public:
    doubles_output(gsl::vector data_)
    {
        data=data_;
    }

    void write_out(binary_output* fout)
    {
        fout->out_short(3);
        fout->out_int(data.size());
        for( double out : data)
        {
            fout->out_double(out);
        }
    }
};

class arrays_output : public array_output
{
private:
    std::list<AO_pntr> data;

public:
    void add_array(AO_pntr new_array)
    {
        data.push_back(new_array);
    }

    void add_doubles(gsl::vector double_data)
    {
        auto new_array=std::make_shared<doubles_output>(double_data);
        data.push_back(new_array);
    }

    void add_ints(gsl::vector_long long_data)
    {
        auto new_array=std::make_shared<ints_output>(long_data);
        data.push_back(new_array);
    }


    void write_out(binary_output* fout)
    {
        fout->out_short(0);
        fout->out_int(data.size());
        for( auto out_array : data)
        {
            out_array->write_out(fout);
        }
    }

    void to_file(std::string fname)
    {
        binary_output fout(fname);
        write_out(&fout);
    }


};

////input

//this needs to be redesigned so that each array input has it's own file handle, and to have more sensiscal API.

class array_input
{
private:
    int type;
    int32_t size;
    int num_left;
    binary_input file_input;

public:
    array_input(binary_input fin)
    {
        file_input=fin;
        type=file_input.in_short();
        size=file_input.in_int();
        num_left=size;
    }

    int32_t get_size()
    {
        return size;
    }

    gsl::vector_long read_ints()
    {
        if(type!=1){ throw gen_exception("cannot read integers from file"); }
        if(num_left==0){ throw gen_exception("no data left in this array"); }

        gsl::vector_long out=gsl::vector_long (size_t(size));
        for(int i=0; i<size; i++)
        {
            out[i]=file_input.in_int();
        }
        num_left=0;
        return out;
    }

    gsl::vector_float read_floats()
    {
        if(type!=2){ throw gen_exception("cannot read floats from file got:", type); }
        if(num_left==0){ throw gen_exception("no data left in this array");}

        gsl::vector_float out=gsl::vector_float (size_t(size));
        for(int i=0; i<size; i++)
        {
            out[i]=file_input.in_float();
        }
        num_left=0;
        return out;
    }

    gsl::vector read_doubles()
    {
        if(type!=3){ throw gen_exception("cannot read doubles from file"); }
        if(num_left==0){ throw gen_exception("no data left in this array"); }

        gsl::vector out=gsl::vector(size_t(size));
        for(int i=0; i<size; i++)
        {
            out[i]=file_input.in_double();
        }
        num_left=0;
        return out;
    }

    gsl::vector read_doublesArray()
    {
        auto inarray=get_array();
        return inarray.read_doubles();
    }

    gsl::vector_long read_intsArray()
    {
        auto inarray=get_array();
        return inarray.read_ints();
    }

    array_input get_array()
    {
        if(type!=0){ throw gen_exception("cannot read arrays from file"); }
        if(num_left==0){ throw gen_exception("no data left in this array"); }
        num_left-=1;
        return array_input(file_input);
    }
};

#endif
