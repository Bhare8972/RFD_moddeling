

#include <cmath>
#include "GSL_utils.hpp"
#include "spline.hpp"
#include "arrays_IO.hpp"


using namespace std;

class test_function
{
    public:
    int num_tests;
    test_function()
    {
        num_tests=0;
    }


    inline double operator()(double X, double Y)
    {
        num_tests++;
        //return sin(3*X);
        return exp(-X*Y/(3.1415926))*sin(3*X);
    }
};

int main()
{
    test_function F;
    adative_2DSpline spline(F, 1E5, 0, 0, 2*3.1415926, 2*3.1415926);

    print(F.num_tests, "function calls");

    size_t nx=400;
    size_t ny=400;

    auto X=linspace(0, 2*3.1415926, nx);
    auto Y=linspace(0, 2*3.1415926, ny);
    gsl::vector output(nx*ny);

    for(size_t x_i=0; x_i<nx; x_i++)
    {
        for(size_t y_i=0; y_i<ny; y_i++)
        {
            output[x_i + nx*y_i]=spline.call(X[x_i], Y[y_i]);
        }
    }

    //write to file
    arrays_output tables_out;

    shared_ptr<doubles_output> X_table=make_shared<doubles_output>(X);
    tables_out.add_array(X_table);

    shared_ptr<doubles_output> Y_table=make_shared<doubles_output>(Y);
    tables_out.add_array(Y_table);

    shared_ptr<doubles_output> spline_table=make_shared<doubles_output>(output);
    tables_out.add_array(spline_table);

	binary_output fout("2D_tst");
	tables_out.write_out( &fout);

}
