/*
 * $Id: math.hpp 64 2012-01-08 15:32:04Z jdl3 $
 * Copyright (C) 2010, 2011, 2012 John D Lamb
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

#ifndef CCGSL_MATH_HPP
#define CCGSL_MATH_HPP

#include<gsl/gsl_math.h>
#include"function_scl.hpp"
#include"function_fdf.hpp"
#include"sys.hpp"

namespace gsl {
  /**
   * Mathematical constants.
   */
  namespace math {
    /**
     * e
     */
    double const E = M_E;
    /**
     * \f$\log_2(\mathrm{e})\f$.
     */
    double const LOG2E = M_LOG2E;
    /**
     * \f$\log_{10}(\mathrm{e})\f$.
     */
    double const LOG10E = M_LOG10E;
    /**
     * \f$\sqrt{2}\f$.
     */
    double const SQRT2 = M_SQRT2;
    /**
     * \f$\sqrt{1/2}\f$.
     */
    double const SQRT1_2 = M_SQRT1_2;
    /**
     * \f$\sqrt{3}\f$.
     */
    double const SQRT3 = M_SQRT3;
    /**
     * \f$\pi\f$.
     */
    double const PI = M_PI;
    /**
     * \f$\pi/2\f$.
     */
    double const PI_2 = M_PI_2;
    /**
     * \f$\pi/4\f$.
     */
    double const PI_4 = M_PI_4;
    /**
     * \f$\sqrt{\pi}\f$.
     */
    double const SQRTPI = M_SQRTPI;
    /**
     * \f$2\sqrt{\pi}\f$.
     */
    double const TWO_SQRTPI = M_2_SQRTPI;
    /**
     * \f$1/\sqrt{\pi}\f$.
     */
    double const ONE_PI = M_1_PI;
    /**
     * \f$2/\sqrt{\pi}\f$.
     */
    double const TWO_PI = M_2_PI;
    /**
     * \f$\log(10)\f$.
     */
    double const LN10 = M_LN10;
    /**
     * \f$\log(2)\f$.
     */
    double const LN2 = M_LN2;
    /**
     * \f$\log(\pi)\f$.
     */
    double const LNPI = M_LNPI;
    /**
     * Euler's constant.
     */
    double const EULER = M_EULER;
  }


  /**
   * Check if odd: only really makes sense for integral types.
   * @param n The value to be tested.
   * @return @c true for odd, @c false for even.
   */
  template<typename T>
  inline bool is_odd( T const& n ){ return n & 1; }
  
  /**
   * Check if even: only really makes sense for integral types.
   * @param n The value to be tested.
   * @return @c true for even, @c false for odd.
   */
  template<typename T>
  inline bool is_even( T const& n ){ return not (n & 1); }
  
  /**
   * Find the sign of x: returns +1 for a value of zero to be
   * consistent with GNU Scientific Library.
   * @param x The value to be tested.
   * @return +1 if x >= 0 and -1 otherwise.
   */
  template<typename T>
  inline short sign( T const& x ){ return x >= 0 ? 1 : -1; }

  /**
   * Variation on gsl_finite: returns 1 for a real number, 0 for
   * infinite or NaN.
   * @param x The value to be tested.
   * @return 1 for a real number; 0 otherwise.
   */
  inline int is_real( double const x ){ return gsl_finite( x ); }

}

#endif
