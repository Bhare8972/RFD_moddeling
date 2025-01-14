/*
 * $Id: sf.hpp 9 2010-06-13 14:02:43Z jdl3 $
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

#ifndef CCGSL_SF_HPP
#define CCGSL_SF_HPP

#include"sf_result.hpp"

#include"sf_airy.hpp"
#include"sf_bessel.hpp"
#include"sf_clausen.hpp"
#include"sf_coulomb.hpp"
#include"sf_coupling.hpp"
#include"sf_dawson.hpp"
#include"sf_debye.hpp"
#include"sf_dilog.hpp"
#include"sf_elementary.hpp"
#include"sf_ellint.hpp"
#include"sf_elljac.hpp"
#include"sf_erf.hpp"
#include"sf_exp.hpp"
#include"sf_expint.hpp"
#include"sf_fermi_dirac.hpp"
#include"sf_gamma.hpp"
#include"sf_gegenbauer.hpp"
#include"sf_hyperg.hpp"
#include"sf_laguerre.hpp"
#include"sf_lambert.hpp"
#include"sf_legendre.hpp"
#include"sf_log.hpp"
#include"sf_mathieu.hpp"
#include"sf_pow_int.hpp"
#include"sf_psi.hpp"
#include"sf_synchrotron.hpp"
#include"sf_transport.hpp"
#include"sf_trig.hpp"
#include"sf_zeta.hpp"

namespace gsl {
  /**
   * This namespace is used for special functions that in GSL are prefixed @c gsl_sf. Note that
   * many of these functions are placed in namespaces within @c sf. So, for example,
   * @c gsl_sf_airy_Bi_deriv_e() becomes @c gsl::sf::airy::deriv_e(). However, other functions such
   * as the trigonometric functions are contained directly in the @c sf namespace.
   *
   * Many of the special functions are available in two forms. The first returns an integer indicating
   * any error that occurred and returns the result as a @c gsl::sf::struct object. The second returns 
   * a double. In C++ the second may be preferred because we can use gsl::exception to cause these
   * functions to throw an exception if any problem occurs.
   */
  namespace sf {}
}

#endif
