/*
 * $Id: const_num.hpp 225 2012-08-08 18:15:27Z jdl3 $
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

#ifndef CCGSL_CONST_NUM_HPP
#define CCGSL_CONST_NUM_HPP

#include<gsl/gsl_const_num.h>

namespace gsl {
  /**
   * Various constants. This namespace is fairly pointless since all the
   * constants are probably more sensibly use through the GSL constants, but it is here
   * for completeness.
   */
  namespace const_num {
    /**
     * @return The fine structure constant
     */
    inline double const fine_structure(){ return GSL_CONST_NUM_FINE_STRUCTURE; }
    /**
     * @return Avogadro's number \f$N_{\mbox{a}}\f$
     */
    inline double const Avogadro(){ return GSL_CONST_NUM_AVOGADRO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const yotta(){ return GSL_CONST_NUM_YOTTA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const zetta(){ return GSL_CONST_NUM_ZETTA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const exa(){ return GSL_CONST_NUM_EXA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const peta(){ return GSL_CONST_NUM_PETA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const tera(){ return GSL_CONST_NUM_TERA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const giga(){ return GSL_CONST_NUM_GIGA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const mega(){ return GSL_CONST_NUM_MEGA; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const kilo(){ return GSL_CONST_NUM_KILO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const milli(){ return GSL_CONST_NUM_MILLI; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const micro(){ return GSL_CONST_NUM_MICRO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const nano(){ return GSL_CONST_NUM_NANO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const pico(){ return GSL_CONST_NUM_PICO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const femto(){ return GSL_CONST_NUM_FEMTO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const atto(){ return GSL_CONST_NUM_ATTO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const zepto(){ return GSL_CONST_NUM_ZEPTO; }
    /**
     * @return The dimensionless scaling factor
     */
    inline double const yocto(){ return GSL_CONST_NUM_YOCTO; }
  }
}
#endif
