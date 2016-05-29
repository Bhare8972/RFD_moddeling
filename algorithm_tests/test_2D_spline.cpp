

#include <cmath>
#include "GSL_utils.hpp"
#include "spline_2D.hpp"
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

class gaussian : public functor_1D
{
    public:

    double width;
    double amp;
    double X0;

    gaussian(double width_, double amp_, double X0_)
    {
        width=width_;
        amp=amp_;
        X0=X0_;
    }

    double call(double X)
    {
        double P=(X-X0)/(width);
        return amp*std::exp( -P*P*0.5 );
    }

};

int main()
{
    test_function F;
    adaptive_2DSpline spline(F, 1E5, 0, 0, 2*3.1415926, 2*3.1415926);

    print(F.num_tests, "function calls");


    ////test just interpolation ability

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

    print("tested spline sampling");

    //write to file
    arrays_output tables_out;

    shared_ptr<doubles_output> X_table=make_shared<doubles_output>(X);
    tables_out.add_array(X_table);

    shared_ptr<doubles_output> Y_table=make_shared<doubles_output>(Y);
    tables_out.add_array(Y_table);

    shared_ptr<doubles_output> spline_table=make_shared<doubles_output>(output);
    tables_out.add_array(spline_table);

	binary_output fout("2D_tst_A");
	tables_out.write_out( &fout);


	//sample the gaussian in 1D
	gaussian K(3.1415926/2.0, 1, 3.1415926);
	auto G_spline =adaptive_sample_retSpline(&K, 0.001, 0, 2*3.1415926  );

	//show the samples
	auto gaussian_points=G_spline->callv(X);

    arrays_output tables_out_2;

    shared_ptr<doubles_output> X_table_2=make_shared<doubles_output>(X);
    tables_out_2.add_array(X_table_2);

    shared_ptr<doubles_output> Y_table_2=make_shared<doubles_output>(gaussian_points);
    tables_out_2.add_array(Y_table_2);

	binary_output fout2("2D_tst_B");
	tables_out.write_out( &fout2);


    print("tested gaussian");


	//do our integration!!!!
    auto integrate_spline= spline.integrate_along_Y(G_spline);

	//show the samples
	auto integration_points=integrate_spline->callv(X);

    arrays_output tables_out_3;

    shared_ptr<doubles_output> X_table_3=make_shared<doubles_output>(X);
    tables_out_2.add_array(X_table_3);

    shared_ptr<doubles_output> Y_table_3=make_shared<doubles_output>(integration_points);
    tables_out_2.add_array(Y_table_3);

	binary_output fout3("2D_tst_C");
	tables_out.write_out( &fout3);

    print("tested integrator");


}
