/*
 * $Id: sf_zeta.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_ZETA_HPP
#define CCGSL_SF_ZETA_HPP

#include<gsl/gsl_sf_zeta.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_zeta_int_e().
     * Riemann Zeta Function
     * zeta(n) = Sum[ k^(-n), {k,1,Infinity} ]
     *
     * n=integer, n != 1
     * @param n An integer
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EOVRFLW
     */
    inline int zeta_int_e( int const n, result& result ){
      return gsl_sf_zeta_int_e( n, &result ); } 
    /**
     * C++ version of gsl_sf_zeta_int().
     * Riemann Zeta Function
     * zeta(n) = Sum[ k^(-n), {k,1,Infinity} ]
     *
     * n=integer, n != 1
     * @param n An integer
     * @return The function value
     */
    inline double zeta_int( int const n ){ return gsl_sf_zeta_int( n ); } 
    /**
     * C++ version of gsl_sf_zeta_e().
     * Riemann Zeta Function
     * zeta(x) = Sum[ k^(-s), {k,1,Infinity} ], s != 1.0
     *
     * s != 1.0
     * @param s A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EOVRFLW
     */
    inline int zeta_e( double const s, result& result ){
      return gsl_sf_zeta_e( s, &result ); } 
    /**
     * C++ version of gsl_sf_zeta().
     * Riemann Zeta Function
     * zeta(x) = Sum[ k^(-s), {k,1,Infinity} ], s != 1.0
     *
     * s != 1.0
     * @param s A real value
     * @return The function value
     */
    inline double zeta( double const s ){ return gsl_sf_zeta( s ); } 
    /**
     * C++ version of gsl_sf_zetam1_e().
     * Riemann Zeta Function minus 1
     *   useful for evaluating the fractional part
     *   of Riemann zeta for large argument
     *
     * s != 1.0
     * @param s A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EOVRFLW
     */
    inline int zetam1_e( double const s, result& result ){
      return gsl_sf_zetam1_e( s, &result ); } 
    /**
     * C++ version of gsl_sf_zetam1().
     * Riemann Zeta Function minus 1
     *   useful for evaluating the fractional part
     *   of Riemann zeta for large argument
     *
     * s != 1.0
     * @param s A real value
     * @return The function value
     */
    inline double zetam1( double const s ){ return gsl_sf_zetam1( s ); } 
    /**
     * C++ version of gsl_sf_zetam1_int_e().
     * Riemann Zeta Function minus 1 for integer arg
     *   useful for evaluating the fractional part
     *   of Riemann zeta for large argument
     *
     * s != 1.0
     * @param s A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EOVRFLW
     */
    inline int zetam1_int_e( int const s, result& result ){
      return gsl_sf_zetam1_int_e( s, &result ); } 
    /**
     * C++ version of gsl_sf_zetam1_int().
     * Riemann Zeta Function minus 1 for integer arg
     *   useful for evaluating the fractional part
     *   of Riemann zeta for large argument
     *
     * s != 1.0
     * @param s A real value
     * @return The function value
     */
    inline double zetam1_int( int const s ){ return gsl_sf_zetam1_int( s ); } 
    /**
     * C++ version of gsl_sf_hzeta_e().
     * Hurwitz Zeta Function
     * zeta(s,q) = Sum[ (k+q)^(-s), {k,0,Infinity} ]
     *
     * s > 1.0, q > 0.0
     * @param s A real value
     * @param q A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW or GSL_EOVRFLW
     */
    inline int hzeta_e( double const s, double const q, result& result ){
      return gsl_sf_hzeta_e( s, q, &result ); } 
    /**
     * C++ version of gsl_sf_hzeta().
     * Hurwitz Zeta Function
     * zeta(s,q) = Sum[ (k+q)^(-s), {k,0,Infinity} ]
     *
     * s > 1.0, q > 0.0
     * @param s A real value
     * @param q A real value
     * @return The function value
     */
    inline double hzeta( double const s, double const q ){
      return gsl_sf_hzeta( s, q ); } 
    /**
     * C++ version of gsl_sf_eta_int_e().
     * Eta Function
     * eta(n) = (1-2^(1-n)) zeta(n)
     * @param n An integer
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EUNDRFLW or GSL_EOVRFLW
     */
    inline int eta_int_e( int n, result& result ){
      return gsl_sf_eta_int_e( n, &result ); } 
    /**
     * C++ version of gsl_sf_eta_int().
     * Eta Function
     * eta(n) = (1-2^(1-n)) zeta(n)
     * @param n An integer
     * @return The function value
     */
    inline double eta_int( int const n ){ return gsl_sf_eta_int( n ); } 
    /**
     * C++ version of gsl_sf_eta_e().
     * Eta Function
     * eta(s) = (1-2^(1-s)) zeta(s)
     * @param s A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EUNDRFLW or GSL_EOVRFLW
     */
    inline int eta_e( double const s, result& result ){
      return gsl_sf_eta_e( s, &result ); } 
    /**
     * C++ version of gsl_sf_eta().
     * Eta Function
     * eta(s) = (1-2^(1-s)) zeta(s)
     * @param s A real value
     * @return The function value
     */
    inline double eta( double const s ){ return gsl_sf_eta( s ); }
  }
}

#endif
