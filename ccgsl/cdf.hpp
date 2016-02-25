/*
 * $Id: cdf.hpp 9 2010-06-13 14:02:43Z jdl3 $
 * Copyright (C) 2010 John D Lamb
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CCGSL_CDF_HPP
#define CCGSL_CDF_HPP

#include<gsl/gsl_cdf.h>

namespace gsl {
  /**
   * Namespace for cumulative distribution functions. See also ran.
   */
  namespace cdf {
    /**
     * C++ version of gsl_cdf_ugaussian_P().
     * @param x A real value
     * @return The cumulative distribution function
     */
    inline double ugaussian_P( double const x ){ return gsl_cdf_ugaussian_P( x ); } 
    /**
     * C++ version of gsl_cdf_ugaussian_Q().
     * @param x A real value
     * @return The cumulative distribution function
     */
    inline double ugaussian_Q( double const x ){ return gsl_cdf_ugaussian_Q( x ); } 
    /**
     * C++ version of gsl_cdf_ugaussian_Pinv().
     * @param P A probability
     * @return The cumulative distribution function
     */
    inline double ugaussian_Pinv( double const P ){ return gsl_cdf_ugaussian_Pinv( P ); } 
    /**
     * C++ version of gsl_cdf_ugaussian_Qinv().
     * @param Q A probability
     * @return The inverse cumulative distribution function
     */
    inline double ugaussian_Qinv( double const Q ){ return gsl_cdf_ugaussian_Qinv( Q ); }
    /**
     * C++ version of gsl_cdf_gaussian_P().
     * @param x A real value
     * @param sigma The standard deviation (parameter)
     * @return The cumulative distribution function
     */
    inline double gaussian_P( double const x, double const sigma ){
      return gsl_cdf_gaussian_P( x, sigma ); } 
    /**
     * C++ version of gsl_cdf_gaussian_Q().
     * @param x A real value
     * @param sigma The standard deviation (parameter)
     * @return The cumulative distribution function
     */
    inline double gaussian_Q( double const x, double const sigma ){
      return gsl_cdf_gaussian_Q( x, sigma ); } 
    /**
     * C++ version of gsl_cdf_gaussian_Pinv().
     * @param P A probability
     * @param sigma The standard deviation (parameter)
     * @return The inverse cumulative distribution function
     */
    inline double gaussian_Pinv( double const P, double const sigma ){
      return gsl_cdf_gaussian_Pinv( P, sigma ); } 
    /**
     * C++ version of gsl_cdf_gaussian_Qinv().
     * @param Q A probability
     * @param sigma The standard deviation (parameter)
     * @return The inverse cumulative distribution function
     */
    inline double gaussian_Qinv( double const Q, double const sigma ){
      return gsl_cdf_gaussian_Qinv( Q, sigma ); } 
    /**
     * C++ version of gsl_cdf_gamma_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gamma_P( double const x, double const a, double const b ){
      return gsl_cdf_gamma_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gamma_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gamma_Q( double const x, double const a, double const b ){
      return gsl_cdf_gamma_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gamma_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double gamma_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_gamma_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_gamma_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double gamma_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_gamma_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_cauchy_P().
     * @param x A real value
     * @param a The scale parameter
     * @return The cumulative distribution function
     */
    inline double cauchy_P( double const x, double const a ){
      return gsl_cdf_cauchy_P( x, a ); } 
    /**
     * C++ version of gsl_cdf_cauchy_Q().
     * @param x A real value
     * @param a The scale parameter
     * @return The cumulative distribution function
     */
    inline double cauchy_Q( double const x, double const a ){
      return gsl_cdf_cauchy_Q( x, a ); } 
    /**
     * C++ version of gsl_cdf_cauchy_Pinv().
     * @param P A probability
     * @param a The scale parameter
     * @return The inverse cumulative distribution function
     */
    inline double cauchy_Pinv( double const P, double const a ){
      return gsl_cdf_cauchy_Pinv( P, a ); } 
    /**
     * C++ version of gsl_cdf_cauchy_Qinv().
     * @param Q A probability
     * @param a The scale parameter
     * @return The inverse cumulative distribution function
     */
    inline double cauchy_Qinv( double const Q, double const a ){
      return gsl_cdf_cauchy_Qinv( Q, a ); } 
    /**
     * C++ version of gsl_cdf_laplace_P().
     * @param x A real value
     * @param a Width parameter
     * @return The cumulative distribution function
     */
    inline double laplace_P( double const x, double const a ){
      return gsl_cdf_laplace_P( x, a ); } 
    /**
     * C++ version of gsl_cdf_laplace_Q().
     * @param x A real value
     * @param a Width parameter
     * @return The cumulative distribution function
     */
    inline double laplace_Q( double const x, double const a ){
      return gsl_cdf_laplace_Q( x, a ); } 
    /**
     * C++ version of gsl_cdf_laplace_Pinv().
     * @param P A probability
     * @param a Width parameter
     * @return The inverse cumulative distribution function
     */
    inline double laplace_Pinv( double const P, double const a ){
      return gsl_cdf_laplace_Pinv( P, a ); } 
    /**
     * C++ version of gsl_cdf_laplace_Qinv().
     * @param Q A probability
     * @param a Width parameter
     * @return The inverse cumulative distribution function
     */
    inline double laplace_Qinv( double const Q, double const a ){
      return gsl_cdf_laplace_Qinv( Q, a ); } 
    /**
     * C++ version of gsl_cdf_rayleigh_P().
     * @param x A real value
     * @param sigma The parameter
     * @return The cumulative distribution function
     */
    inline double rayleigh_P( double const x, double const sigma ){
      return gsl_cdf_rayleigh_P( x, sigma ); } 
    /**
     * C++ version of gsl_cdf_rayleigh_Q().
     * @param x A real value
     * @param sigma The parameter
     * @return The cumulative distribution function
     */
    inline double rayleigh_Q( double const x, double const sigma ){
      return gsl_cdf_rayleigh_Q( x, sigma ); } 
    /**
     * C++ version of gsl_cdf_rayleigh_Pinv().
     * @param P A probability
     * @param sigma The parameter
     * @return The inverse cumulative distribution function
     */
    inline double rayleigh_Pinv( double const P, double const sigma ){
      return gsl_cdf_rayleigh_Pinv( P, sigma ); } 
    /**
     * C++ version of gsl_cdf_rayleigh_Qinv().
     * @param Q A probability
     * @param sigma The parameter
     * @return The inverse cumulative distribution function
     */
    inline double rayleigh_Qinv( double const Q, double const sigma ){
      return gsl_cdf_rayleigh_Qinv( Q, sigma ); } 
    /**
     * C++ version of gsl_cdf_chisq_P().
     * @param x A real value
     * @param nu The degrees of freedom
     * @return The cumulative distribution function
     */
    inline double chisq_P( double const x, double const nu ){
      return gsl_cdf_chisq_P( x, nu ); } 
    /**
     * C++ version of gsl_cdf_chisq_Q().
     * @param x A real value
     * @param nu The degrees of freedom
     * @return The cumulative distribution function
     */
    inline double chisq_Q( double const x, double const nu ){
      return gsl_cdf_chisq_Q( x, nu ); } 
    /**
     * C++ version of gsl_cdf_chisq_Pinv().
     * @param P A probability
     * @param nu The degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double chisq_Pinv( double const P, double const nu ){
      return gsl_cdf_chisq_Pinv( P, nu ); } 
    /**
     * C++ version of gsl_cdf_chisq_Qinv().
     * @param Q A probability
     * @param nu The degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double chisq_Qinv( double const Q, double const nu ){
      return gsl_cdf_chisq_Qinv( Q, nu ); } 
    /**
     * C++ version of gsl_cdf_exponential_P().
     * @param x A real value
     * @param mu The mean (parameter)
     * @return The cumulative distribution function
     */
    inline double exponential_P( double const x, double const mu ){
      return gsl_cdf_exponential_P( x, mu ); } 
    /**
     * C++ version of gsl_cdf_exponential_Q().
     * @param x A real value
     * @param mu The mean (parameter)
     * @return The cumulative distribution function
     */
    inline double exponential_Q( double const x, double const mu ){
      return gsl_cdf_exponential_Q( x, mu ); } 
    /**
     * C++ version of gsl_cdf_exponential_Pinv().
     * @param P A probability
     * @param mu The mean (parameter)
     * @return The inverse cumulative distribution function
     */
    inline double exponential_Pinv( double const P, double const mu ){
      return gsl_cdf_exponential_Pinv( P, mu ); } 
    /**
     * C++ version of gsl_cdf_exponential_Qinv().
     * @param Q A probability
     * @param mu The mean (parameter)
     * @return The inverse cumulative distribution function
     */
    inline double exponential_Qinv( double const Q, double const mu ){
      return gsl_cdf_exponential_Qinv( Q, mu ); } 
    /**
     * C++ version of gsl_cdf_exppow_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double exppow_P( double const x, double const a, double const b ){
      return gsl_cdf_exppow_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_exppow_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double exppow_Q( double const x, double const a, double const b ){
      return gsl_cdf_exppow_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_tdist_P().
     * @param x A real value
     * @param nu The degrees of freedom
     * @return The cumulative distribution function
     */
    inline double tdist_P( double const x, double const nu ){ return gsl_cdf_tdist_P( x, nu ); } 
    /**
     * C++ version of gsl_cdf_tdist_Q().
     * @param x A real value
     * @param nu The degrees of freedom
     * @return The cumulative distribution function
     */
    inline double tdist_Q( double const x, double const nu ){ return gsl_cdf_tdist_Q( x, nu ); } 
    /**
     * C++ version of gsl_cdf_tdist_Pinv().
     * @param P A probability
     * @param nu The degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double tdist_Pinv( double const P, double const nu ){ return gsl_cdf_tdist_Pinv( P, nu ); } 
    /**
     * C++ version of gsl_cdf_tdist_Qinv().
     * @param Q A probability
     * @param nu The degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double tdist_Qinv( double const Q, double const nu ){ return gsl_cdf_tdist_Qinv( Q, nu ); } 
    /**
     * C++ version of gsl_cdf_fdist_P().
     * @param x A real value
     * @param nu1 The first degrees of freedom
     * @param nu2 The second degrees of freedom
     * @return The cumulative distribution function
     */
    inline double fdist_P( double const x, double const nu1, double const nu2 ){
      return gsl_cdf_fdist_P( x, nu1, nu2 ); } 
    /**
     * C++ version of gsl_cdf_fdist_Q().
     * @param x A real value
     * @param nu1 The first degrees of freedom
     * @param nu2 The second degrees of freedom
     * @return The cumulative distribution function
     */
    inline double fdist_Q( double const x, double const nu1, double const nu2 ){
      return gsl_cdf_fdist_Q( x, nu1, nu2 ); } 
    /**
     * C++ version of gsl_cdf_fdist_Pinv().
     * @param P A probability
     * @param nu1 The first degrees of freedom
     * @param nu2 The second degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double fdist_Pinv( double const P, double const nu1, double const nu2 ){
      return gsl_cdf_fdist_Pinv( P, nu1, nu2 ); } 
    /**
     * C++ version of gsl_cdf_fdist_Qinv().
     * @param Q A probability
     * @param nu1 The first degrees of freedom
     * @param nu2 The second degrees of freedom
     * @return The inverse cumulative distribution function
     */
    inline double fdist_Qinv( double const Q, double const nu1, double const nu2 ){
      return gsl_cdf_fdist_Qinv( Q, nu1, nu2 ); } 
    /**
     * C++ version of gsl_cdf_beta_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double beta_P( double const x, double const a, double const b ){
      return gsl_cdf_beta_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_beta_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double beta_Q( double const x, double const a, double const b ){
      return gsl_cdf_beta_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_beta_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double beta_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_beta_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_beta_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double beta_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_beta_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_flat_P().
     * @param x A real value
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return The cumulative distribution function
     */
    inline double flat_P( double const x, double const a, double const b ){
      return gsl_cdf_flat_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_flat_Q().
     * @param x A real value
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return The cumulative distribution function
     */
    inline double flat_Q( double const x, double const a, double const b ){
      return gsl_cdf_flat_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_flat_Pinv().
     * @param P A probability
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return The inverse cumulative distribution function
     */
    inline double flat_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_flat_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_flat_Qinv().
     * @param Q A probability
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return The inverse cumulative distribution function
     */
    inline double flat_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_flat_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_lognormal_P().
     * @param x A real value
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Parameter (standard deviation of corresponding normal distribution)
     * @return The cumulative distribution function
     */
    inline double lognormal_P( double const x, double const zeta, double const sigma ){
      return gsl_cdf_lognormal_P( x, zeta, sigma ); } 
    /**
     * C++ version of gsl_cdf_lognormal_Q().
     * @param x A real value
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Parameter (standard deviation of corresponding normal distribution)
     * @return The cumulative distribution function
     */
    inline double lognormal_Q( double const x, double const zeta, double const sigma ){
      return gsl_cdf_lognormal_Q( x, zeta, sigma ); } 
    /**
     * C++ version of gsl_cdf_lognormal_Pinv().
     * @param P A probability
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Parameter (standard deviation of corresponding normal distribution)
     * @return The inverse cumulative distribution function
     */
    inline double lognormal_Pinv( double const P, double const zeta, double const sigma ){
      return gsl_cdf_lognormal_Pinv( P, zeta, sigma ); } 
    /**
     * C++ version of gsl_cdf_lognormal_Qinv().
     * @param Q A probability
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Parameter (standard deviation of corresponding normal distribution)
     * @return The inverse cumulative distribution function
     */
    inline double lognormal_Qinv( double const Q, double const zeta, double const sigma ){
      return gsl_cdf_lognormal_Qinv( Q, zeta, sigma ); } 
    /**
     * C++ version of gsl_cdf_gumbel1_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gumbel1_P( double const x, double const a, double const b ){
      return gsl_cdf_gumbel1_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel1_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gumbel1_Q( double const x, double const a, double const b ){
      return gsl_cdf_gumbel1_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel1_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double gumbel1_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_gumbel1_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel1_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gumbel1_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_gumbel1_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel2_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gumbel2_P( double const x, double const a, double const b ){
      return gsl_cdf_gumbel2_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel2_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double gumbel2_Q( double const x, double const a, double const b ){
      return gsl_cdf_gumbel2_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel2_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double gumbel2_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_gumbel2_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_gumbel2_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double gumbel2_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_gumbel2_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_weibull_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double weibull_P( double const x, double const a, double const b ){
      return gsl_cdf_weibull_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_weibull_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double weibull_Q( double const x, double const a, double const b ){
      return gsl_cdf_weibull_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_weibull_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double weibull_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_weibull_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_weibull_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double weibull_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_weibull_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_pareto_P().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double pareto_P( double const x, double const a, double const b ){
      return gsl_cdf_pareto_P( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_pareto_Q().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The cumulative distribution function
     */
    inline double pareto_Q( double const x, double const a, double const b ){
      return gsl_cdf_pareto_Q( x, a, b ); } 
    /**
     * C++ version of gsl_cdf_pareto_Pinv().
     * @param P A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double pareto_Pinv( double const P, double const a, double const b ){
      return gsl_cdf_pareto_Pinv( P, a, b ); } 
    /**
     * C++ version of gsl_cdf_pareto_Qinv().
     * @param Q A probability
     * @param a The first parameter
     * @param b The second parameter
     * @return The inverse cumulative distribution function
     */
    inline double pareto_Qinv( double const Q, double const a, double const b ){
      return gsl_cdf_pareto_Qinv( Q, a, b ); } 
    /**
     * C++ version of gsl_cdf_logistic_P().
     * @param x A real value
     * @param a The parameter
     * @return The cumulative distribution function
     */
    inline double logistic_P( double const x, double const a ){
      return gsl_cdf_logistic_P( x, a ); } 
    /**
     * C++ version of gsl_cdf_logistic_Q().
     * @param x A real value
     * @param a The parameter
     * @return The cumulative distribution function
     */
    inline double logistic_Q( double const x, double const a ){ return gsl_cdf_logistic_Q( x, a ); } 
    /**
     * C++ version of gsl_cdf_logistic_Pinv().
     * @param P A probability
     * @param a The parameter
     * @return The inverse cumulative distribution function
     */
    inline double logistic_Pinv( double const P, double const a ){
      return gsl_cdf_logistic_Pinv( P, a ); } 
    /**
     * C++ version of gsl_cdf_logistic_Qinv().
     * @param Q A probability
     * @param a The parameter
     * @return The inverse cumulative distribution function
     */
    inline double logistic_Qinv( double const Q, double const a ){
      return gsl_cdf_logistic_Qinv( Q, a ); } 
    /**
     * C++ version of gsl_cdf_binomial_P().
     * @param k The number of successes
     * @param p The probability of success in one trial
     * @param n The number of trials
     * @return The cumulative distribution function
     */
    inline double binomial_P( unsigned int const k, double const p, unsigned int const n ){
      return gsl_cdf_binomial_P( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_binomial_Q().
     * @param k The number of successes
     * @param p The probability of success in one trial
     * @param n The number of trials
     * @return The cumulative distribution function
     */
    inline double binomial_Q( unsigned int const k, double const p, unsigned int const n ){
      return gsl_cdf_binomial_Q( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_poisson_P().
     * @param k The number of observations
     * @param mu The mean (parameter)
     * @return The cumulative distribution function
     */
    inline double poisson_P( unsigned int const k, double const mu ){
      return gsl_cdf_poisson_P( k, mu ); } 
    /**
     * C++ version of gsl_cdf_poisson_Q().
     * @param k The number of observations
     * @param mu The mean (parameter)
     * @return The cumulative distribution function
     */
    inline double poisson_Q( unsigned int const k, double const mu ){
      return gsl_cdf_poisson_Q( k, mu ); } 
    /**
     * C++ version of gsl_cdf_geometric_P().
     * @param k The number of observations
     * @param p The probability
     * @return The cumulative distribution function
     */
    inline double geometric_P( unsigned int const k, double const p ){
      return gsl_cdf_geometric_P( k, p ); } 
    /**
     * C++ version of gsl_cdf_geometric_Q().
     * @param k The number of observations
     * @param p The probability
     * @return The cumulative distribution function
     */
    inline double geometric_Q( unsigned int const k, double const p ){
      return gsl_cdf_geometric_Q( k, p ); } 
    /**
     * C++ version of gsl_cdf_negative_binomial_P().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n The number of successes
     * @return The cumulative distribution function
     */
    inline double negative_binomial_P( unsigned int const k, double const p, double const n ){
      return gsl_cdf_negative_binomial_P( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_negative_binomial_Q().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n The number of successes
     * @return The cumulative distribution function
     */
    inline double negative_binomial_Q( unsigned int const k, double const p, double const n ){
      return gsl_cdf_negative_binomial_Q( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_pascal_P().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n The number of successes
     * @return The cumulative distribution function
     */
    inline double pascal_P( unsigned int const k, double const p, unsigned int const n ){
      return gsl_cdf_pascal_P( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_pascal_Q().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n The number of successes
     * @return The cumulative distribution function
     */
    inline double pascal_Q( unsigned int const k, double const p, unsigned int const n ){
      return gsl_cdf_pascal_Q( k, p, n ); } 
    /**
     * C++ version of gsl_cdf_hypergeometric_P().
     * @param k An integer
     * @param n1 The first parameter
     * @param n2 The second parameter
     * @param t The number of samples
     * @return The cumulative distribution function
     */
    inline double hypergeometric_P( unsigned int const k, unsigned int const n1,
				    unsigned int const n2, unsigned int const t ){
      return gsl_cdf_hypergeometric_P( k, n1, n2, t ); } 
    /**
     * C++ version of gsl_cdf_hypergeometric_Q().
     * @param k An integer
     * @param n1 The first parameter
     * @param n2 The second parameter
     * @param t The number of samples
     * @return The cumulative distribution function
     */
    inline double hypergeometric_Q( unsigned int const k, unsigned int const n1,
				    unsigned int const n2, unsigned int const t ){
      return gsl_cdf_hypergeometric_Q( k, n1, n2, t ); }
  }
}

#endif
