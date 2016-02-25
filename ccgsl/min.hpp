/*
 * $Id: min.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_MIN_HPP
#define CCGSL_MIN_HPP

#include<new>
#include<gsl/gsl_min.h>
#include"function_scl.hpp"

namespace gsl {
  /**
   * One-dimensional minimisation.
   */
  namespace min {
    /**
     * Workspace for one-dimensional minimisation. Also acts like a namespace for fminimizer
     * functions. The functions can also be called on an fminimizer object.
     */
    class fminimizer {
    public:
      /**
       * Typedef.
       */
      typedef gsl_min_fminimizer_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      fminimizer(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The standard constructor creates a new minimizer of type fminimizer::type
       * @param T The type of the fminimizer
       */
      explicit fminimizer( type const* T ){
	ccgsl_pointer = gsl_min_fminimizer_alloc( T );
	// just plausibly we could allocate fminimizer but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_min_fminimizer_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_min_fminimizer.
       * This is not usually a good idea. In this case
       * you should not use gsl_min_fminimizer_free() to deallocate the memory.
       * @param v The fminimizer
       */
      explicit fminimizer( gsl_min_fminimizer* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fminimizer to copy.
       */
      fminimizer( fminimizer const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fminimizer to copy
       */
      fminimizer& operator=( fminimizer const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_min_fminimizer_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fminimizer(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_min_fminimizer_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The fminimizer to move.
       */
      fminimizer( fminimizer&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The fminimizer to move.
       * @return A reference to this.
       */
      fminimizer& operator=( fminimizer&& v ){
	fminimizer( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fminimizer are identically equal if their elements are identical.
       * @param v The fminimizer to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fminimizer const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fminimizer are different if their elements are not identical.
       * @param v The fminimizer to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fminimizer const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fminimizer is nonnegative.
       * @param v The fminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fminimizer const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fminimizer is nonnegative.
       * @param v The fminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fminimizer const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fminimizer is nonnegative.
       * @param v The fminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fminimizer const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fminimizer is nonnegative.
       * @param v The fminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fminimizer const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fminimizer is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fminimizer objects. This works even if the fminimizer have different sizes
       * because it swaps pointers.
       * @param v The fminimizer to swap with @c this.
       */
      void swap( fminimizer& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_min_fminimizer* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_min_fminimizer.
       * @return the gsl_min_fminimizer
       */
      gsl_min_fminimizer* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_min_fminimizer.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_min_fminimizer.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fminimizer objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_min_fminimizer.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_min_fminimizer_set().
       * @param s An fminimizer.
       * @param f the function to be minimised.
       * @param x_minimum Guess for value of x at which minimum occurs.
       * @param x_lower Lower bound of initial search interval.
       * @param x_upper Upper bound of initial search interval.
       * @return Error code on failure.
       */
      inline static int set( fminimizer& s, gsl_function* f, double x_minimum,
			     double x_lower, double x_upper ){
	return gsl_min_fminimizer_set( s.get(), f, x_minimum, x_lower, x_upper ); }

      /**
       * C++ version of gsl_min_fminimizer_set_with_values().
       * @param s An fminimizer.
       * @param f the function to be minimised.
       * @param x_minimum Guess for value of x at which minimum occurs.
       * @param f_minimum Value of f at x_minimum.
       * @param x_lower Lower bound of initial search interval.
       * @param f_lower Value of f at x_lower.
       * @param x_upper Upper bound of initial search interval.
       * @param f_upper Value of f at x_upper.
       * @return Error code on failure.
       */
      inline static int set_with_values( fminimizer& s, gsl_function* f,
					 double x_minimum, double f_minimum,
					 double x_lower, double f_lower,
					 double x_upper, double f_upper ){
	return gsl_min_fminimizer_set_with_values( s.get(), f, x_minimum, f_minimum,
						   x_lower, f_lower, x_upper, f_upper ); }

      /**
       * C++ version of gsl_min_fminimizer_iterate().
       * @param s An fminimizer.
       * @return Error code on failure.
       */
      inline static int iterate( fminimizer& s ){ return gsl_min_fminimizer_iterate( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_name().
       * @param s An fminimizer.
       * @return Name of fminimizer.
       */
      inline static std::string name( fminimizer const& s ){
	return gsl_min_fminimizer_name( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_minimum().
       * @param s An fminimizer.
       * @return Current estimate of x that minimises f.
       */
      inline static double x_minimum( fminimizer const& s ){
	return gsl_min_fminimizer_x_minimum( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_lower().
       * @param s An fminimizer.
       * @return Current lower bound of search interval.
       */
      inline static double x_lower( fminimizer const& s ){
	return gsl_min_fminimizer_x_lower( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_upper().
       * @param s An fminimizer.
       * @return Current upper bound of search interval.
       */
      inline static double x_upper( fminimizer const& s ){
	return gsl_min_fminimizer_x_upper( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_minimum().
       * @param s An fminimizer.
       * @return Current value of f at x_minimum.
       */
      inline static double f_minimum( fminimizer const& s ){
	return gsl_min_fminimizer_f_minimum( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_lower().
       * @param s An fminimizer.
       * @return Current value of f at x_lower.
       */
      inline static double f_lower( fminimizer const& s ){
	return gsl_min_fminimizer_f_lower( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_upper().
       * @param s An fminimizer.
       * @return Current value of f at x_upper.
       */
      inline static double f_upper( fminimizer const& s ){
	return gsl_min_fminimizer_f_upper( s.get() ); }

      /**
       * C++ version of gsl_min_fminimizer_set().
       * @param f the function to be minimised.
       * @param x_minimum Guess for value of x at which minimum occurs.
       * @param x_lower Lower bound of initial search interval.
       * @param x_upper Upper bound of initial search interval.
       * @return Error code on failure.
       */
      inline int set( gsl_function* f, double x_minimum, double x_lower, double x_upper ){
	return gsl_min_fminimizer_set( get(), f, x_minimum, x_lower, x_upper ); }

      /**
       * C++ version of gsl_min_fminimizer_set_with_values().
       * @param f the function to be minimised.
       * @param x_minimum Guess for value of x at which minimum occurs.
       * @param f_minimum Value of f at x_minimum.
       * @param x_lower Lower bound of initial search interval.
       * @param f_lower Value of f at x_lower.
       * @param x_upper Upper bound of initial search interval.
       * @param f_upper Value of f at x_upper.
       * @return Error code on failure.
       */
      inline int set_with_values( gsl_function* f, double x_minimum, double f_minimum,
				  double x_lower, double f_lower, double x_upper, double f_upper ){
	return gsl_min_fminimizer_set_with_values( get(), f, x_minimum, f_minimum,
						   x_lower, f_lower, x_upper, f_upper ); }

      /**
       * C++ version of gsl_min_fminimizer_iterate().
       * @return Error code on failure.
       */
      int iterate(){ return gsl_min_fminimizer_iterate( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_name().
       * @return Name of fminimizer.
       */
      char const* name() const { return gsl_min_fminimizer_name( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_minimum().
       * @return Current estimate of x that minimises f.
       */
      double x_minimum() const { return gsl_min_fminimizer_x_minimum( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_lower().
       * @return Current lower bound of search interval.
       */
      double x_lower(){ return gsl_min_fminimizer_x_lower( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_x_upper().
       * @return Current upper bound of search interval.
       */
      double x_upper() const { return gsl_min_fminimizer_x_upper( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_minimum().
       * @return Current value of f at x_minimum.
       */
      double f_minimum() const { return gsl_min_fminimizer_f_minimum( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_lower().
       * @return Current value of f at x_lower.
       */
      double f_lower() const { return gsl_min_fminimizer_f_lower( get() ); }

      /**
       * C++ version of gsl_min_fminimizer_f_upper().
       * @return Current value of f at x_upper.
       */
      double f_upper() const { return gsl_min_fminimizer_f_upper( get() ); }

      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* goldensection(){ return gsl_min_fminimizer_goldensection; }

      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* brent(){ return gsl_min_fminimizer_brent; }

      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* quad_golden(){ return gsl_min_fminimizer_quad_golden; }

    };

    /**
     * C++ version of gsl_min_test_interval().
     * @param x_lower Lower bound of interval containing minimum.
     * @param x_upper Lower bound of interval containing minimum.
     * @param epsabs Absolute error.
     * @param epsrel Relative error.
     * @return GSL_SUCCESS if condition met.
     */
    inline int test_interval( double x_lower, double x_upper, double epsabs, double epsrel ){
      return gsl_min_test_interval( x_lower, x_upper, epsabs, epsrel ); }

    /**
     * C++ version of gsl_min_find_bracket().
     * @param f the function to be minimised.
     * @param x_minimum Current estimate of the value that minimises f.
     * @param f_minimum Value of f at x_minimum.
     * @param x_lower Lower bound of the interval containing minimum.
     * @param f_lower Value of f at x_lower.
     * @param x_upper Upper bound of the interval containing minimum.
     * @param f_upper Value of f at x_upper.
     * @param eval_max Maximum number of evaluations.
     * @return GSL_SUCCESS if initial bracketing interval found; otherwise GSL_FAILURE.
     */
    inline int find_bracket( gsl_function* f, double* x_minimum, double* f_minimum,
			     double* x_lower, double* f_lower, double* x_upper,
			     double* f_upper, size_t eval_max ){
      return gsl_min_find_bracket( f, x_minimum, f_minimum, x_lower, f_lower, x_upper,
				   f_upper, eval_max ); }
  }
}
#endif
