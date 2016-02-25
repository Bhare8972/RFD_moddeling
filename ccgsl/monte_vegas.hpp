/*
 * $Id: monte_vegas.hpp 236 2012-08-08 19:47:53Z jdl3 $
 * Copyright (C) 2010, 2011, 2012 John D Lamb
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

#ifndef CCGSL_MONTE_VEGAS_HPP
#define CCGSL_MONTE_VEGAS_HPP

#include<gsl/gsl_monte_vegas.h>
#include"monte.hpp"

namespace gsl {
  /**
   * Monte Carlo integration.
   */
  namespace monte {
    /**
     * Monte Carlo integration using VEGAS.
     */
    namespace vegas {
      /**
       * Workspace for Monte Carlo integration using the VEGAS algorithm.
       */
      class state {
      public:
	/**
	 * The default constructor is only really useful for assigning to.
	 */
	state(){
	  ccgsl_pointer = 0;
	  count = 0; // ititially nullptr will do
	}
	// Refines random access container
	// Refines assignable
	/**
	 * The default constructor creates a new state with n elements.
	 * @param dim The number of function arguments
	 */
	explicit state( size_t const dim ){
	  ccgsl_pointer = gsl_monte_vegas_alloc( dim );
	  // just plausibly we could allocate state but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_monte_vegas_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_monte_vegas_state.
	 * This is not usually a good idea. In this case
	 * we should not use gsl_monte_vegas_free() to deallocate the memory.
	 * @param v The state
	 */
	explicit state( gsl_monte_vegas_state* v ){
	  ccgsl_pointer = v;
	  // just plausibly we could fail to allocate count: no further action needed.
	  count = new size_t;
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	// copy constructor
	/**
	 * The copy constructor. This creates a new reference to the workspace.
	 * @param v The state to copy.
	 */
	state( state const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	  count = v.count; if( count != 0 ) ++*count; }
	/**
	 * Initialise. This allows this state to be reused.
	 * @return Error code on failure
	 */
	int init(){ return gsl_monte_vegas_init( ccgsl_pointer ); }
	// assignment operator
	/**
	 * The assignment operator. This copies elementwise.
	 * @param v The state to copy
	 */
	state& operator=( state const& v ){
	  // first, possibly delete anything pointed to by this
	  if( count == 0 or --*count == 0 ){
	    if( ccgsl_pointer != 0 ) gsl_monte_vegas_free( ccgsl_pointer );
	    delete count;
	  } // Then copy
	  ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
	}
	// destructor
	/**
	 * The destructor only deletes the pointers if count reaches zero.
	 */
	~state(){
	  if( count == 0 or --*count == 0 ){
	    // could have allocated null pointer
	    if( ccgsl_pointer != 0 ) gsl_monte_vegas_free( ccgsl_pointer );
	    delete count;
	  }
	}
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The state to move.
	 */
	state( state&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The state to move.
	 * @return A reference to this.
	 */
	state& operator=( state&& v ){
	  state( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
	// Refines equality comparable
	// == operator
	/**
	 * Two state are identically equal if their elements are identical.
	 * @param v The state to be compared with @c this
	 * @return @c true or @c false according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator==( state const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
	// != operator
	/**
	 * Two state are different if their elements are not identical.
	 * @param v The state to be compared with @c this
	 * @return @c false or @c true according as @c this and @c v have
	 * identical elements or not
	 */
	bool operator!=( state const& v ) const { return not operator==( v ); }
	// Refines forward container
	// Refines less than comparable
	// operator<
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for example, for checking, that a state is nonnegative.
	 * @param v The state to be compared with @c this
	 * @return @c false or @c true according as @c this is less than @c v
	 * lexicographically
	 */
	bool operator<( state const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
	// operator>
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for example, for checking, that a state is nonnegative.
	 * @param v The state to be compared with @c this
	 * @return @c false or @c true according as @c this is greater than @c v
	 * lexicographically
	 */
	bool operator>( state const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
	// operator<=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for example, for checking, that a state is nonnegative.
	 * @param v The state to be compared with @c this
	 * @return @c false or @c true according as @c this is less than
	 * or equal to @c v lexicographically
	 */
	bool operator<=( state const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
	// operator>=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for example, for checking, that a state is nonnegative.
	 * @param v The state to be compared with @c this
	 * @return @c false or @c true according as @c this is no 
	 * less than @c v lexicographically
	 */
	bool operator>=( state const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
	/**
	 * Find if the state is empty.
	 * @return @c true if has size zero; otherwise @c false
	 */
	bool empty() const { return ccgsl_pointer == 0; }
	// swap() --- should work even if sizes don't match
	/**
	 * Swap two state objects. This works even if the state have different sizes
	 * because it swaps pointers.
	 * @param v The state to swap with @c this.
	 */
	void swap( state& v ){
	  std::swap( ccgsl_pointer, v.ccgsl_pointer );
	  std::swap( count, v.count );
	}
      private:
	/**
	 * The shared pointer
	 */
	gsl_monte_vegas_state* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_monte_vegas_state.
	 * @return the gsl_monte_vegas_state
	 */
	gsl_monte_vegas_state* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_monte_vegas_state.
	 * @return @c true or @c falses according as 
	 * this is the only state object sharing the gsl_monte_vegas_state.
	 */
	bool unique() const { return count != 0 and *count == 1; }
	/**
	 * Find how many state objects share this pointer.
	 * @return the number of state objects that share this pointer.
	 */
	size_t use_count() const { return count == 0 ? 0 : *count; }
	/**
	 * Allow conversion to bool.
	 * @return @c true or @c false according as this contains a pointer
	 * to a gsl_monte_vegas_state.
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
	// Access params
	/**
	 * Get stage.
	 * @return stage
	 */
	int get_stage() const { return ccgsl_pointer->stage; }
	/**
	 * Get iterations.
	 * @return iterations
	 */
	size_t get_iterations() const { return ccgsl_pointer->iterations; }
	/**
	 * Get mode.
	 * @return mode
	 */
	size_t get_mode() const { return ccgsl_pointer->mode; }
	/**
	 * Get alpha.
	 * @return alpha
	 */
	double get_alpha() const { return ccgsl_pointer->alpha; }
	/**
	 * Get verbose.
	 * @return verbose
	 */
	int get_verbose() const { return ccgsl_pointer->verbose; }
	/**
	 * Get ostream.
	 * @return ostream
	 */
	FILE* get_ostream() const { return ccgsl_pointer->ostream; }
	/**
	 * Set stage.
	 * @param stage New value for stage
	 */
	void set_stage( int const stage ){ ccgsl_pointer->stage = stage; }
	/**
	 * Set iterations.
	 * @param iterations New value for iterations
	 */
	void set_iterations( size_t const iterations ){ ccgsl_pointer->iterations = iterations; }
	/**
	 * Set mode.
	 * @param  mode New value for mode
	 */
	void set_mode( int const mode ){ ccgsl_pointer->mode = mode; }
	/**
	 * Set alpha.
	 * @param alpha New value for alpha
	 */
	void set_alpha( double const alpha ){ ccgsl_pointer->alpha = alpha; }
	/**
	 * Set verbose.
	 * @param verbose New value for verbose
	 */
	void set_verbose( int const verbose ){ ccgsl_pointer->verbose = verbose; }
	/**
	 * Set ostream.
	 * @param  ostream New value for ostream
	 */
	void set_ostream( FILE* ostream ){ ccgsl_pointer->ostream = ostream; }
	// Other functions
	/**
	 * Get chi-squared value: this should be close to 1.
	 * @return chi-squared value
	 */
	double chisq() const { return gsl_monte_vegas_chisq( ccgsl_pointer ); }
	/**
	 * Returns the raw (unaveraged) values of the integral result and its error
	 * sigma from the most recent iteration of the algorithm.
	 * @param result The raw value (return value)
	 * @param sigma The error (return value)
	 */
	void runval( double* result, double* sigma ){
	  gsl_monte_vegas_runval( ccgsl_pointer, result, sigma ); }
      };
      /**
       * C++ version of gsl_monte_vegas_init(). This allows this state to be reused.
       * @param state The workspace
       * @return Error code on failure
       */
      inline int init( gsl::monte::vegas::state& state ){ return gsl_monte_vegas_init( state.get() ); }
      /**
       * C++ version of gsl_monte_vegas_integrate().
       * @param f The function
       * @param xl[] An array of size dim
       * @param xu[] An array of size dim
       * @param dim The dimension of the function
       * @param calls The fixed number of function calls to use
       * @param r A random number generator
       * @param state The state
       * @param result Storage for result
       * @param abserr Estimated absolute error
       * @return Error code on failure
       */
      inline int integrate( gsl::monte::function* f, double xl[],
			    double xu[], size_t const dim, size_t const calls,
			    gsl::rng& r, state& state, double* result, double* abserr ){
	return gsl_monte_vegas_integrate( f, xl, xu, dim, calls, r.get(), state.get(),
					  result, abserr ); } 
      /**
       * C++ version of gsl_monte_vegas_integrate(). This version can use gsl::vector
       * or std::vector instead of arrays of doubles.
       * @param f The function
       * @param xl An array of size dim (lower bounds)
       * @param xu An array of size dim (upper bounds)
       * @param calls The fixed number of function calls to use
       * @param r A random number generator
       * @param state The state
       * @param result Storage for result
       * @param abserr Estimated absolute error
       * @return Error code on failure
       */
      template<typename L, typename U>
      inline int integrate( gsl::monte::function* f, L& xl,
			    U& xu, size_t const calls,
			    gsl::rng& r, state& state, double* result, double* abserr ){
	size_t const dim = xl.size();
	if( dim != xu.size() )
	  GSL_ERROR( "Mismatch in array lengths", GSL_EBADLEN );
	    return gsl_monte_vegas_integrate( f, xl.data(), xu.data(), dim, calls,
					      r.get(), state.get(), result, abserr ); } 
    }
  }
}
#endif
