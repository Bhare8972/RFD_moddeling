/*
 * $Id: spline.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_SPLINE_HPP
#define CCGSL_SPLINE_HPP

#include<new>
#include<gsl/gsl_spline.h>
#include"interp.hpp"

namespace gsl {
  /**
   * Higher level interface for interpolation.
   */
  class spline {
  public:
    /**
     * Convenience typedef
     */
    typedef gsl_interp_type type;
    /**
     * The default constructor is only really useful for assigning to.
     */
    spline(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new spline with n elements.
     * @param T The interpolation type: use static functions from interp to
     * choose a type
     * @param n The number of elements in the spline
     */
    explicit spline( type const* T, size_t const n ){
      ccgsl_pointer = gsl_spline_alloc( T, n );
      // just plausibly we could allocate spline but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_spline_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_spline.
     * This is not usually a good idea. In this case
     * we should not use gsl_spline_free() to deallocate the memory.
     * @param v The spline
     */
    explicit spline( gsl_spline* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The spline to copy.
     */
    spline( spline const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The spline to copy
     */
    spline& operator=( spline const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_spline_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~spline(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_spline_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The spline to move.
     */
    spline( spline&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The spline to move.
     * @return A reference to this.
     */
    spline& operator=( spline&& v ){
      spline( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two spline are identically equal if their elements are identical.
     * @param v The spline to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( spline const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two spline are different if their elements are not identical.
     * @param v The spline to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( spline const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a spline is nonnegative.
     * @param v The spline to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( spline const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a spline is nonnegative.
     * @param v The spline to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( spline const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a spline is nonnegative.
     * @param v The spline to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( spline const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a spline is nonnegative.
     * @param v The spline to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( spline const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the spline is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two spline objects. This works even if the spline have different sizes
     * because it swaps pointers.
     * @param v The spline to swap with @c this.
     */
    void swap( spline& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_spline* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_spline.
     * @return the gsl_spline
     */
    gsl_spline* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_spline.
     * @return @c true or @c falses according as 
     * this is the only spline object sharing the gsl_spline.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many spline objects share this pointer.
     * @return the number of spline objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_spline.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * C++ version of gsl_spline_init().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param size the size of xa and ya
     * @return Error code on failure
     */
    int init( double const xa[], double const ya[], size_t size ){
      return gsl_spline_init( get(), xa, ya, size ); }

    /**
     * C++ version of gsl_spline_init().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int init( XA const& xa, YA const& ya ){
      return gsl_spline_init( get(), xa.data(), ya.data(), xa.size() ); }

    /**
     * C++ version of gsl_spline_name().
     * @return the name of the interpolation method
     */
    char const* name() const { return gsl_spline_name( get() ); }

    /**
     * C++ version of gsl_spline_min_size().
     * @return the minimum number of points needed for interpolation
     */
    unsigned int min_size() const { return gsl_spline_min_size( get() ); }

    /**
     * C++ version of gsl_spline_eval_e().
     * @param x a given value of x
     * @param a an accelerator
     * @param y the interpolated value of y at x (return value)
     * @return Error code on failure
     */
    int eval_e( double x, interp::accel& a, double* y ) const {
      return gsl_spline_eval_e( get(), x, a.get(), y ); }

    /**
     * C++ version of gsl_spline_eval().
     * @param x a given value of x
     * @param a an accelerator
     * @return the interpolated value of y at x
     */
    double eval( double x, interp::accel& a ) const {
      return gsl_spline_eval( get(), x, a.get() ); }

    /**
     * C++ version of gsl_spline_eval_deriv_e().
     * @param x a given value of x
     * @param a an accelerator
     * @param y the derivative of the interpolated value at x (return value)
     * @return Error code on failure
     */
    int eval_deriv_e( double x, interp::accel& a, double* y ) const {
      return gsl_spline_eval_deriv_e( get(), x, a.get(), y ); }

    /**
     * C++ version of gsl_spline_eval_deriv().
     * @param x a given value of x
     * @param a an accelerator
     * @return the derivative of the interpolated value at x
     */
    double eval_deriv( double x, interp::accel& a ) const {
      return gsl_spline_eval_deriv( get(), x, a.get() ); }

    /**
     * C++ version of gsl_spline_eval_deriv2_e().
     * @param x a given value of x
     * @param a an accelerator
     * @param y the second derivative of the interpolated value at x (return value)
     * @return Error code on failure
     */
    int eval_deriv2_e( double x, interp::accel& a, double* y ) const {
      return gsl_spline_eval_deriv2_e( get(), x, a.get(), y ); }

    /**
     * C++ version of gsl_spline_eval_deriv2().
     * @param x a given value of x
     * @param a an accelerator
     * @return the second derivative of the interpolated value at x
     */
    double eval_deriv2( double x, interp::accel& a ) const {
      return gsl_spline_eval_deriv2( get(), x, a.get() ); }

    /**
     * C++ version of gsl_spline_eval_integ_e().
     * @param a lower limit of integration
     * @param b upper limit of integration
     * @param acc an accelerator
     * @param y interpolated integral between a and b (return value)
     * @return Error code on failure
     */
    int eval_integ_e( double a, double b, interp::accel& acc, double* y ) const {
      return gsl_spline_eval_integ_e( get(), a, b, acc.get(), y ); }

    /**
     * C++ version of gsl_spline_eval_integ().
     * @param a lower limit of integration
     * @param b upper limit of integration
     * @param acc an accelerator
     * @return interpolated integral between a and b
     */
    double eval_integ( double a, double b, interp::accel& acc ) const {
      return gsl_spline_eval_integ( get(), a, b, acc.get() ); }

  };
}
#endif
