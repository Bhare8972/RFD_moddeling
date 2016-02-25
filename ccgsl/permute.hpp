/*
 * $Id: permute.hpp 296 2013-01-02 12:17:04Z jdl3 $
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

#ifndef CCGSL_PERMUTE_HPP
#define CCGSL_PERMUTE_HPP

#include<cmath>
#include<gsl/gsl_permute.h>
#include"exception.hpp"

namespace gsl {
  /**
   * This namespace handles the @c gsl_permute functions.
   */
  namespace permute {
    /**
     * C++ version of gsl_permute_complex().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_forward( permutation const& p, double* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_complex_float().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_float_forward( permutation const& p, float* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_complex_long_double().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_long_double_forward( permutation const& p, long double* data, size_t const stride,
					    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int forward( permutation const& p, double* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_float().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int float_forward( permutation const& p, float* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_int().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int int_forward( permutation const& p, int* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int( p.get()->data, data, stride, n ); }

    /**
     * C++ version of gsl_permute_long_double().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_double_forward( permutation const& p, long double* data, size_t const stride,
				    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_long().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_forward( permutation const& p, long* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_short().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int short_forward( permutation const& p, short* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_uchar().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uchar_forward( permutation const& p, unsigned char* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_uint().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uint_forward( permutation const& p, unsigned int* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_ulong().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ulong_forward( permutation const& p, unsigned long* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_ushort().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ushort_forward( permutation const& p, unsigned short* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort( p.get()->data, data, stride, n ); }

    // versions without strides
    
    /**
     * C++ version of gsl_permute_complex().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_forward( permutation const& p, double* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_complex_float().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_float_forward( permutation const& p, float* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_complex_long_double().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_long_double_forward( permutation const& p, long double* data,
					    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int forward( permutation const& p, double* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_float().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int float_forward( permutation const& p, float* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_int().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int int_forward( permutation const& p, int* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int( p.get()->data, data, 1, n ); }

    /**
     * C++ version of gsl_permute_long_double().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_double_forward( permutation const& p, long double* data,
				    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_long().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_forward( permutation const& p, long* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_short().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int short_forward( permutation const& p, short* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_uchar().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uchar_forward( permutation const& p, unsigned char* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_uint().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uint_forward( permutation const& p, unsigned int* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_ulong().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ulong_forward( permutation const& p, unsigned long* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_ushort().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ushort_forward( permutation const& p, unsigned short* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort( p.get()->data, data, 1, n ); }
  
    // Template versions
    /**
     * C++ version of gsl_permute_complex().
     * This version works with gsl::vector_complex arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_complex_float().
     * This version works with gsl::vector_complex_float arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_float_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_complex_long_double().
     * This version works with gsl::vector_complex_long_double arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_long_double_forward( permutation const& p, DATA& data, size_t const stride = 1){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute().
     * This version works with gsl::vector and std::vector<double> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_float().
     * This version works with gsl::vector and std::vector<float> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int float_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_int().
     * This version works with gsl::vector and std::vector<float> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int int_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int( p.get()->data, data.data(), stride, data.size() / stride ); }

    /**
     * C++ version of gsl_permute_long_double().
     * This version works with gsl::vector and std::vector<long double> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int long_double_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_long().
     * This version works with gsl::vector and std::vector<long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int long_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_short().
     * This version works with gsl::vector_long and std::vector<long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int short_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_uchar().
     * This version works with gsl::vector_short and std::vector<short> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int uchar_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_uint().
     * This version works with gsl::vector_uint and std::vector<unsigned int> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int uint_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_ulong().
     * This version works with gsl::vector_ulong and std::vector<unsigned long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int ulong_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_ushort().
     * This version works with gsl::vector_ushort and std::vector<unsigned short> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int ushort_forward( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort( p.get()->data, data.data(), stride, data.size() / stride ); }

    // Inverse functions
    /**
     * C++ version of gsl_permute_complex_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_inverse( permutation const& p, double* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_inverse( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_complex_float_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_float_inverse( permutation const& p, float* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float_inverse( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_complex_long_double_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_long_double_inverse( permutation const& p, long double* data, size_t const stride,
					    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double_inverse( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int inverse( permutation const& p, double* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_float_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int float_inverse( permutation const& p, float* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float_inverse( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_int_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int int_inverse( permutation const& p, int* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int_inverse( p.get()->data, data, stride, n ); }

    /**
     * C++ version of gsl_permute_long_double_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_double_inverse( permutation const& p, long double* data, size_t const stride,
				    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double_inverse( p.get()->data, data, stride, n ); }
    /**
     * C++ version of gsl_permute_long_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_inverse( permutation const& p, long* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_short_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int short_inverse( permutation const& p, short* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_uchar_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uchar_inverse( permutation const& p, unsigned char* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_uint_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uint_inverse( permutation const& p, unsigned int* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_ulong_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ulong_inverse( permutation const& p, unsigned long* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong_inverse( p.get()->data, data, stride, n ); } 
    /**
     * C++ version of gsl_permute_ushort_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ushort_inverse( permutation const& p, unsigned short* data, size_t const stride, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort_inverse( p.get()->data, data, stride, n ); }

    // versions without strides
    
    /**
     * C++ version of gsl_permute_complex_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_inverse( permutation const& p, double* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_inverse( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_complex_float_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_float_inverse( permutation const& p, float* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float_inverse( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_complex_long_double_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int complex_long_double_inverse( permutation const& p, long double* data,
					    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double_inverse( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int inverse( permutation const& p, double* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_float_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int float_inverse( permutation const& p, float* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float_inverse( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_int_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int int_inverse( permutation const& p, int* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int_inverse( p.get()->data, data, 1, n ); }

    /**
     * C++ version of gsl_permute_long_double_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_double_inverse( permutation const& p, long double* data,
				    size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double_inverse( p.get()->data, data, 1, n ); }
    /**
     * C++ version of gsl_permute_long_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int long_inverse( permutation const& p, long* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_short_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int short_inverse( permutation const& p, short* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_uchar_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uchar_inverse( permutation const& p, unsigned char* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_uint_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int uint_inverse( permutation const& p, unsigned int* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_ulong_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ulong_inverse( permutation const& p, unsigned long* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong_inverse( p.get()->data, data, 1, n ); } 
    /**
     * C++ version of gsl_permute_ushort_inverse().
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param n The size of @c data
     * @return Error code on failure
     */
    inline int ushort_inverse( permutation const& p, unsigned short* data, size_t const n ){
      if( p.get()->size != n ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort_inverse( p.get()->data, data, 1, n ); }
  
    // Template versions
    /**
     * C++ version of gsl_permute_complex_inverse().
     * This version works with gsl::vector_complex arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_complex_float_inverse().
     * This version works with gsl::vector_complex_float arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_float_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_float_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_complex_long_double_inverse().
     * This version works with gsl::vector_complex_long_double arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int complex_long_double_inverse( permutation const& p, DATA& data, size_t const stride = 1){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_complex_long_double_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_inverse().
     * This version works with gsl::vector and std::vector<double> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_float_inverse().
     * This version works with gsl::vector and std::vector<float> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int float_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_float_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_int_inverse().
     * This version works with gsl::vector and std::vector<float> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int int_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_int_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }

    /**
     * C++ version of gsl_permute_long_double_inverse().
     * This version works with gsl::vector and std::vector<long double> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int long_double_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_double_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }
    /**
     * C++ version of gsl_permute_long_inverse().
     * This version works with gsl::vector and std::vector<long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int long_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_long_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_short_inverse().
     * This version works with gsl::vector_long and std::vector<long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int short_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_short_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_uchar_inverse().
     * This version works with gsl::vector_short and std::vector<short> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int uchar_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uchar_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_uint_inverse().
     * This version works with gsl::vector_uint and std::vector<unsigned int> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int uint_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_uint_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_ulong_inverse().
     * This version works with gsl::vector_ulong and std::vector<unsigned long> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int ulong_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ulong_inverse( p.get()->data, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_permute_ushort_inverse().
     * This version works with gsl::vector_ushort and std::vector<unsigned short> arrays.
     * @param p Integer representation of permutation
     * @param data An array to be permuted
     * @param stride The size of the stride to use within @c data
     * @return Error code on failure
     */
    template<typename DATA>
    inline int ushort_inverse( permutation const& p, DATA& data, size_t const stride = 1 ){
      if( p.get()->size != data.size() ){
	gsl_error( "array and permutation sizes do not match", __FILE__, __LINE__, exception::GSL_ESANITY );
	return exception::GSL_ESANITY;
      }
      return gsl_permute_ushort_inverse( p.get()->data, data.data(), stride, data.size() / stride ); }

  }
  
}

#endif
