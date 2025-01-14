/*
 * $Id: permute_vector.hpp 274 2012-08-26 11:33:44Z jdl3 $
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

#ifndef CCGSL_PERMUTE_VECTOR_HPP
#define CCGSL_PERMUTE_VECTOR_HPP

#include<cmath>
#include<gsl/gsl_permute_vector.h>
#include"permute.hpp"
#include"vector.hpp"
#include"vector_char.hpp"
#include"vector_uchar.hpp"
#include"vector_short.hpp"
#include"vector_ushort.hpp"
#include"vector_int.hpp"
#include"vector_uint.hpp"
#include"vector_long.hpp"
#include"vector_ulong.hpp"
#include"vector_float.hpp"
#include"vector_long_double.hpp"
#include"vector_complex.hpp"
#include"vector_complex_float.hpp"
#include"vector_complex_long_double.hpp"

namespace gsl {
  namespace permute {
    /**
     * C++ version of gsl_permute_vector_char().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_char_forward( permutation const& p, vector_char& v ){
      return gsl_permute_vector_char( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_forward( permutation const& p, vector_complex& v ){
      return gsl_permute_vector_complex( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex_float().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_float_forward( permutation const& p, vector_complex_float& v ){
      return gsl_permute_vector_complex_float( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex_long_double().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_long_double_forward( permutation const& p,
						   vector_complex_long_double& v ){
      return gsl_permute_vector_complex_long_double( p.get(), v.get() ); }
    /**
     * C++ version of gsl_permute_vector().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_forward( permutation const& p, gsl::vector& v ){
      return gsl_permute_vector( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_float().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_float_forward( permutation const& p, vector_float& v ){
      return gsl_permute_vector_float( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_int().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_int_forward( permutation const& p, vector_int& v ){
      return gsl_permute_vector_int( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_long_double().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_long_double_forward( permutation const& p, vector_long_double& v ){
      return gsl_permute_vector_long_double( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_long().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_long_forward( permutation const& p, vector_long& v ){
      return gsl_permute_vector_long( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_short().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_short_forward( permutation const& p, vector_short& v ){
      return gsl_permute_vector_short( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_uchar().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_uchar_forward( permutation const& p, vector_uchar& v ){
      return gsl_permute_vector_uchar( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_uint().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_uint_forward( permutation const& p, vector_uint& v ){
      return gsl_permute_vector_uint( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_ulong().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_ulong_forward( permutation const& p, vector_ulong& v ){
      return gsl_permute_vector_ulong( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_ushort().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_ushort_forward( permutation const& p, vector_ushort& v ){
      return gsl_permute_vector_ushort( p.get(), v.get() ); }

    // Inverse functions
    /**
     * C++ version of gsl_permute_vector_char_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_char_inverse( permutation const& p, vector_char& v ){
      return gsl_permute_vector_char_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_inverse( permutation const& p, vector_complex& v ){
      return gsl_permute_vector_complex_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex_float_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_float_inverse( permutation const& p, vector_complex_float& v ){
      return gsl_permute_vector_complex_float_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_complex_long_double_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_complex_long_double_inverse( permutation const& p,
						   vector_complex_long_double& v ){
      return gsl_permute_vector_complex_long_double_inverse( p.get(), v.get() ); }
    /**
     * C++ version of gsl_permute_vector_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_inverse( permutation const& p, gsl::vector& v ){
      return gsl_permute_vector_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_float_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_float_inverse( permutation const& p, vector_float& v ){
      return gsl_permute_vector_float_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_int_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_int_inverse( permutation const& p, vector_int& v ){
      return gsl_permute_vector_int_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_long_double_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_long_double_inverse( permutation const& p, vector_long_double& v ){
      return gsl_permute_vector_long_double_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_long_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_long_inverse( permutation const& p, vector_long& v ){
      return gsl_permute_vector_long_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_short_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_short_inverse( permutation const& p, vector_short& v ){
      return gsl_permute_vector_short_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_uchar_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_uchar_inverse( permutation const& p, vector_uchar& v ){
      return gsl_permute_vector_uchar_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_uint_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_uint_inverse( permutation const& p, vector_uint& v ){
      return gsl_permute_vector_uint_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_ulong_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_ulong_inverse( permutation const& p, vector_ulong& v ){
      return gsl_permute_vector_ulong_inverse( p.get(), v.get() ); } 
    /**
     * C++ version of gsl_permute_vector_ushort_inverse().
     * @param p A permutation
     * @param v A vector
     * @return Error code on failure
     */
    inline int vector_ushort_inverse( permutation const& p, vector_ushort& v ){
      return gsl_permute_vector_ushort_inverse( p.get(), v.get() ); }
  }
}

#endif
