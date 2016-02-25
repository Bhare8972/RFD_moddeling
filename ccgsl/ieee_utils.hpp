/*
 * $Id: ieee_utils.hpp 230 2012-08-08 19:41:17Z jdl3 $
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

#ifndef CCGSL_IEEE_UTILS_HPP
#define CCGSL_IEEE_UTILS_HPP

#include<gsl/gsl_ieee_utils.h>

namespace gsl {
  /**
   * IEEE floating point arithmetic representations
   */
  namespace ieee {
    /**
     * Type definition.
     */
    typedef gsl_ieee_float_rep float_rep;
    /**
     * Type definition.
     */
    typedef gsl_ieee_double_rep double_rep;
    /**
     * C++ version of gsl_ieee_printf_float().
     * @param x A floating point number
     */
    inline void printf_float( float const* x ){ gsl_ieee_printf_float( x ); }

    /**
     * C++ version of gsl_ieee_printf_double().
     * @param x A floating point number
     */
    inline void printf_double( double const* x ){ gsl_ieee_printf_double( x ); }

    /**
     * C++ version of gsl_ieee_fprintf_float().
     * @param stream A file
     * @param x A floating point number
     */
    inline void fprintf_float( FILE* stream, float const* x ){ gsl_ieee_fprintf_float( stream, x ); }

    /**
     * C++ version of gsl_ieee_fprintf_double().
     * @param stream A file
     * @param x A floating point number
     */
    inline void fprintf_double( FILE* stream, double const* x ){ gsl_ieee_fprintf_double( stream, x ); }

    /**
     * C++ version of gsl_ieee_float_to_rep().
     * @param x A floating point number
     * @param r A float representation [return value]
     */
    inline void float_to_rep( float const* x, float_rep* r ){
      gsl_ieee_float_to_rep( x, r ); }

    /**
     * C++ version of gsl_ieee_double_to_rep().
     * @param x A floating point numbe
     * @param r A double representation [return value]
     */
    inline void double_to_rep( double const* x, double_rep* r ){
      gsl_ieee_double_to_rep( x, r ); }

    /**
     * C++ version of gsl_ieee_env_setup().
     */
    inline void env_setup(){ gsl_ieee_env_setup(); }

    /**
     * C++ version of gsl_ieee_read_mode_string().
     * @param description Mode description
     * @param precision The precision [return value]
     * @param rounding The rounding type [return value]
     * @param exception_mask The exception mask [return value]
     * @return Error code on failure
     */
    inline int read_mode_string( char const* description, int* precision, int* rounding,
				 int* exception_mask ){
      return gsl_ieee_read_mode_string( description, precision, rounding, exception_mask ); }

    /**
     * C++ version of gsl_ieee_set_mode().
     * @param precision The precision
     * @param rounding The rounding type
     * @param exception_mask The exception mask
     * @return Error code on failure
     */
    inline int set_mode( int precision, int rounding, int exception_mask ){
      return gsl_ieee_set_mode( precision, rounding, exception_mask ); }

  }
}
#endif
