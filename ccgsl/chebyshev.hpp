/*
 * $Id: chebyshev.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_CHEB_HPP
#define CCGSL_CHEB_HPP

#include<new>
#include<gsl/gsl_chebyshev.h>
#include"mode.hpp"

namespace gsl {
  /**
   * Chebyshev_approximations to univariate functions.
   */
  namespace cheb {
    /**
     * Workspace for Chebyshev series
     */
    class series {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      series(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new series with n elements.
       * @param order The order of the series
       */
      explicit series( size_t const order ){
	ccgsl_pointer = gsl_cheb_alloc( order );
	// just plausibly we could allocate series but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_cheb_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_cheb_series.
       * This is not usually a good idea. In this case
       * we should not use gsl_cheb_free() to deallocate the memory.
       * @param v The series
       */
      explicit series( gsl_cheb_series* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The series to copy.
       */
      series( series const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The series to copy
       */
      series& operator=( series const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_cheb_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~series(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_cheb_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The series to move.
       */
      series( series&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The series to move.
       * @return A reference to this.
       */
      series& operator=( series&& v ){
	series( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two series are identically equal if their elements are identical.
       * @param v The series to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( series const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two series are different if their elements are not identical.
       * @param v The series to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( series const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a series is nonnegative.
       * @param v The series to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( series const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a series is nonnegative.
       * @param v The series to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( series const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a series is nonnegative.
       * @param v The series to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( series const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a series is nonnegative.
       * @param v The series to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( series const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the series is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two series objects. This works even if the series have different sizes
       * because it swaps pointers.
       * @param v The series to swap with @c this.
       */
      void swap( series& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_cheb_series* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_cheb_series.
       * @return the gsl_cheb_series
       */
      gsl_cheb_series* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_cheb_series.
       * @return @c true or @c falses according as 
       * this is the only series object sharing the gsl_cheb_series.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many series objects share this pointer.
       * @return the number of series objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_cheb_series.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    public:
      /**
       * C++ version of gsl_cheb_init().
       * @param func A gsl_function or gsl::function_scl
       * @param a The lower interval point
       * @param b The upper interval point
       * @return Error code on failure
       */
      int init( gsl_function const* func, double const a, double const b ){
	return gsl_cheb_init( get(), func, a, b ); }
    };

    /**
     * C++ version of gsl_cheb_init().
     * @param cs The cheb::series
     * @param func A gsl_function or gsl::function_scl
     * @param a The lower interval point
     * @param b The upper interval point
     * @return Error code on failure
     */
    inline int init( series& cs, gsl_function const* func, double const a, double const b ){
      return gsl_cheb_init( cs.get(), func, a, b ); }

    /**
     * C++ version of gsl_cheb_order().
     * @param cs The cheb::series
     * @return The order of the series
     */
    inline size_t order( series const& cs ){ return gsl_cheb_order( cs.get() ); }

    /**
     * C++ version of gsl_cheb_size().
     * @param cs The cheb::series
     * @return The size of the Chebyshev coefficient array
     */
    inline size_t size( series const& cs ){ return gsl_cheb_size( cs.get() ); }

    /**
     * C++ version of gsl_cheb_coeffs().
     * @param cs The cheb::series
     * @return The array of Chebyshev coefficients
     */
    inline double* coeffs( series const& cs ){ return gsl_cheb_coeffs( cs.get() ); }

    /**
     * C++ version of gsl_cheb_eval().
     * @param cs The cheb::series
     * @param x The point at which to evaluate the Chebyshev series
     * @return The series evaluated at x
     */
    inline double eval( series const& cs, double const x )
    { return gsl_cheb_eval( cs.get(), x ); }

    /**
     * C++ version of gsl_cheb_eval_err().
     * @param cs The cheb::series
     * @param x The point at which to evaluate the Chebyshev series
     * @param result The series evaluated at x [return value]
     * @param abserr The absolute error estimate [return value]
     * @return Error code on failure
     */
    inline int eval_err( series const& cs, double const x, double* result, double* abserr ){
      return gsl_cheb_eval_err( cs.get(), x, result, abserr ); }

    /**
     * C++ version of gsl_cheb_eval_n().
     * @param cs The cheb::series
     * @param order The order to which to evaluate the series
     * @param x The point at which to evaluate the Chebyshev series
     * @return The Chebyshev series evaluated at x to order at most order
     */
    inline double eval_n( series const& cs, size_t const order, double const x ){
      return gsl_cheb_eval_n( cs.get(), order, x ); }
    
    /**
     * C++ version of gsl_cheb_eval_n_err().
     * @param cs The cheb::series
     * @param order The order to which to evaluate the series
     * @param x The point at which to evaluate the Chebyshev series
     * @param result The series evaluated at x to (at most) the given order [return value]
     * @param abserr The absolute error estimate [return value]
     * @return Error code on failure
     */
    inline int eval_n_err( series const& cs, size_t const order, double const x,
			   double* result, double* abserr ){
      return gsl_cheb_eval_n_err( cs.get(), order, x, result, abserr ); }

    /**
     * C++ version of gsl_cheb_eval_mode().
     * @param cs The cheb::series
     * @param x The point at which to evaluate the Chebyshev series
     * @param mode The mode (DEFAULT, DOUBLE, SINGLE or APPROX)
     * @return The Chebyshev series evaluated at x in the given mode
     */
    inline double eval_mode( series const& cs, double const x, gsl::mode_t mode ){
      return gsl_cheb_eval_mode( cs.get(), x, mode ); }
    
    /**
     * C++ version of gsl_cheb_eval_mode_e().
     * @param cs The cheb::series
     * @param x The point at which to evaluate the Chebyshev series
     * @param mode The mode (DEFAULT, DOUBLE, SINGLE or APPROX)
     * @param result The Chebyshev series evaluated at x in the given mode [return value]
     * @param abserr The absolute error estimate [return value]
     * @return Error code on failure
     */
    inline int eval_mode_e( series const& cs, double const x, gsl_mode_t mode,
		     double* result, double* abserr ){
      return gsl_cheb_eval_mode_e( cs.get(), x, mode, result, abserr ); }

    /**
     * C++ version of gsl_cheb_calc_deriv().
     * @param deriv The cheb::series for the deriv
     * @param cs The cheb::series
     * @return Error code on failure
     */
    inline int calc_deriv( series& deriv, series const& cs ){
      return gsl_cheb_calc_deriv( deriv.get(), cs.get() ); }

    /**
     * C++ version of gsl_cheb_calc_integ().
     * @param integ The cheb::series for the integral
     * @param cs The cheb::series
     * @return Error code on failure
     */
    inline int calc_integ( series& integ, series const& cs ){
      return gsl_cheb_calc_integ( integ.get(), cs.get() ); }
  }
}
#endif
