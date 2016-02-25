/*
 * $Id: sf_synchrotron.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_SYNCHROTRON_HPP
#define CCGSL_SF_SYNCHROTRON_HPP

#include<gsl/gsl_sf_synchrotron.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_synchrotron_1_e().
     * First synchrotron function:
     *  synchrotron_1(x) = x Integral[ K_{5/3}(t), {t, x, Infinity}]
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW
     */
    inline int synchrotron_1_e( double const x, result& result ){
      return gsl_sf_synchrotron_1_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_synchrotron_1().
     * First synchrotron function:
     *  synchrotron_1(x) = x Integral[ K_{5/3}(t), {t, x, Infinity}]
     * @param x A real value
     * @return The function value
     */
    inline double synchrotron_1( double const x ){ return gsl_sf_synchrotron_1( x ); } 
    /**
     * C++ version of gsl_sf_synchrotron_2_e().
     * Second synchroton function:
     *   synchrotron_2(x) = x * K_{2/3}(x)
     * @param x A real value
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS or GSL_EDOM or GSL_EUNDRFLW
     */
    inline int synchrotron_2_e( double const x, result& result ){
      return gsl_sf_synchrotron_2_e( x, &result ); } 
    /**
     * C++ version of gsl_sf_synchrotron_2().
     * Second synchroton function:
     *   synchrotron_2(x) = x * K_{2/3}(x)
     * @param x A real value
     * @return The function value
     */
    inline double synchrotron_2( double const x ){ return gsl_sf_synchrotron_2( x ); }
  }
}

#endif
