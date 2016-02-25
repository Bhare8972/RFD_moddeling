/*
 * $Id: deriv.hpp 236 2012-08-08 19:47:53Z jdl3 $
 * Copyright (C) 2012 John D Lamb
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

#ifndef CCGSL_DERIV_HPP
#define CCGSL_DERIV_HPP

#include<gsl/gsl_deriv.h>
#include<ccgsl/function_scl.hpp>

namespace gsl {
  /**
   * First-order numerical differentiation. The gsl_function* pointers can be replaced
   * with gsl::function_scl pointers because gsl::function_scl is a derived class of
   * gsl_function.
   */
  namespace deriv {
    /**
     * C++ version of gsl_deriv_central().
     * @param f A function of one variable
     * @param x Value at which function is to be differentiated
     * @param h A small value: std::sqrt( std::numeric_limits<double>::epsilon() ) is a
     * reasonable choice in most cases
     * @param result An estimate of the derivative at x
     * @param abserr Estimate of the error in the estimated derivative
     * @return Error code on failure
     */
    inline int central( gsl_function const* f, double x, double h, double* result, double* abserr ){
      return gsl_deriv_central( f, x, h, result, abserr ); }

    /**
     * C++ version of gsl_deriv_backward().
     * @param f A function of one variable
     * @param x Value at which function is to be differentiated
     * @param h A small value: std::sqrt( std::numeric_limits<double>::epsilon() ) is a
     * reasonable choice in most cases
     * @param result An estimate of the derivative at x
     * @param abserr Estimate of the error in the estimated derivative
     * @return Error code on failure
     */
    inline int backward( gsl_function const* f, double x, double h, double* result, double* abserr ){
      return gsl_deriv_backward( f, x, h, result, abserr ); }

    /**
     * C++ version of gsl_deriv_forward().
     * @param f A function of one variable
     * @param x Value at which function is to be differentiated
     * @param h A small value: std::sqrt( std::numeric_limits<double>::epsilon() ) is a
     * reasonable choice in most cases
     * @param result An estimate of the derivative at x
     * @param abserr Estimate of the error in the estimated derivative
     * @return Error code on failure
     */
    inline int forward( gsl_function const* f, double x, double h, double* result, double* abserr ){
      return gsl_deriv_forward( f, x, h, result, abserr );
    }
  }
}
#endif
