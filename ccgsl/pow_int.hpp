/*
 * $Id: pow_int.hpp 68 2012-01-08 17:35:44Z jdl3 $
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

#ifndef CCGSL_POW_INT_HPP
#define CCGSL_POW_INT_HPP

#include<gsl/gsl_pow_int.h>

namespace gsl {
    /**
     * C++ version of gsl_pow_2().
     * @param x A double.
     * @return \f$x^2\f$.
     */
    inline double pow_2( double const x ){ return gsl_pow_2( x ); }

    /**
     * C++ version of gsl_pow_3().
     * @param x A double.
     * @return \f$x^3\f$.
     */
    inline double pow_3( double const x ){ return gsl_pow_3( x ); }

    /**
     * C++ version of gsl_pow_4().
     * @param x A double.
     * @return \f$x^4\f$.
     */
    inline double pow_4( double const x ){ return gsl_pow_4( x ); }

    /**
     * C++ version of gsl_pow_5().
     * @param x A double.
     * @return \f$x^5\f$.
     */
    inline double pow_5( double const x ){ return gsl_pow_5( x ); }

    /**
     * C++ version of gsl_pow_6().
     * @param x A double.
     * @return \f$x^6\f$.
     */
    inline double pow_6( double const x ){ return gsl_pow_6( x ); }

    /**
     * C++ version of gsl_pow_7().
     * @param x A double.
     * @return \f$x^7\f$.
     */
    inline double pow_7( double const x ){ return gsl_pow_7( x ); } 

    /**
     * C++ version of gsl_pow_8().
     * @param x A double.
     * @return \f$x^8\f$.
     */
    inline double pow_8( double const x ){ return gsl_pow_8( x ); } 

    /**
     * C++ version of gsl_pow_9().
     * @param x A double.
     * @return \f$x^9\f$.
     */
    inline double pow_9( double const x ){ return gsl_pow_9( x ); } 

    /**
     * C++ version of gsl_pow_int().
     * @param x A double.
     * @param n An integer.
     * @return \f$x^n\f$.
     */
    inline double pow_int( double x, int n ){ return gsl_pow_int( x, n ); }

    /**
     * C++ version of gsl_pow_uint().
     * @param x A double.
     * @param n A nonnegative integer.
     * @return \f$x^n\f$.
     */
    inline double pow_uint( double x, unsigned int n ){ return gsl_pow_uint( x, n ); }
}
#endif
