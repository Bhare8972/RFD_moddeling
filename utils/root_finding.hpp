
#ifndef ROOT_FINDING_HPP
#define ROOT_FINDING_HPP

#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>

#include "functor.hpp"
#include "gen_ex.hpp"

double root_finder_brent(gsl_function& func, double upper_bound, double lower_bound, double epsabs=0.001, double epsrel=0.001, int max_iter=100)
{
    const gsl_root_fsolver_type *T= gsl_root_fsolver_brent;
    gsl_root_fsolver *solver = gsl_root_fsolver_alloc (T);

    gsl_root_fsolver_set (solver, &func, lower_bound, upper_bound);

    double root=0;
    int iter=0;
    int status=GSL_CONTINUE;
    do
    {
        iter++;
        status = gsl_root_fsolver_iterate (solver);
        root = gsl_root_fsolver_root (solver);
        lower_bound = gsl_root_fsolver_x_lower (solver);
        upper_bound = gsl_root_fsolver_x_upper (solver);
        status = gsl_root_test_interval(lower_bound, upper_bound, epsabs, epsrel);

        if (status == GSL_SUCCESS) break;
    }
    while (status == GSL_CONTINUE && iter < max_iter);

    if(status != GSL_SUCCESS)
    {
        throw general_exception("root finding errored");
    }

    gsl_root_fsolver_free (solver);

    return root;

}

double root_finder_brent(functor_1D* func, double upper_bound, double lower_bound, double epsabs=0.001, double epsrel=0.001, int max_iter=100)
{

    const gsl_root_fsolver_type *T= gsl_root_fsolver_brent;
    gsl_root_fsolver *solver = gsl_root_fsolver_alloc(T);

    auto F=func->get_gsl_func();
    gsl_root_fsolver_set (solver, &F, lower_bound, upper_bound);

    double root=0;
    int iter=0;
    int status=GSL_CONTINUE;
    do
    {
        iter++;
        status = gsl_root_fsolver_iterate (solver);
        root = gsl_root_fsolver_root (solver);
        lower_bound = gsl_root_fsolver_x_lower (solver);
        upper_bound = gsl_root_fsolver_x_upper (solver);
        status = gsl_root_test_interval(lower_bound, upper_bound, epsabs, epsrel);

        if (status == GSL_SUCCESS) break;
    }
    while (status == GSL_CONTINUE && iter < max_iter);

    if(status != GSL_SUCCESS)
    {
        throw general_exception("root finding errored");
    }

    gsl_root_fsolver_free (solver);

    return root;
}


#endif
