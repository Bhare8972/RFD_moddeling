/*
 * $Id: sys.hpp 67 2012-01-08 15:36:19Z jdl3 $
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

#ifndef CCGSL_SYS_HPP
#define CCGSL_SYS_HPP

#include<gsl/gsl_sys.h>

namespace gsl {
  /**
   * C++ version of gsl_log1p(). Accurate for small x.
   * @param x A real value (should be greater than -1)
   * @return \f$\log(1+x)\f$.
   */
  inline double log1p( double const x ){ return gsl_log1p( x ); }

  /**
   * C++ version of gsl_expm1(). Accurate for small x.
   * @param x A real value.
   * @return \f$\exp x - 1\f$.
   */
  inline double expm1( double const x ){ return gsl_expm1( x ); }

  /**
   * C++ version of gsl_hypot(). Avoids overflow.
   * @param x A real value.
   * @param y A real value.
   * @return \f$\sqrt{x^2+y^2}\f$.
   */
  inline double hypot( double const x, double const y ){ return gsl_hypot( x, y ); }

  /**
   * C++ version of gsl_hypot3(). Avoids overflow.
   * @param x A real value.
   * @param y A real value.
   * @param z A real value.
   * @return \f$\sqrt{x^2+y^2+z^2}\f$.
   */
  inline double hypot3( double const x, double const y, double const z ){ return gsl_hypot3( x, y, z ); }

  /**
   * C++ version of gsl_acosh().
   * @param x A real value.
   * @return acosh( x ).
   */
  inline double acosh( double const x ){ return gsl_acosh( x ); }

  /**
   * C++ version of gsl_asinh().
   * @param x A real value.
   * @return asinh( x ).
   */
  inline double asinh( double const x ){ return gsl_asinh( x ); }

  /**
   * C++ version of gsl_atanh().
   * @param x A real value.
   * @return atanh( x ).
   */
  inline double atanh( double const x ){ return gsl_atanh( x ); }

  /**
   * C++ version of gsl_isnan().
   * @param x A double.
   * @return 0 for NaN; 1 otherwise.
   */
  inline int is_nan( double const x ){ return gsl_isnan( x ); }

  /**
   * C++ version of gsl_isinf().
   * @param x A double.
   * @return 1 for infinite; 0 otherwise.
   */
  inline int is_inf( double const x ){ return gsl_isinf( x ); }

  /**
   * C++ version of gsl_finite().
   * @param x A double.
   * @return 1 for finite; 0 otherwise.
   */
  inline int finite( double const x ){ return gsl_finite( x ); }

  /**
   * C++ version of gsl_nan().
   * @return NaN.
   */
  inline double nan(){ return gsl_nan(); }

  /**
   * C++ version of gsl_posinf().
   * @return \f$\infty\f$.
   */
  inline double posinf(){ return gsl_posinf(); }

  /**
   * C++ version of gsl_neginf().
   * @return \f$-\infty\f$.
   */
  inline double neginf(){ return gsl_neginf(); }

  /**
   * C++ version of gsl_fdiv().
   * @param x A real value.
   * @param y A real value.
   * @return x / y.
   */
  inline double fdiv( double const x, double const y ){ return gsl_fdiv( x, y ); }

  /**
   * C++ version of gsl_coerce_double().
   * @param x A double.
   * @return A double.
   */
  inline double coerce_double( double const x ){ return gsl_coerce_double( x ); }

  /**
   * C++ version of gsl_coerce_float().
   * @param x A float.
   * @return A float.
   */
  inline float coerce_float( float const x ){ return gsl_coerce_float( x ); }

  /**
   * C++ version of gsl_coerce_long_double().
   * @param x A long double.
   * @return A long double.
   */
  inline long double coerce_long_double( long double const x ){ return gsl_coerce_long_double( x ); }

  /**
   * C++ version of gsl_ldexp().
   * @param x A real value.
   * @param e An integer.
   * @return \f$x\cdot2^e\f$.
   */
  inline double ldexp( double const x, int const e ){ return gsl_ldexp( x, e ); }

  /**
   * C++ version of gsl_frexp().
   * @param x A real value.
   * @param e An integer (return value ).
   * @return A value \f$f\in[0.5,1)\f$ (and \f$e\f$) such that \f$x=f\cdot2^e\f$.
   */
  inline double frexp( double const x, int* e ){ return gsl_frexp( x, e ); }

  /**
   * C++ version of gsl_fcmp(). Test if x1 and x2 are approximately equal to a relative
   * accuracy epsilon.
   * @param x1 A real value.
   * @param x2 A real value.
   * @param epsilon A positive real value.
   * @return 0 if x1 and x2 are approximately equal; 1 if x1 > x2; -1 if x1 < x2.
   */
  inline int fcmp( double const x1, double const x2, double const epsilon ){
    return gsl_fcmp( x1, x2, epsilon ); }
}
#endif
