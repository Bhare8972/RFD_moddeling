/*
 * $Id: fft_complex.hpp 283 2012-08-26 15:36:43Z jdl3 $
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

#ifndef CCGSL_FFT_COMPLEX_HPP
#define CCGSL_FFT_COMPLEX_HPP

#include <gsl/gsl_fft_complex.h>
#include "complex.hpp"
#include "vector_complex.hpp"
#include "fft.hpp"

namespace gsl {
  namespace fft {
    /**
     * Complex fast Fourier transforms.
     */
    namespace complex {
      /**
       * Radix 2 transforms.
       */
      namespace radix2 {
	/**
	 * C++ version of gsl_fft_complex_radix2_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int forward( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_forward( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int backward( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_backward( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int inverse( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_inverse( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int transform( gsl::complex_packed_array data, size_t const stride, size_t const n,
			      fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data, stride, n, sign ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_forward( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_dif_forward( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_backward( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_dif_backward( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_inverse( gsl::complex_packed_array data, size_t const stride, size_t const n ){
	  return gsl_fft_complex_radix2_dif_inverse( data, stride, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param n The size of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int dif_transform( gsl::complex_packed_array data, size_t const stride, size_t const n,
				  fft::direction const sign ){
	  return gsl_fft_complex_radix2_dif_transform( data, stride, n, sign ); }

	// Version swithout stride argument
	/**
	 * C++ version of gsl_fft_complex_radix2_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int forward( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_forward( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int backward( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_backward( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int inverse( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_inverse( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int transform( gsl::complex_packed_array data, size_t const n,
			      fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data, 1, n, sign ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_forward( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_dif_forward( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_backward( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_dif_backward( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @return Error code on failure.
	 */
	inline int dif_inverse( gsl::complex_packed_array data, size_t const n ){
	  return gsl_fft_complex_radix2_dif_inverse( data, 1, n ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param n The size of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int dif_transform( gsl::complex_packed_array data, size_t const n,
				  fft::direction const sign ){
	  return gsl_fft_complex_radix2_dif_transform( data, 1, n, sign ); }
	
	/**
	 * C++ version of gsl_fft_complex_radix2_forward().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int forward( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_forward( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_backward().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int backward( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_backward( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_inverse().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int inverse( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_inverse( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int transform( DATA& data, size_t const stride, fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data.data(), stride,
						   data.size() / (2 * stride), sign ); }
	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int transform( DATA& data, fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data.data(), 1, data.size() / 2, sign ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_forward().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int dif_forward( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_dif_forward( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_backward().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int dif_backward( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_dif_backward( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_inverse().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int dif_inverse( DATA& data, size_t const stride = 1 ){
	  return gsl_fft_complex_radix2_dif_inverse( data.data(), stride, data.size() / (2 * stride) ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_transform().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int dif_transform( DATA& data, size_t const stride, fft::direction const sign ){
	  return gsl_fft_complex_radix2_dif_transform( data.data(), stride,
						       data.size() / (2 * stride), sign ); }
	/**
	 * C++ version of gsl_fft_complex_radix2_dif_transform().
	 * Works with std::array<double>, std::vector<double> and gsl::vector.
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	template<typename DATA>
	inline int dif_transform( DATA& data, fft::direction const sign ){
	  return gsl_fft_complex_radix2_dif_transform( data.data(), 1, data.size() / 2, sign ); }
	/**
	 * C++ version of gsl_fft_complex_radix2_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<>
	inline int forward( gsl::vector_complex& data, size_t const stride ){
	  return gsl_fft_complex_radix2_forward( data.data(), stride, data.size() / stride ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<>
	inline int backward( gsl::vector_complex& data, size_t const stride ){
	  return gsl_fft_complex_radix2_backward( data.data(), stride, data.size() / stride ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @return Error code on failure.
	 */
	template<>
	inline int inverse( gsl::vector_complex& data, size_t const stride ){
	  return gsl_fft_complex_radix2_inverse( data.data(), stride, data.size() / stride ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param stride The stride of the array.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	template<>
	inline int transform( gsl::vector_complex& data, size_t const stride, fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data.data(), stride, data.size() / stride, sign ); }

	// stride-free versions

	/**
	 * C++ version of gsl_fft_complex_radix2_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int forward( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_forward( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int backward( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_backward( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int inverse( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_inverse( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int transform( gsl::vector_complex& data, fft::direction const sign ){
	  return gsl_fft_complex_radix2_transform( data.data(), 1,
						   data.size(), sign ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_forward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int dif_forward( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_dif_forward( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_backward().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int dif_backward( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_dif_backward( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_inverse().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @return Error code on failure.
	 */
	inline int dif_inverse( gsl::vector_complex& data ){
	  return gsl_fft_complex_radix2_dif_inverse( data.data(), 1, data.size() ); }

	/**
	 * C++ version of gsl_fft_complex_radix2_dif_transform().
	 * @param data An array of complex values packed as an array of double length of doubles.
	 * @param sign fft::forward or fft::backward.
	 * @return Error code on failure.
	 */
	inline int dif_transform( gsl::vector_complex& data, fft::direction const sign ){
	  return gsl_fft_complex_radix2_dif_transform( data.data(), 1,
						       data.size(), sign ); }
      }

      /**
       * C++ version of gsl_fft_complex_wavetable functions.
       */
      class wavetable {
      public:
	/**
	 * The default constructor is only really useful for assigning to.
	 */
	wavetable(){
	  ccgsl_pointer = 0;
	  count = 0; // ititially nullptr will do
	}
	// Refines random access container
	// Refines assignable
	/**
	 * The default constructor creates a new workspace of size n.
	 * @param n The size of the workspace.
	 */
	explicit wavetable( size_t const n ){
	  ccgsl_pointer = gsl_fft_complex_wavetable_alloc( n );
	  // just plausibly we could allocate wavetable but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_fft_complex_wavetable_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_fft_complex_wavetable.
	 * This is not usually a good idea. In this case
	 * you should not use gsl_fft_complex_wavetable_free() to deallocate the memory.
	 * @param v The wavetable
	 */
	explicit wavetable( gsl_fft_complex_wavetable* v ){
	  ccgsl_pointer = v;
	  // just plausibly we could fail to allocate count: no further action needed.
	  count = new size_t;
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	// copy constructor
	/**
	 * The copy constructor. This creates a new reference to the workspace.
	 * @param v The wavetable to copy.
	 */
	wavetable( wavetable const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	  count = v.count; if( count != 0 ) ++*count; }
	// assignment operator
	/**
	 * The assignment operator. This copies elementwise.
	 * @param v The wavetable to copy
	 */
	wavetable& operator=( wavetable const& v ){
	  // first, possibly delete anything pointed to by this
	  if( count == 0 or --*count == 0 ){
	    if( ccgsl_pointer != 0 ) gsl_fft_complex_wavetable_free( ccgsl_pointer );
	    delete count;
	  } // Then copy
	  ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
	}
	// clone()
	/**
	 * The clone function. Use this if you want a copy of the wavetable that does
	 * not share the underlying data.
	 * @return a new copy of this.
	 */
	wavetable clone() const throw( std::bad_alloc ){
	  wavetable copy( ccgsl_pointer->n );
	  // Now copy
	  if( gsl_fft_complex_memcpy( copy.ccgsl_pointer, ccgsl_pointer ) != GSL_SUCCESS )
	    throw( std::bad_alloc() );
	  // return new object
	  return copy;
	}

	// destructor
	/**
	 * The destructor only deletes the pointers if count reaches zero.
	 */
	~wavetable(){
	  if( count == 0 or --*count == 0 ){
	    // could have allocated null pointer
	    if( ccgsl_pointer != 0 ) gsl_fft_complex_wavetable_free( ccgsl_pointer );
	    delete count;
	  }
	}
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The wavetable to move.
	 */
	wavetable( wavetable&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The wavetable to move.
	 * @return A reference to this.
	 */
	wavetable& operator=( wavetable&& v ){
	  wavetable( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
	// Refines equality comparable
	// == operator
	/**
	 * Two wavetable are identically equal if their elements are identical.
	 * @param v The wavetable to be compared with @c this
	 * @return @c true or @c false according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator==( wavetable const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
	// != operator
	/**
	 * Two wavetable are different equal if their elements are not identical.
	 * @param v The wavetable to be compared with @c this
	 * @return @c false or @c true according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator!=( wavetable const& v ) const { return not operator==( v ); }
	// Refines forward container
	// Refines less than comparable
	// operator<
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a wavetable is nonnegative.
	 * @param v The wavetable to be compared with @c this
	 * @return @c false or @c true according as @c this is less than @c v
	 * lexicographically
	 */
	bool operator<( wavetable const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
	// operator>
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a wavetable is nonnegative.
	 * @param v The wavetable to be compared with @c this
	 * @return @c false or @c true according as @c this is greater than @c v
	 * lexicographically
	 */
	bool operator>( wavetable const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
	// operator<=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a wavetable is nonnegative.
	 * @param v The wavetable to be compared with @c this
	 * @return @c false or @c true according as @c this is less than
	 * or equal to @c v lexicographically
	 */
	bool operator<=( wavetable const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
	// operator>=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a wavetable is nonnegative.
	 * @param v The wavetable to be compared with @c this
	 * @return @c false or @c true according as @c this is no 
	 * less than @c v lexicographically
	 */
	bool operator>=( wavetable const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
	/**
	 * Find if the wavetable is empty.
	 * @return @c true if has size zero; otherwise @c false
	 */
	bool empty() const { return ccgsl_pointer == 0; }
	// swap() --- should work even if sizes don't match
	/**
	 * Swap two wavetable. This works even if the wavetable have different sizes
	 * because it swaps pointers.
	 * @param v The wavetable to swap with @c this.
	 */
	void swap( wavetable& v ){
	  std::swap( ccgsl_pointer, v.ccgsl_pointer );
	  std::swap( count, v.count );
	}
      private:
	/**
	 * The shared pointer
	 */
	gsl_fft_complex_wavetable* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_fft_complex_wavetable.
	 * @return the gsl_fft_complex_wavetable
	 */
	gsl_fft_complex_wavetable* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_fft_complex_wavetable.
	 * @return @c true or @c falses according as 
	 * this is the only workspace object sharing the gsl_fft_complex_wavetable
	 */
	bool unique() const { return count != 0 and *count == 1; }
	/**
	 * Find how many workspace objects share this pointer.
	 * @return the number of workspace objects that share this pointer
	 */
	size_t use_count() const { return count == 0 ? 0 : *count; }
	/**
	 * Allow conversion to bool.
	 * @return @c true or @c false according as this contains a pointer
	 * to a gsl_fft_complex_wavetable
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
      };

      /**
       * Workspace for complex fast fourier transforms.
       */
      class workspace {
      public:
	/**
	 * The default constructor is only really useful for assigning to.
	 */
	workspace(){
	  ccgsl_pointer = 0;
	  count = 0; // ititially nullptr will do
	}
	// Refines random access container
	// Refines assignable
	/**
	 * The default constructor creates a new workspace of size n.
	 * @param n The size of the workspace.
	 */
	explicit workspace( size_t const n ){
	  ccgsl_pointer = gsl_fft_complex_workspace_alloc( n );
	  // just plausibly we could allocate workspace but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_fft_complex_workspace_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_fft_complex_workspace.
	 * This is not usually a good idea. In this case
	 * you should not use gsl_fft_complex_workspace_free() to deallocate the memory.
	 * @param v The workspace
	 */
	explicit workspace( gsl_fft_complex_workspace* v ){
	  ccgsl_pointer = v;
	  // just plausibly we could fail to allocate count: no further action needed.
	  count = new size_t;
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	// copy constructor
	/**
	 * The copy constructor. This creates a new reference to the workspace.
	 * @param v The workspace to copy.
	 */
	workspace( workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	  count = v.count; if( count != 0 ) ++*count; }
	// assignment operator
	/**
	 * The assignment operator. This copies elementwise.
	 * @param v The workspace to copy
	 */
	workspace& operator=( workspace const& v ){
	  // first, possibly delete anything pointed to by this
	  if( count == 0 or --*count == 0 ){
	    if( ccgsl_pointer != 0 ) gsl_fft_complex_workspace_free( ccgsl_pointer );
	    delete count;
	  } // Then copy
	  ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
	}
	// destructor
	/**
	 * The destructor only deletes the pointers if count reaches zero.
	 */
	~workspace(){
	  if( count == 0 or --*count == 0 ){
	    // could have allocated null pointer
	    if( ccgsl_pointer != 0 ) gsl_fft_complex_workspace_free( ccgsl_pointer );
	    delete count;
	  }
	}
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The workspace to move.
	 */
	workspace( workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The workspace to move.
	 * @return A reference to this.
	 */
	workspace& operator=( workspace&& v ){
	  workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
	// Refines equality comparable
	// == operator
	/**
	 * Two workspace are identically equal if their elements are identical.
	 * @param v The workspace to be compared with @c this
	 * @return @c true or @c false according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator==( workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
	// != operator
	/**
	 * Two workspace are different equal if their elements are not identical.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator!=( workspace const& v ) const { return not operator==( v ); }
	// Refines forward container
	// Refines less than comparable
	// operator<
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a workspace is nonnegative.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this is less than @c v
	 * lexicographically
	 */
	bool operator<( workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
	// operator>
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a workspace is nonnegative.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this is greater than @c v
	 * lexicographically
	 */
	bool operator>( workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
	// operator<=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a workspace is nonnegative.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this is less than
	 * or equal to @c v lexicographically
	 */
	bool operator<=( workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
	// operator>=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for eaxmple, for checking, that a workspace is nonnegative.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this is no 
	 * less than @c v lexicographically
	 */
	bool operator>=( workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
	/**
	 * Find if the workspace is empty.
	 * @return @c true if has size zero; otherwise @c false
	 */
	bool empty() const { return ccgsl_pointer == 0; }
	// swap() --- should work even if sizes don't match
	/**
	 * Swap two workspace. This works even if the workspace have different sizes
	 * because it swaps pointers.
	 * @param v The workspace to swap with @c this.
	 */
	void swap( workspace& v ){
	  std::swap( ccgsl_pointer, v.ccgsl_pointer );
	  std::swap( count, v.count );
	}
      private:
	/**
	 * The shared pointer
	 */
	gsl_fft_complex_workspace* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_fft_complex_workspace.
	 * @return the gsl_fft_complex_workspace
	 */
	gsl_fft_complex_workspace* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_fft_complex_workspace.
	 * @return @c true or @c falses according as 
	 * this is the only workspace object sharing the gsl_fft_complex_workspace
	 */
	bool unique() const { return count != 0 and *count == 1; }
	/**
	 * Find how many workspace objects share this pointer.
	 * @return the number of workspace objects that share this pointer
	 */
	size_t use_count() const { return count == 0 ? 0 : *count; }
	/**
	 * Allow conversion to bool.
	 * @return @c true or @c false according as this contains a pointer
	 * to a gsl_fft_complex_workspace
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
      };

      /**
       * C++ version of gsl_fft_complex_forward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int forward( gsl::complex_packed_array data, size_t const stride, size_t const n,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data, stride, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int backward( gsl::complex_packed_array data, size_t const stride, size_t const n,
			   wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data, stride, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int inverse( gsl::complex_packed_array data, size_t const stride, size_t const n,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data, stride, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      inline int transform( gsl::complex_packed_array data, size_t const stride, size_t const n,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data, stride, n, wavetable.get(), work.get(), sign ); }

      // stride-free versions
      
      /**
       * C++ version of gsl_fft_complex_forward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int forward( gsl::complex_packed_array data, size_t const n,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data, 1, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int backward( gsl::complex_packed_array data, size_t const n,
			   wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data, 1, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      inline int inverse( gsl::complex_packed_array data, size_t const n,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data, 1, n, wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param n The size of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      inline int transform( gsl::complex_packed_array data, size_t const n,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data, 1, n, wavetable.get(), work.get(), sign ); }

      // template versions
      
      /**
       * C++ version of gsl_fft_complex_forward().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int forward( DATA& data, size_t const stride, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data.data(), stride, data.size() / (2 * stride),
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int backward(DATA& data, size_t const stride, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data.data(), stride, data.size() / (2 * stride),
					 wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int inverse( DATA& data, size_t const stride, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data.data(), stride, data.size() / (2 * stride),
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int transform( DATA& data, size_t const stride,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data.data(), stride, data.size() / (2 * stride),
					  wavetable.get(), work.get(), sign ); }
      /**
       * C++ version of gsl_fft_complex_forward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int forward( gsl::vector_complex& data, size_t const stride,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data.data(), stride, data.size() / stride,
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int backward( gsl::vector_complex& data, size_t const stride,
			   wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data.data(), stride, data.size() / stride,
					 wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int inverse( gsl::vector_complex& data, size_t const stride,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data.data(), stride, data.size() / stride,
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param stride The stride of the array.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      template<>
      inline int transform( gsl::vector_complex& data, size_t const stride,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data.data(), stride, data.size() / stride,
					  wavetable.get(), work.get(), sign ); }

      // stride-free versions

      /**
       * C++ version of gsl_fft_complex_forward().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int forward( DATA& data, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data.data(), 1, data.size() / 2,
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int backward(DATA& data, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data.data(), 1, data.size() / 2,
					 wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int inverse( DATA& data, wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data.data(), 1, data.size() / 2,
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * Works with std::array<double>, std::vector<double> and gsl::vector.
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      template<typename DATA>
      inline int transform( DATA& data,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data.data(), 1, data.size() / 2,
					  wavetable.get(), work.get(), sign ); }
      /**
       * C++ version of gsl_fft_complex_forward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int forward( gsl::vector_complex& data,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_forward( data.data(), 1, data.size(),
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_backward().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int backward( gsl::vector_complex& data,
			   wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_backward( data.data(), 1, data.size(),
					 wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_inverse().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @return Error code on failure.
       */
      template<>
      inline int inverse( gsl::vector_complex& data,
			  wavetable const& wavetable, workspace& work ){
	return gsl_fft_complex_inverse( data.data(), 1, data.size(),
					wavetable.get(), work.get() ); }

      /**
       * C++ version of gsl_fft_complex_transform().
       * @param data An array of complex values packed as an array of double length of doubles.
       * @param wavetable A wavetable of size n.
       * @param work A workspace of size n.
       * @param sign fft::forward or fft::backward.
       * @return Error code on failure.
       */
      template<>
      inline int transform( gsl::vector_complex& data,
			    wavetable const& wavetable, workspace& work, fft::direction const sign ){
	return gsl_fft_complex_transform( data.data(), 1, data.size(),
					  wavetable.get(), work.get(), sign ); }
    }
  }
}

#endif
