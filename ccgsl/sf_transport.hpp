/*
 * $Id: sf_transport.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_TRANSPORT_HPP
#define CCGSL_SF_TRANSPORT_HPP

#include<gsl/gsl_sf_transport.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_transport_2_e().
     * Transport function:
     *   J(2,x) := Integral[ t^2 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM
     */
    inline int transport_2_e( double const x, result& result ){
      return gsl_sf_transport_2_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_transport_2().
     * Transport function:
     *   J(2,x) := Integral[ t^2 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @return The function value
     */
    inline double transport_2( double const x ){ return gsl_sf_transport_2( x ); } 
    /**
     * C++ version of gsl_sf_transport_3_e().
     * Transport function:
     *   J(3,x) := Integral[ t^3 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW
     */
    inline int transport_3_e( double const x, result& result ){
      return gsl_sf_transport_3_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_transport_3().
     * Transport function:
     *   J(3,x) := Integral[ t^3 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @return The function value
     */
    inline double transport_3( double const x ){ return gsl_sf_transport_3( x ); } 
    /**
     * C++ version of gsl_sf_transport_4_e().
     * Transport function:
     *   J(4,x) := Integral[ t^4 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW
     */
    inline int transport_4_e( double const x, result& result ){
      return gsl_sf_transport_4_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_transport_4().
     * Transport function:
     *   J(4,x) := Integral[ t^4 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @return The function value
     */
    inline double transport_4( double const x ){ return gsl_sf_transport_4( x ); } 
    /**
     * C++ version of gsl_sf_transport_5_e().
     * Transport function:
     *   J(5,x) := Integral[ t^5 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW
     */
    inline int transport_5_e( double const x, result& result ){
      return gsl_sf_transport_5_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_transport_5().
     * Transport function:
     *   J(5,x) := Integral[ t^5 e^t /(e^t - 1)^2, {t,0,x}]
     * @param x A real value
     * @return The function value
     */
    inline double transport_5( double const x ){ return gsl_sf_transport_5( x ); }
  }
}

#endif
