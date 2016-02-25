/*
 * $Id: fit.hpp 281 2012-08-26 14:35:20Z jdl3 $
 * Copyright (C) 2011, 2012 John D Lamb
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

#ifndef CCGSL_FIT_HPP
#define CCGSL_FIT_HPP

#include<cmath>
#include<gsl/gsl_fit.h>
#include"vector.hpp"

namespace gsl {
  /**
   * This namespace gives access to all the gsl_fit functions. Since the GNU Scientific library
   * does not have functions for univariate regression that work directly with vectors, we add
   * some here. You can use gsl::vector objects with non-unit strides in the overloaded functions.
   */
  namespace fit {
    /**
     * C++ version of gsl_fit_linear(). Univariate regression.
     * @param x An array of values of the independent variable.
     * @param xstride Stride of the independent variable array
     * @param y An array of values of the dependent variable.
     * @param ystride Stride of the dependent variable
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param sumsq Residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int linear( double const* x, size_t const xstride, double const* y, size_t const ystride,
		       size_t const n, double* c0, double* c1, double* cov00, double* cov01, double* cov11,
		       double* sumsq ){
      return gsl_fit_linear( x, xstride, y, ystride, n, c0, c1, cov00, cov01, cov11, sumsq ); }
    
    /**
     * C++ version of gsl_fit_wlinear(). Univariate weighted regression.
     * @param x An array of values of the independent variable.
     * @param xstride Stride of the independent variable array
     * @param w An array of weights for the data.
     * @param wstride Stride of the array of weights
     * @param y An array of values of the dependent variable.
     * @param ystride Stride of the dependent variable
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param chisq Weighted residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int wlinear( double const* x, size_t const xstride, double const* w, size_t const wstride,
			double const* y, size_t const ystride, size_t const n, double* c0, double* c1,
			double* cov00, double* cov01, double* cov11, double* chisq ){
      return gsl_fit_wlinear( x, xstride, w, wstride, y, ystride, n, c0, c1, cov00, cov01,
			      cov11, chisq ); }

    /**
     * C++ version of gsl_fit_linear_est(). Estimate fit from regression model.
     * @param x Value of the independent variable.
     * @param c0 First coefficient.
     * @param c1 Second coefficient.
     * @param cov00 First variance from covariance matrix.
     * @param cov01 Covariance from covariance matrix.
     * @param cov11 Second variance from covariance matrix.
     * @param y Predicted value of the dependent variable.
     * @param y_err Estimated standard deviation in y.
     * @return Error code on failure.
     */
    inline int linear_est( double const x, double const c0, double const c1, double const cov00,
			   double const cov01, double const cov11, double* y, double* y_err ){
      return gsl_fit_linear_est( x, c0, c1, cov00, cov01, cov11, y, y_err ); }
    
    /**
     * C++ version of gsl_fit_mul(). Univariate regression with no constant.
     * @param x An array of values of the independent variable.
     * @param xstride Stride of the independent variable array
     * @param y An array of values of the dependent variable.
     * @param ystride Stride of the dependent variable
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param sumsq Residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int mul( double const* x, size_t const xstride, double const* y, size_t const ystride,
		    size_t const n, double* c1, double* cov11, double* sumsq ){
      return gsl_fit_mul( x, xstride, y, ystride, n, c1, cov11, sumsq ); }
    
    /**
     * C++ version of gsl_fit_wmul(). Weighted univariate regression with no constant.
     * @param x An array of values of the independent variable.
     * @param xstride Stride of the independent variable array
     * @param w An array of weights for the data.
     * @param wstride Stride of the array of weights
     * @param y An array of values of the dependent variable.
     * @param ystride Stride of the dependent variable
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param chisq Weighted residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int wmul( double const* x, size_t const xstride, double const* w, size_t const wstride,
		     double const* y, size_t const ystride, size_t const n, double* c1, double* cov11,
		     double* chisq ){
      return gsl_fit_wmul( x, xstride, w, wstride, y, ystride, n, c1, cov11, chisq ); }

    /**
     * C++ version of gsl_fit_mul_est(). Estimate fit from regression model.
     * @param x Value of the independent variable.
     * @param c1 Coefficient.
     * @param cov11 Covariance.
     * @param y Predicted value of the dependent variable.
     * @param y_err Estimated standard deviation in y.
     * @return Error code on failure.
     */
    inline int mul_est( double const x, double const c1, double const cov11, double* y, double* y_err ){
      return gsl_fit_mul_est( x, c1, cov11, y, y_err ); } 

    // Versions without stride arguments

    /**
     * C++ version of gsl_fit_linear(). Univariate regression.
     * @param x An array of values of the independent variable.
     * @param y An array of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param sumsq Residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int linear( double const* x, double const* y,
		       size_t const n, double* c0, double* c1, double* cov00, double* cov01, double* cov11,
		       double* sumsq ){
      return gsl_fit_linear( x, 1, y, 1, n, c0, c1, cov00, cov01, cov11, sumsq ); }
    
    /**
     * C++ version of gsl_fit_wlinear(). Univariate weighted regression.
     * @param x An array of values of the independent variable.
     * @param w An array of weights for the data.
     * @param y An array of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param chisq Weighted residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int wlinear( double const* x, double const* w,
			double const* y, size_t const n, double* c0, double* c1,
			double* cov00, double* cov01, double* cov11, double* chisq ){
      return gsl_fit_wlinear( x, 1, w, 1, y, 1, n, c0, c1, cov00, cov01, cov11, chisq ); }

    /**
     * C++ version of gsl_fit_mul(). Univariate regression with no constant.
     * @param x An array of values of the independent variable.
     * @param y An array of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param sumsq Residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int mul( double const* x, double const* y,
		    size_t const n, double* c1, double* cov11, double* sumsq ){
      return gsl_fit_mul( x, 1, y, 1, n, c1, cov11, sumsq ); }
    
    /**
     * C++ version of gsl_fit_wmul(). Weighted univariate regression with no constant.
     * @param x An array of values of the independent variable.
     * @param w An array of weights for the data.
     * @param y An array of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param chisq Weighted residual sum of squares (return value).
     * @return Error code on failure.
     */
    inline int wmul( double const* x, double const* w,
		     double const* y, size_t const n, double* c1, double* cov11,
		     double* chisq ){
      return gsl_fit_wmul( x, 1, w, 1, y, 1, n, c1, cov11, chisq ); }

    // CCGSL additions
    
    /**
     * C++ version of gsl_fit_linear() for gsl::vector objects. Univariate regression.
     * @param x A vector of values of the independent variable.
     * @param y A vector of values of the dependent variable.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param sumsq Residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    inline int linear( gsl::vector const& x, gsl::vector const& y, double* c0, double* c1,
		       double* cov00, double* cov01, double* cov11, double* sumsq, size_t const n = 0 ){
      return linear( x.data(), x.get()->stride, y.data(), y.get()->stride,
		     n == 0 ? y.size() / y.get()->stride : n, c0, c1, cov00, cov01, cov11, sumsq ); }

    /**
     * C++ version of gsl_fit_wlinear() for gsl::vector objects. Univariate weighted regression.
     * @param x A vector of values of the independent variable.
     * @param w A vector of weights for the data.
     * @param y A vector of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param chisq Weighted residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    inline int wlinear( gsl::vector const& x, gsl::vector const& w,
			gsl::vector const& y, double* c0, double* c1, double* cov00, double* cov01,
			double* cov11, double* chisq, size_t const n = 0 ){
      return wlinear( x.data(), x.get()->stride, w.data(), w.get()->stride,
		      y.data(), y.get()->stride, n == 0 ? y.size() / y.get()->stride : n, c0, c1,
		      cov00, cov01, cov11, chisq ); }
    
    /**
     * C++ version of gsl_fit_mul() for gsl::vector objects. Univariate regression with no constant.
     * @param x A vector of values of the independent variable.
     * @param y A vector of values of the dependent variable.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param sumsq Residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    inline int mul( gsl::vector const& x, gsl::vector const& y, double* c1, double* cov11,
		    double* sumsq, size_t const n = 0 ){
      return mul( x.data(), x.get()->stride, y.data(), y.get()->stride,
		  n == 0 ? y.size() / y.get()->stride : n, c1, cov11, sumsq ); }

    /**
     * C++ version of gsl_fit_wmul() for gsl::vector objects.
     * Weighted univariate regression with no constant.
     * @param x A vector of values of the independent variable.
     * @param w A vector of weights for the data.
     * @param y A vector of values of the dependent variable.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param chisq Weighted residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    inline int wmul( gsl::vector const& x, gsl::vector const& w,
		     gsl::vector const& y, double* c1, double* cov11, double* chisq, size_t const n = 0 ){
      return wmul( x.data(), x.get()->stride, w.data(), w.get()->stride,
		   y.data(), y.get()->stride, n == 0 ? y.size() / y.get()->stride : n,
		   c1, cov11, chisq ); }

    // Template versions
    /**
     * C++ version of gsl_fit_linear() for std::vector and the like. Univariate regression.
     * @param x A vector of values of the independent variable.
     * @param y A vector of values of the dependent variable.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param sumsq Residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    template<typename ARRAY>
    inline int linear( ARRAY const& x, ARRAY const& y, double* c0, double* c1,
		       double* cov00, double* cov01, double* cov11, double* sumsq, size_t const n = 0 ){
      return linear( x.data(), 1, y.data(), 1,
		     n == 0 ? y.size() : n, c0, c1, cov00, cov01, cov11, sumsq ); }

    /**
     * C++ version of gsl_fit_wlinear() for std::vector and the like. Univariate weighted regression.
     * @param x A vector of values of the independent variable.
     * @param w A vector of weights for the data.
     * @param y A vector of values of the dependent variable.
     * @param n Number of elements in arrays of dependent and independent variables.
     * @param c0 First (constant coefficient) (return value).
     * @param c1 Second coefficient (return value).
     * @param cov00 First variance in covariance matrix (return value).
     * @param cov01 Covariance from covariance matrix (return value).
     * @param cov11 Second variance in covariance matrix (return value).
     * @param chisq Weighted residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    template<typename ARRAY>
    inline int wlinear( ARRAY const& x, ARRAY const& w,
			ARRAY const& y, double* c0, double* c1, double* cov00, double* cov01,
			double* cov11, double* chisq, size_t const n = 0 ){
      return wlinear( x.data(), 1, w.data(), 1, y.data(), 1, n == 0 ? y.size() : n, c0, c1,
		      cov00, cov01, cov11, chisq ); }
    
    /**
     * C++ version of gsl_fit_mul() for std::vector and the like. Univariate regression with no constant.
     * @param x A vector of values of the independent variable.
     * @param y A vector of values of the dependent variable.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param sumsq Residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    template<typename ARRAY>
    inline int mul( ARRAY const& x, ARRAY const& y, double* c1, double* cov11,
		    double* sumsq, size_t const n = 0 ){
      return mul( x.data(), 1, y.data(), 1, n == 0 ? y.size() : n, c1, cov11, sumsq ); }

    /**
     * C++ version of gsl_fit_wmul() for std::vector and the like.
     * Weighted univariate regression with no constant.
     * @param x A vector of values of the independent variable.
     * @param w A vector of weights for the data.
     * @param y A vector of values of the dependent variable.
     * @param c1 Coefficient (return value).
     * @param cov11 Variance (return value)
     * @param chisq Weighted residual sum of squares (return value).
     * @param n Number of elements in vectors of dependent and independent variables (default of
     * zero is substituted with vector size).
     * @return Error code on failure.
     */
    template<typename ARRAY>
    inline int wmul( ARRAY const& x, ARRAY const& w,
		     ARRAY const& y, double* c1, double* cov11, double* chisq, size_t const n = 0 ){
      return wmul( x.data(), 1, w.data(), 1, y.data(), 1, n == 0 ? y.size() : n,
		   c1, cov11, chisq ); }
  }
}
#endif
