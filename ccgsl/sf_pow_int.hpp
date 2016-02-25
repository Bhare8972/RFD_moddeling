/*
 * $Id: sf_pow_int.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_POW_INT_HPP
#define CCGSL_SF_POW_INT_HPP

#include<gsl/gsl_sf_pow_int.h>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * C++ version of gsl_sf_pow_int_e().
     * @param x A real value
     * @param n An integer
     * @param result The result as a @c gsl::sf::result object
     * @return GSL_SUCCESS
     */
    inline int pow_int_e( double x, int n, result& result ){ return gsl_sf_pow_int_e( x, n, &result ); } 
    /**
     * C++ version of gsl_sf_pow_int().
     * @param x A real value
     * @param n An integer
     * @return The nth power of x
     */
    inline double pow_int( double const x, int const n ){ return gsl_sf_pow_int( x, n ); }
  }
}

#endif
