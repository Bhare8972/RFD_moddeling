/*
 * $Id: sf_log.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_LOG_HPP
#define CCGSL_SF_LOG_HPP

#include<gsl/gsl_sf_log.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_log_e().
     * @param x A real value
     * @param result The result as a @c gs::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int log_e( double const x, result& result ){ return gsl_sf_log_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_log().
     * @param x A real value
     * @return The function value
     */
    inline double log( double const x ){ return gsl_sf_log( x ); } 
    /**
     * C++ version of gsl_sf_log_abs_e().
     * Log(|x|)
     * @param x A real value
     * @param result The result as a @c gs::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int log_abs_e( double const x, result& result ){
      return gsl_sf_log_abs_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_log_abs().
     * Log(|x|)
     * @param x A real value
     * @return The function value
     */
    inline double log_abs( double const x ){ return gsl_sf_log_abs( x ); } 
    /**
     * C++ version of gsl_sf_complex_log_e().
     * Complex Logarithm
     *   exp(lnr + I theta) = zr + I zi
     * Returns argument in [-pi,pi].
     * @param zr Real part of a complex number
     * @param zi Imaginary part of a complex number
     * @param lnr The result (ln r) as a gsl::sf::result object
     * @param theta The result (theta) as a gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int complex_log_e( double const zr, double const zi, result& lnr, result& theta ){
      return gsl_sf_complex_log_e( zr, zi, &lnr, &theta ); } 
    /**
     * C++ version of gsl_sf_log_1plusx_e().
     * Log(1 + x)
     * @param x A real value
     * @param result The result as a @c gs::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int log_1plusx_e( double const x, result& result ){ return gsl_sf_log_1plusx_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_log_1plusx().
     * Log(1 + x)
     * @param x A real value
     * @return The function value
     */
    inline double log_1plusx( double const x ){ return gsl_sf_log_1plusx( x ); } 
    /**
     * C++ version of gsl_sf_log_1plusx_mx_e().
     * Log(1 + x) - x
     * @param x A real value
     * @param result The result as a @c gs::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int log_1plusx_mx_e( double const x, result& result ){
      return gsl_sf_log_1plusx_mx_e( x, &result ); }
    /**
     * C++ version of gsl_sf_log_1plusx_mx().
     * Log(1 + x) - x
     * @param x A real value
     * @return The function value
     */
    inline double log_1plusx_mx( double const x ){ return gsl_sf_log_1plusx_mx( x ); }
  }
}

#endif
