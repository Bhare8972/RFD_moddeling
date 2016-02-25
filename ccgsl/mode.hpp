/*
 * $Id: mode.hpp 199 2012-08-05 19:11:05Z jdl3 $
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

#ifndef CCGSL_MODE_HPP
#define CCGSL_MODE_HPP

#include<gsl/gsl_mode.h>

namespace gsl {
  /**
   * A typedef for @c gsl_mode_t
   */
  typedef gsl_mode_t mode_t;
  /**
   * This namespace handles @c gsl_mode for functions that use a mode.
   */
  namespace mode {
    /**
     * Predefined mode. This is the default.
     */
    inline mode_t const DOUBLE(){ return 0; }
    /**
     * Predefined mode. 
     */
    inline mode_t const SINGLE(){ return 1; }
    /**
     * Predefined mode. 
     */
    inline mode_t const APPROX(){ return 2; }
    /**
     * Extract precision from 3 low-order bits of mode.
     * @param mt A @c mode_t
     * @return precision
     */
    inline unsigned int PREC( mode_t mt ){ return GSL_MODE_PREC( mt ); }
    /**
     * Predefined mode. The default is 0 for @c DOUBLE
     */
    inline mode_t const DEFAULT(){ return 0; }
  }
}

#endif
