/*
 * $Id: statistics.hpp 137 2012-03-29 15:24:35Z jdl3 $
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

#ifndef CCGSL_STATISTICS_HPP
#define CCGSL_STATISTICS_HPP

namespace gsl {
  /**
   * This namespace provides the gsl_stats functions. Besides simple wrappers to the
   * original functions, it provides template functions that can be called on any class
   * that implements the following two functions:
   * -# double* data(): returns an array of elements of type double,
   * -# size_t size(): returns size of array.
   * The template versions use either default arguments or function overloading to ensure
   * that any statistics function may be called without stride arguments (i.e. with default
   * stride = 1).
   */
  namespace stats {
  }
}

#include"statistics_long_double.hpp"
#include"statistics_double.hpp"
#include"statistics_float.hpp"
#include"statistics_ulong.hpp"
#include"statistics_long.hpp"
#include"statistics_uint.hpp"
#include"statistics_int.hpp"
#include"statistics_ushort.hpp"
#include"statistics_short.hpp"
#include"statistics_uchar.hpp"
#include"statistics_char.hpp"
#endif
