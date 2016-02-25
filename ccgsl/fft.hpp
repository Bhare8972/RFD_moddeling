/*
 * $Id: fft.hpp 90 2012-01-21 14:49:39Z jdl3 $
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

#ifndef CCGSL_FFT_HPP
#define CCGSL_FFT_HPP

#include <gsl/gsl_fft.h>

namespace gsl {
  /**
   * C++ implementation of the GNU Scientific Library fast Fourier transform functions.
   */
  namespace fft {
    /**
     * Typedef for fft::direction.
     */
    typedef gsl_fft_direction direction;
    /**
     * Forward.
     */
    direction const forward = gsl_fft_forward;
    /**
     * Backward.
     */
    direction const backward = gsl_fft_forward;
  }
}

#endif
