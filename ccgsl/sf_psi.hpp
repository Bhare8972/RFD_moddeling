/*
 * $Id: sf_psi.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_PSI_HPP
#define CCGSL_SF_PSI_HPP

#include<gsl/gsl_sf_psi.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_psi_int_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Di-Gamma Function  psi(n) = psi(0,n)
     *
     * n > 0
     * @param n An integer
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int psi_int_e( int const n, result& result ){ return gsl_sf_psi_int_e( n, &result ); } 
    /**
     * C++ version of gsl_sf_psi_int().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Di-Gamma Function  psi(n) = psi(0,n)
     *
     * n > 0
     * @param n An integer
     * @return The function value
     */
    inline double psi_int( int const n ){ return gsl_sf_psi_int( n ); } 
    /**
     * C++ version of gsl_sf_psi_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Di-Gamma Function psi(x) = psi(0, x)
     *
     * x != 0.0, -1.0, -2.0, ...
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS  or GSL_EDOM  or GSL_ELOSS
     */
    inline int psi_e( double const x, result& result ){ return gsl_sf_psi_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_psi().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Di-Gamma Function psi(x) = psi(0, x)
     *
     * x != 0.0, -1.0, -2.0, ...
     * @param x A real value
     * @return The function value
     */
    inline double psi( double const x ){ return gsl_sf_psi( x ); } 
    /**
     * C++ version of gsl_sf_psi_1piy_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Di-Gamma Function Re[psi(1 + I y)]
     * @param y The imaginary part
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int psi_1piy_e( double const y, result& result ){ return gsl_sf_psi_1piy_e( y, &result ); } 
    /**
     * C++ version of gsl_sf_psi_1piy().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Di-Gamma Function Re[psi(1 + I y)]
     * @param y The imaginary part
     * @return The function value
     */
    inline double psi_1piy( double const y ){ return gsl_sf_psi_1piy( y ); } 
    /**
     * C++ version of gsl_sf_complex_psi_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * @param x Real part of complex number
     * @param y Imaginary part of complex number
     * @param result_re Real part of result as @c gsl::sf::result object
     * @param result_im Imaginary part of result as @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int complex_psi_e( double const x, double const y, result& result_re, result& result_im ){
      return gsl_sf_complex_psi_e( x, y, &result_re, &result_im ); } 
    /**
     * C++ version of gsl_sf_psi_1_int_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Di-Gamma Function psi(z) for general complex argument z = x + iy
     * @param n An integer
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int psi_1_int_e( int const n, result& result ){ return gsl_sf_psi_1_int_e( n, &result ); } 
    /**
     * C++ version of gsl_sf_psi_1_int().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Di-Gamma Function psi(z) for general complex argument z = x + iy
     * @param n An integer
     * @return The function value
     */
    inline double psi_1_int( int const n ){ return gsl_sf_psi_1_int( n ); } 
    /**
     * C++ version of gsl_sf_psi_1_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Tri-Gamma Function psi^(1)(n)
     *
     * n > 0
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int psi_1_e( double const x, result& result ){ return gsl_sf_psi_1_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_psi_1().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     * 
     * Tri-Gamma Function psi^(1)(n)
     *
     * n > 0
     * @param x A real value
     * @return The function value
     */
    inline double psi_1( double const x ){ return gsl_sf_psi_1( x ); } 
    /**
     * C++ version of gsl_sf_psi_n_e().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Poly-Gamma Function psi^(n)(x)
     *
     * n >= 0, x > 0.0
     * @param n An integer
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int psi_n_e( int const n, double const x, result& result ){ return gsl_sf_psi_n_e( n, x, &result ); } 
    /**
     * C++ version of gsl_sf_psi_n().
     * Poly-Gamma Functions
     *
     * psi(m,x) := (d/dx)^m psi(0,x) = (d/dx)^{m+1} log(gamma(x))
     *
     * Poly-Gamma Function psi^(n)(x)
     *
     * n >= 0, x > 0.0
     * @param n An integer
     * @param x A real value
     * @return The function value
     */
    inline double psi_n( int const n, double const x ){ return gsl_sf_psi_n( n, x ); }
  }
}

#endif
