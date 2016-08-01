
#ifndef GSL_SPLINE_HPP
#define GSL_SPLINE_HPP

#include <gsl/gsl_spline.h>

#include "spline.hpp"

namespace GSL_SPLINE_TOOLS
{
    //copied from GSL code
    static inline void
    coeff_calc (const double c_array[], double dy, double dx, size_t index,
                double * b, double * c, double * d)
    {
      const double c_i = c_array[index];
      const double c_ip1 = c_array[index + 1];
      *b = (dy / dx) - dx * (c_ip1 + 2.0 * c_i) / 3.0;
      *c = c_i;
      *d = (c_ip1 - c_i) / (3.0 * dx);
    }

    typedef struct
    {
      double * c;
      double * g;
      double * diag;
      double * offdiag;
    } cspline_state_t;

    typedef struct
    {
      double * b;
      double * c;
      double * d;
      double * _m;
    } akima_state_t;

};

std::shared_ptr<poly_spline> natural_cubic_spline(gsl::vector X, gsl::vector Y)
{
    gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, X.size());
    gsl_spline_init(spline, &X[0], &Y[0], X.size());

    auto out_spline=std::make_shared<poly_spline>();
    out_spline->x_vals=X;
    out_spline->splines.reserve(X.size()-1);


    const GSL_SPLINE_TOOLS::cspline_state_t *state = (const GSL_SPLINE_TOOLS::cspline_state_t *) spline->interp->state;
    for(int spline_index=0; spline_index<X.size()-1; spline_index++)
    {
        double x_hi = X[spline_index + 1];
        double x_lo = X[spline_index];
        double dx = x_hi - x_lo;

        double y_lo = Y[spline_index];
        double y_hi = Y[spline_index + 1];
        double dy = y_hi - y_lo;
        double delx = x_hi - x_lo;
        double b_i, c_i, d_i;
        GSL_SPLINE_TOOLS::coeff_calc(state->c, dy, dx, spline_index,  &b_i, &c_i, &d_i);

        gsl::vector weights(4);

        weights[0]=y_lo - x_lo*( b_i + x_lo*(c_i - x_lo*d_i ) );
        weights[1]=b_i - x_lo*(c_i + x_lo*d_i);
        weights[2]=c_i - x_lo*d_i;
        weights[3]=d_i;

        out_spline->splines.emplace_back(weights);
    }

    gsl_spline_free(spline);
    return out_spline;
}

std::shared_ptr<poly_spline> akima_spline(gsl::vector X, gsl::vector Y)
{
    gsl_spline *spline = gsl_spline_alloc (gsl_interp_akima, X.size());
    gsl_spline_init(spline, &X[0], &Y[0], X.size());

    auto out_spline=std::make_shared<poly_spline>();
    out_spline->x_vals=X;
    out_spline->splines.reserve(X.size()-1);


    const GSL_SPLINE_TOOLS::akima_state_t *state = (const GSL_SPLINE_TOOLS::akima_state_t *) spline->interp->state;
    for(int spline_index=0; spline_index<X.size()-1; spline_index++)
    {
        double x_hi = X[spline_index + 1];
        double x_lo = X[spline_index];
        double dx = x_hi - x_lo;

        double y_lo = Y[spline_index];
        double y_hi = Y[spline_index + 1];
        double dy = y_hi - y_lo;
        double delx = x_hi - x_lo;

        double b = state->b[spline_index];
        double c = state->c[spline_index];
        double d = state->d[spline_index];

        gsl::vector weights(4);

        weights[0]=y_lo - x_lo*( b + x_lo*(c - x_lo*d ) );
        weights[1]=b - x_lo*(c + x_lo*d);
        weights[2]=c - x_lo*d;
        weights[3]=d;

        out_spline->splines.emplace_back(weights);
    }

    gsl_spline_free(spline);
    return out_spline;
}

#endif // GSL_SPLINE_HPP
