/*
 * $Id: interp.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_INTERP_HPP
#define CCGSL_INTERP_HPP

#include<new>
#include<gsl/gsl_interp.h>

namespace gsl {
  /**
   * Interpolation functions and objects.
   */
  class interp {
  public:
    /**
     * Convenient typedef.
     */
    typedef gsl_interp_type type;
    /**
     * The default constructor is only really useful for assigning to.
     */
    interp(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new interp with n elements.
     * @param T The interpolation type
     * @param n The number of elements in the interp
     */
    explicit interp( type const* T, size_t const n ){
      ccgsl_pointer = gsl_interp_alloc( T, n );
      // just plausibly we could allocate interp but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_interp_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_interp.
     * This is not usually a good idea. In this case
     * we should not use gsl_interp_free() to deallocate the memory.
     * @param v The interp
     */
    explicit interp( gsl_interp* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The interp to copy.
     */
    interp( interp const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The interp to copy
     */
    interp& operator=( interp const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_interp_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~interp(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_interp_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The interp to move.
     */
    interp( interp&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The interp to move.
     * @return A reference to this.
     */
    interp& operator=( interp&& v ){
      interp( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two interp are identically equal if their elements are identical.
     * @param v The interp to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( interp const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two interp are different if their elements are not identical.
     * @param v The interp to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( interp const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a interp is nonnegative.
     * @param v The interp to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( interp const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a interp is nonnegative.
     * @param v The interp to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( interp const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a interp is nonnegative.
     * @param v The interp to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( interp const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a interp is nonnegative.
     * @param v The interp to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( interp const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the interp is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two interp objects. This works even if the interp have different sizes
     * because it swaps pointers.
     * @param v The interp to swap with @c this.
     */
    void swap( interp& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_interp* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_interp.
     * @return the gsl_interp
     */
    gsl_interp* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_interp.
     * @return @c true or @c falses according as 
     * this is the only interp object sharing the gsl_interp.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many interp objects share this pointer.
     * @return the number of interp objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_interp.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * Workspace for acceleration
     */
    class accel {
    public:
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new accel.
       */
      accel(){
	ccgsl_pointer = gsl_interp_accel_alloc();
	// just plausibly we could allocate accel but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_interp_accel_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_interp_accel.
       * This is not usually a good idea. In this case
       * we should not use gsl_interp_accel_free() to deallocate the memory.
       * @param v The accel
       */
      explicit accel( gsl_interp_accel* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The accel to copy.
       */
      accel( accel const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The accel to copy
       */
      accel& operator=( accel const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_interp_accel_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~accel(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_interp_accel_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The accel to move.
	 */
	accel( accel&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The accel to move.
	 * @return A reference to this.
	 */
	accel& operator=( accel&& v ){
	  accel( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two accel are identically equal if their elements are identical.
       * @param v The accel to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( accel const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two accel are different if their elements are not identical.
       * @param v The accel to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( accel const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a accel is nonnegative.
       * @param v The accel to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( accel const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a accel is nonnegative.
       * @param v The accel to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( accel const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a accel is nonnegative.
       * @param v The accel to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( accel const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a accel is nonnegative.
       * @param v The accel to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( accel const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the accel is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two accel objects. This works even if the accel have different sizes
       * because it swaps pointers.
       * @param v The accel to swap with @c this.
       */
      void swap( accel& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_interp_accel* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_interp_accel.
       * @return the gsl_interp_accel
       */
      gsl_interp_accel* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_interp_accel.
       * @return @c true or @c falses according as 
       * this is the only accel object sharing the gsl_interp_accel.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many accel objects share this pointer.
       * @return the number of accel objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_interp_accel.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    
      /**
       * C++ version of gsl_interp_accel_reset().
       * @return Error code on failure
       */
      int reset(){ return gsl_interp_accel_reset( get() ); }
      /**
       * C++ version of gsl_interp_accel_find().
       * @param xa[] an array
       * @param len the size of the array
       * @param x the element to find
       * @return the index i such that x_array[i] <= x < x_array[i+1]
       */
      size_t find( double const xa[], size_t len, double x ){
	return gsl_interp_accel_find( get(), xa, len, x ); }
      /**
       * C++ version of gsl_interp_accel_find().
       * This version handles std::vector and gsl::vector.
       * @param xa[] an array
       * @param x the element to find
       * @return the index i such that x_array[i] <= x < x_array[i+1]
       */
      template<typename XA>
      size_t find( XA const& xa, double x ){
	return gsl_interp_accel_find( get(), xa.data(), xa.size(), x ); }
    };

    /**
     * C++ version of gsl_interp_init().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param size the size of xa and ya
     * @return Error code on failure
     */
    int init( double const xa[], double const ya[], size_t const size ){
      return gsl_interp_init( get(), xa, ya, size ); }

    /**
     * C++ version of gsl_interp_init().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int init( XA const& xa, YA const& ya ){
      return gsl_interp_init( get(), xa.data(), ya.data(), xa.size() ); }

    /**
     * C++ version of gsl_interp_name().
     * @return The name of the interpolation method
     */
    char const* name() const { return gsl_interp_name( get() ); }

    /**
     * C++ version of gsl_interp_min_size().
     * @return the minimum number of points for the interpolation
     */
    unsigned int min_size() const { return gsl_interp_min_size( get() ); }

    /**
     * C++ version of gsl_interp_type_min_size().
     * @param T The type
     * @return The minimum number of points required for the interpolation
     */
    unsigned int type_min_size( gsl_interp_type const* T ){ return gsl_interp_type_min_size( T ); }

    /**
     * C++ version of gsl_interp_eval_e().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param y the interpolated value of y at x (return value)
     * @return Error code on failure
     */
    int eval_e( double const xa[], double const ya[], double x, accel& a, double* y ) const {
      return gsl_interp_eval_e( get(), xa, ya, x, a.get(), y ); }

    /**
     * C++ version of gsl_interp_eval().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of y at x
     */
    double eval( double const xa[], double const ya[], double x, accel& a ) const {
      return gsl_interp_eval( get(), xa, ya, x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_deriv_e().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param d the interpolated value of the derivative of y at x (return value)
     * @return Error code on failure
     */
    int eval_deriv_e( double const xa[], double const ya[], double x, accel& a, double* d ) const {
      return gsl_interp_eval_deriv_e( get(), xa, ya, x, a.get(), d ); }

    /**
     * C++ version of gsl_interp_eval_deriv().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of the derivative of y at x
     */
    double eval_deriv( double const xa[], double const ya[], double x, accel& a ) const {
      return gsl_interp_eval_deriv( get(), xa, ya, x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_deriv2_e().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param d2 the interpolated value of the second derivative of y at x (return value)
     * @return Error code on failure
     */
    int eval_deriv2_e( double const xa[], double const ya[], double x, accel& a, double* d2 ) const {
      return gsl_interp_eval_deriv2_e( get(), xa, ya, x, a.get(), d2 ); }

    /**
     * C++ version of gsl_interp_eval_deriv2().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of the second derivative of y at x
     */
    double eval_deriv2( double const xa[], double const ya[], double x, accel& a ) const {
      return gsl_interp_eval_deriv2( get(), xa, ya, x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_integ_e().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param a the lower integral limit
     * @param b the upper integral limit
     * @param acc an accelerator
     * @param result the interpolated integral between a and b (return value)
     * @return Error code on failure
     */
    int eval_integ_e( double const xa[], double const ya[], double a, double b,
		      accel& acc, double* result ) const {
      return gsl_interp_eval_integ_e( get(), xa, ya, a, b, acc.get(), result ); }

    /**
     * C++ version of gsl_interp_eval_integ().
     * @param xa[] the array of x values
     * @param ya[] the array of y values
     * @param a the lower integral limit
     * @param b the upper integral limit
     * @param acc an accelerator
     * @return the interpolated integral between a and b 
     */
    double eval_integ( double const xa[], double const ya[], double a, double b, accel& acc ) const {
      return gsl_interp_eval_integ( get(), xa, ya, a, b, acc.get() ); }

    /**
     * C++ version of gsl_interp_bsearch().
     * @param x_array[] the array of x values
     * @param x a given value x
     * @param index_lo lower bound for result
     * @param index_hi upper bound for result
     * @return the index
     */
    size_t bsearch( double const x_array[], double x, size_t index_lo, size_t index_hi ){
      return gsl_interp_bsearch( x_array, x, index_lo, index_hi ); }

    // Template versions
    /**
     * C++ version of gsl_interp_eval_e().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param y the interpolated value of y at x (return value)
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int eval_e( XA const& xa, YA const& ya, double x, accel& a, double* y ) const {
      return gsl_interp_eval_e( get(), xa.data(), ya.data(), x, a.get(), y ); }

    /**
     * C++ version of gsl_interp_eval().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of y at x
     */
    template<typename XA,typename YA>
    double eval( XA const& xa, YA const& ya, double x, accel& a ) const {
      return gsl_interp_eval( get(), xa.data(), ya.data(), x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_deriv_e().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param d the interpolated value of the derivative of y at x (return value)
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int eval_deriv_e( XA const& xa, YA const& ya, double x, accel& a, double* d ) const {
      return gsl_interp_eval_deriv_e( get(), xa.data(), ya.data(), x, a.get(), d ); }

    /**
     * C++ version of gsl_interp_eval_deriv().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of the derivative of y at x
     */
    template<typename XA,typename YA>
    double eval_deriv( XA const& xa, YA const& ya, double x, accel& a ) const {
      return gsl_interp_eval_deriv( get(), xa.data(), ya.data(), x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_deriv2_e().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @param d2 the interpolated value of the second derivative of y at x (return value)
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int eval_deriv2_e( XA const& xa, YA const& ya, double x, accel& a, double* d2 ) const {
      return gsl_interp_eval_deriv2_e( get(), xa.data(), ya.data(), x, a.get(), d2 ); }

    /**
     * C++ version of gsl_interp_eval_deriv2().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param x a given value x
     * @param a an accelerator
     * @return the interpolated value of the second derivative of y at x
     */
    template<typename XA,typename YA>
    double eval_deriv2( XA const& xa, YA const& ya, double x, accel& a ) const {
      return gsl_interp_eval_deriv2( get(), xa.data(), ya.data(), x, a.get() ); }

    /**
     * C++ version of gsl_interp_eval_integ_e().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param a the lower integral limit
     * @param b the upper integral limit
     * @param acc an accelerator
     * @param result the interpolated integral between a and b (return value)
     * @return Error code on failure
     */
    template<typename XA,typename YA>
    int eval_integ_e( XA const& xa, YA const& ya, double a, double b,
		      accel& acc, double* result ) const {
      return gsl_interp_eval_integ_e( get(), xa.data(), ya.data(), a, b, acc.get(), result ); }

    /**
     * C++ version of gsl_interp_eval_integ().
     * This version handles std::vector and gsl::vector.
     * @param xa the array of x values
     * @param ya the array of y values
     * @param a the lower integral limit
     * @param b the upper integral limit
     * @param acc an accelerator
     * @return the interpolated integral between a and b 
     */
    template<typename XA,typename YA>
    double eval_integ( XA const& xa, YA const& ya, double a, double b, accel& acc ) const {
      return gsl_interp_eval_integ( get(), xa.data(), ya.data(), a, b, acc.get() ); }

    /**
     * C++ version of gsl_interp_bsearch().
     * This version handles std::vector and gsl::vector.
     * @param x_array the array of x values
     * @param x a given value x
     * @param index_lo lower bound for result
     * @param index_hi upper bound for result
     * @return the index
     */
    template<typename X>
    size_t bsearch( X const& x_array, double x, size_t index_lo, size_t index_hi ){
      return gsl_interp_bsearch( x_array.data(), x, index_lo, index_hi ); }

    // Types
    /**
     * Static type.
     * @return the named interpolation type
     */
    inline static type const* cspline(){ return gsl_interp_cspline; }
    /**
     * Static type.
     * @return the named interpolation type
     */
    inline static type const* cspline_periodic(){ return gsl_interp_cspline_periodic; }
    /**
     * Static type.
     * @return the named interpolation type
     */
    inline static type const* akima(){ return gsl_interp_akima; }
    /**
     * Static type.
     * @return the named interpolation type
     */
    inline static type const* akima_periodic(){ return gsl_interp_akima_periodic; }
  };
}
#endif
