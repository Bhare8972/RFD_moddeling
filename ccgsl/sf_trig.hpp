/*
 * $Id: sf_trig.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_TRIG_HPP
#define CCGSL_SF_TRIG_HPP

#include<gsl/gsl_sf_trig.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_sin_e().
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int sin_e( double x, result& result ){ return gsl_sf_sin_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_sin().
     * @param x A real value
     * @return The function value
     */
    inline double sin( double const x ){ return gsl_sf_sin( x ); } 
    /**
     * C++ version of gsl_sf_cos_e().
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int cos_e( double x, result& result ){ return gsl_sf_cos_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_cos().
     * @param x A real value
     * @return The function value
     */
    inline double cos( double const x ){ return gsl_sf_cos( x ); } 
    /**
     * C++ version of gsl_sf_hypot_e().
     * @param x A real value
     * @param y A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int hypot_e( double const x, double const y, result& result ){
      return gsl_sf_hypot_e( x, y, &result ); } 
    /**
     * C++ version of gsl_sf_hypot().
     * @param x A real value
     * @param y A real value
     * @return The function value
     */
    inline double hypot( double const x, double const y ){ return gsl_sf_hypot( x, y ); } 
    /**
     * C++ version of gsl_sf_complex_sin_e().
     * @param zr Real part of a complex number
     * @param zi Imaginary part of a complex number
     * @param szr The real part of the result as a @c gsl::sf::result object
     * @param szi The imaginary part of the result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EOVRFLW
     */
    inline int complex_sin_e( double const zr, double const zi, result& szr, result& szi ){
      return gsl_sf_complex_sin_e( zr, zi, &szr, &szi ); } 
    /**
     * C++ version of gsl_sf_complex_cos_e().
     * @param zr Real part of a complex number
     * @param zi Imaginary part of a complex number
     * @param czr The real part of the result as a @c gsl::sf::result object
     * @param czi The imaginary part of the result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EOVRFLW
     */
    inline int complex_cos_e( double const zr, double const zi, result& czr, result& czi ){
      return gsl_sf_complex_cos_e( zr, zi, &czr, &czi ); } 
    /**
     * C++ version of gsl_sf_complex_logsin_e().
     * @param zr Real part of a complex number
     * @param zi Imaginary part of a complex number
     * @param lszr The real part of the result as a @c gsl::sf::result object
     * @param lszi The imaginary part of the result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_ELOSS
     */
    inline int complex_logsin_e( double const zr, double const zi, result& lszr, result& lszi ){
      return gsl_sf_complex_logsin_e( zr, zi, &lszr, &lszi ); } 
    /**
     * C++ version of gsl_sf_sinc_e().
     * Sinc(x) = sin(pi x) / (pi x)
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int sinc_e( double x, result& result ){ return gsl_sf_sinc_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_sinc().
     * Sinc(x) = sin(pi x) / (pi x)
     * @param x A real value
     * @return The function value
     */
    inline double sinc( double const x ){ return gsl_sf_sinc( x ); } 
    /**
     * C++ version of gsl_sf_lnsinh_e().
     * Log(Sinh(x)), x > 0
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int lnsinh_e( double const x, result& result ){ return gsl_sf_lnsinh_e( x, &result ); }
    /**
     * C++ version of gsl_sf_lnsinh().
     * Log(Sinh(x)), x > 0
     * @param x A real value
     * @return The function value
     */
    inline double lnsinh( double const x ){ return gsl_sf_lnsinh( x ); } 
    /**
     * C++ version of gsl_sf_lncosh_e().
     * Log(Cosh(x))
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int lncosh_e( double const x, result& result ){ return gsl_sf_lncosh_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_lncosh().
     * Log(Cosh(x))
     * @param x A real value
     * @return The function value
     */
    inline double lncosh( double const x ){ return gsl_sf_lncosh( x ); } 
    /**
     * C++ version of gsl_sf_polar_to_rect().
     * @param r A real value (distance)
     * @param theta A real value (angle)
     * @param x A real value
     * @param y A real value
     * @return GSL_SUCCESS
     */
    inline int polar_to_rect( double const r, double const theta, result& x, result& y ){
      return gsl_sf_polar_to_rect( r, theta, &x, &y ); } 
    /**
     * C++ version of gsl_sf_rect_to_polar().
     * @param x A real value
     * @param y A real value
     * @param r The distance as a @c gsl::sf::result object
     * @param theta The angle as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_ELOSS
     */
    inline int rect_to_polar( double const x, double const y, result& r, result& theta ){
      return gsl_sf_rect_to_polar( x, y, &r, &theta ); } 
    /**
     * C++ version of gsl_sf_sin_err_e().
     * Sin(x) for quantity with an associated error.
     * @param x A real value
     * @param dx The error in @c x
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int sin_err_e( double const x, double const dx, result& result ){
      return gsl_sf_sin_err_e( x, dx, &result ); } 
    /**
     * C++ version of gsl_sf_cos_err_e().
     * Cos(x) for quantity with an associated error.
     * @param x A real value
     * @param dx The error in @c x
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int cos_err_e( double const x, double const dx, result& result ){
      return gsl_sf_cos_err_e( x, dx, &result ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_symm_e().
     * Force an angle to lie in the range (-pi,pi].
     * @param theta A real value (angle)
     * @return GSL_SUCCESS or GSL_ELOSS
     */
    inline int angle_restrict_symm_e( double* theta ){
      return gsl_sf_angle_restrict_symm_e( theta ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_symm().
     * Force an angle to lie in the range (-pi,pi].
     * @param theta A real value (angle)
     * @return The function value
     */
    inline double angle_restrict_symm( double const theta ){
      return gsl_sf_angle_restrict_symm( theta ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_pos_e().
     * Force an angle to lie in the range [0, 2pi)
     * @param theta A real value (angle)
     * @return GSL_SUCCESS or GSL_ELOSS
     */
    inline int angle_restrict_pos_e( double* theta ){
      return gsl_sf_angle_restrict_pos_e( theta ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_pos().
     * Force an angle to lie in the range [0, 2pi)
     * @param theta A real value (angle)
     * @return The function value
     */
    inline double angle_restrict_pos( double const theta ){
      return gsl_sf_angle_restrict_pos( theta ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_symm_err_e().
     * @param theta A real value (angle)
     * @param result The result as a @c gsl::sf::result object
     * @return Error code on failure
     */
    inline int angle_restrict_symm_err_e( double const theta, result& result ){
      return gsl_sf_angle_restrict_symm_err_e( theta, &result ); } 
    /**
     * C++ version of gsl_sf_angle_restrict_pos_err_e().
     * @param theta A real value (angle)
     * @param result The result as a @c gsl::sf::result object
     * @return Error code on failure
     */
    inline int angle_restrict_pos_err_e( double const theta, result& result ){
      return gsl_sf_angle_restrict_pos_err_e( theta, &result ); }
  }
}

#endif
