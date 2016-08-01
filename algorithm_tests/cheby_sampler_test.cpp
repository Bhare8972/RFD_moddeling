
#include <cmath>

#include "vector.hpp"
#include "vector_long.hpp"

#include "GSL_utils.hpp"
#include "spline.hpp"
#include "GSL_spline.hpp"
#include "integrate.hpp"
#include "spline_2D.hpp"
#include "arrays_IO.hpp"
#include "rand.hpp"
#include "chebyshev.hpp"

using namespace std;


class PDF : public functor_1D
{
public:
    const double W=1.0;
    int fcalls;
    PDF()
    {
        fcalls=0;
    }

    double call(double X)
    {
        fcalls++;
        return sin(W*X)+1;
    }

    double operator()(double X)
    {
        return call(X);
    }
};

int main()
{
    size_t num_samples=10000000;
    auto X=linspace(0, 3.141592, 1000);

    PDF un_normed_pdf;
    auto actual_Y=un_normed_pdf.callv(X);

    ////use old method
    cum_adap_simps simps_integrator(&un_normed_pdf, 0, 3.1415926, 1E3);
    print("simpson calls:", un_normed_pdf.fcalls);
    un_normed_pdf.fcalls=0;
    auto simps_X=simps_integrator.points();
    auto simps_Y=simps_integrator.values();

    //auto simps_spline = natural_cubic_spline(simps_X, simps_Y);
    auto simps_spline = make_shared<poly_spline>(simps_X, simps_Y);
    auto simps_functions=simps_spline->callv(X);


    ////use chebys
    AdaptiveSpline_Cheby_O3 cheby_integrator(un_normed_pdf, 1E3, 0, 3.1415926);
    print("Cheby calls:", un_normed_pdf.fcalls);
    auto spliner=cheby_integrator.get_spline();
    auto cheby_Y=spliner->callv(X);




    double rate;
    auto sampler=cheby_integrator.inverse_transform(0.01, rate);

    print("testing");
    rand_gen rand;
    gsl::vector out_samples(num_samples);
    for(int i=0; i<num_samples; i++)
    {
        out_samples[i]=sampler.sample(rand.uniform());
    }




    print("saving");



    arrays_output tables_out;

    shared_ptr<doubles_output> X_table=make_shared<doubles_output>(X);
    tables_out.add_array(X_table);

    shared_ptr<doubles_output> actual_Y_table=make_shared<doubles_output>(actual_Y);
    tables_out.add_array(actual_Y_table);

    shared_ptr<doubles_output> cheby_Y_table=make_shared<doubles_output>(cheby_Y);
    tables_out.add_array(cheby_Y_table);

    shared_ptr<doubles_output> simps_Y_table=make_shared<doubles_output>(simps_functions);
    tables_out.add_array(simps_Y_table);

    shared_ptr<doubles_output> cheby_samples=make_shared<doubles_output>(out_samples);
    tables_out.add_array(cheby_samples);



    binary_output fout("./cheby_test");
    tables_out.write_out( &fout);
}
