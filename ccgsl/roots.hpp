/*
 * $Id: roots.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_ROOTS_HPP
#define CCGSL_ROOTS_HPP

#include<new>
#include<gsl/gsl_roots.h>
#include"function_scl.hpp"
#include"function_fdf.hpp"

namespace gsl {
  /**
   * One-dimensional root finding.
   */
  namespace root {
    /**
     * Workspace for root finding without a derivative. Also functions as a namespace
     * for fsolver functions. This also allows for functions to be called on fsolver as an object.
     */
    class fsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_root_fsolver_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      fsolver(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new fsolver of type fsolver::type
       * @param T The type of the fsolver
       */
      explicit fsolver( type const* T ){
	ccgsl_pointer = gsl_root_fsolver_alloc( T );
	// just plausibly we could allocate fsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_root_fsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_root_fsolver.
       * This is not usually a good idea. In this case
       * you should not use gsl_root_fsolver_free() to deallocate the memory.
       * @param v The fsolver
       */
      explicit fsolver( gsl_root_fsolver* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fsolver to copy.
       */
      fsolver( fsolver const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fsolver to copy
       */
      fsolver& operator=( fsolver const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_root_fsolver_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fsolver(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_root_fsolver_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The fsolver to move.
	 */
	fsolver( fsolver&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The fsolver to move.
	 * @return A reference to this.
	 */
	fsolver& operator=( fsolver&& v ){
	  fsolver( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fsolver are identically equal if their elements are identical.
       * @param v The fsolver to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fsolver const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fsolver are different if their elements are not identical.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fsolver const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fsolver const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fsolver is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fsolver. This works even if the fsolver have different sizes
       * because it swaps pointers.
       * @param v The fsolver to swap with @c this.
       */
      void swap( fsolver& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_root_fsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_root_fsolver.
       * @return the gsl_root_fsolver
       */
      gsl_root_fsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_root_fsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_root_fsolver.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fsolver objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_root_fsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_root_fsolver_set().
       * @param s The fsolver.
       * @param f The function to find a root of.
       * @param x_lower The lower bound of the initial search interval.
       * @param x_upper The upper bound of the initial search interval.
       * @return Error code on failure.
       */
      inline static int set( fsolver& s, gsl_function* f, double x_lower, double x_upper ){
	return gsl_root_fsolver_set( s.get(), f, x_lower, x_upper ); }

      /**
       * C++ version of gsl_root_fsolver_iterate().
       * @param s The fsolver.
       * @return Error code on failure.
       */
      inline static int iterate( fsolver& s ){
	return gsl_root_fsolver_iterate( s.get() ); }

      /**
       * C++ version of gsl_root_fsolver_name().
       * @param s The fsolver.
       * @return The name of the solver.
       */
      inline static std::string name( fsolver const& s ){
	return std::string( gsl_root_fsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_root_fsolver_root().
       * @param s The fsolver.
       * @return The current estimate of the root of f.
       */
      inline static double root( fsolver const& s ){
	return gsl_root_fsolver_root( s.get() ); }

      /**
       * C++ version of gsl_root_fsolver_x_lower().
       * @param s The fsolver.
       * @return The current lower bound for the search interval.
       */
      inline static double x_lower( fsolver const& s ){
	return gsl_root_fsolver_x_lower( s.get() ); }

     /**
       * C++ version of gsl_root_fsolver_x_upper().
       * @param s The fsolver.
       * @return The current upper bound for the search interval.
       */
      inline static double x_upper( fsolver const& s ){
	return gsl_root_fsolver_x_upper( s.get() ); }

      /**
       * C++ version of gsl_root_fsolver_set().
       * @param f The function to find a root of.
       * @param x_lower The lower bound of the initial search interval.
       * @param x_upper The upper bound of the initial search interval.
       * @return Error code on failure.
       */
      int set( gsl_function* f, double x_lower, double x_upper ){
	return gsl_root_fsolver_set( get(), f, x_lower, x_upper ); }

      /**
       * C++ version of gsl_root_fsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){ return gsl_root_fsolver_iterate( get() ); }

      /**
       * C++ version of gsl_root_fsolver_name().
       * @return The name of the solver.
       */
      char const* name() const { return gsl_root_fsolver_name( get() ); }

      /**
       * C++ version of gsl_root_fsolver_root().
       * @return The current estimate of the root of f.
       */
      double root() const { return gsl_root_fsolver_root( get() ); }

      /**
       * C++ version of gsl_root_fsolver_x_lower().
       * @return The current lower bound for the search interval.
       */
      double x_lower() const { return gsl_root_fsolver_x_lower( get() ); }

      /**
       * C++ version of gsl_root_fsolver_x_upper().
       * @return The current upper bound for the search interval.
       */
      double x_upper() const { return gsl_root_fsolver_x_upper( get() ); }

      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* bisection(){ return gsl_root_fsolver_bisection; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* brent(){ return gsl_root_fsolver_brent; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* falsepos(){ return gsl_root_fsolver_falsepos; }
    };

    /**
     * Workspace for root finding with a derivative. Also functions as a namespace
     * for fdfsolver functions. This also allows for functions to be called on fdfsolver as an
     * object.
     */
    class fdfsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_root_fdfsolver_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      fdfsolver(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new fdfsolver of one of the type fdfsolver::type.
       * @param T The fdfsolver type.
       */
      explicit fdfsolver( type const* T ){
	ccgsl_pointer = gsl_root_fdfsolver_alloc( T );
	// just plausibly we could allocate fdfsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_root_fdfsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_root_fdfsolver.
       * This is not usually a good idea. In this case
       * you should not use gsl_root_fdfsolver_free() to deallocate the memory.
       * @param v The fdfsolver
       */
      explicit fdfsolver( gsl_root_fdfsolver* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fdfsolver to copy.
       */
      fdfsolver( fdfsolver const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fdfsolver to copy
       */
      fdfsolver& operator=( fdfsolver const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_root_fdfsolver_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fdfsolver(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_root_fdfsolver_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The fdfsolver to move.
	 */
	fdfsolver( fdfsolver&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The fdfsolver to move.
	 * @return A reference to this.
	 */
	fdfsolver& operator=( fdfsolver&& v ){
	  fdfsolver( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fdfsolver are identically equal if their elements are identical.
       * @param v The fdfsolver to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fdfsolver const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fdfsolver are different if their elements are not identical.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fdfsolver const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fdfsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fdfsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fdfsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fdfsolver const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fdfsolver is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fdfsolver. This works even if the fdfsolver have different sizes
       * because it swaps pointers.
       * @param v The fdfsolver to swap with @c this.
       */
      void swap( fdfsolver& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_root_fdfsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_root_fdfsolver.
       * @return the gsl_root_fdfsolver
       */
      gsl_root_fdfsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_root_fdfsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_root_fdfsolver.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fdfsolver objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_root_fdfsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_root_fdfsolver_set().
       * @param s The solver.
       * @param fdf The function we want a root of.
       * @param root Initial guess of root.
       * @return Error code on failure.
       */
      inline static int set( fdfsolver& s, gsl_function_fdf* fdf, double root ){
	return gsl_root_fdfsolver_set( s.get(), fdf, root ); }

      /**
       * C++ version of gsl_root_fdfsolver_iterate().
       * @param s The solver.
       * @return Error code on failure.
       */
      inline static int iterate( fdfsolver& s ){ return gsl_root_fdfsolver_iterate( s.get() ); }

      /**
       * C++ version of gsl_root_fdfsolver_name().
       * @param s The solver.
       * @return UNDOCUMENTED
       */
      inline static std::string name( fdfsolver const& s ){
	return std::string( gsl_root_fdfsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_root_fdfsolver_root().
       * @param s The solver.
       * @return Current estimate of root.
       */
      inline static double root( fdfsolver const& s ){ return gsl_root_fdfsolver_root( s.get() ); }

      /**
       * C++ version of gsl_root_fdfsolver_set().
       * @param fdf The function we want a root of.
       * @param root Initial guess of root.
       * @return Error code on failure.
       */
      int set( gsl_function_fdf* fdf, double root ){
	return gsl_root_fdfsolver_set( get(), fdf, root ); }

      /**
       * C++ version of gsl_root_fdfsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){ return gsl_root_fdfsolver_iterate( get() ); }

      /**
       * C++ version of gsl_root_fdfsolver_name().
       * @return UNDOCUMENTED
       */
      char const* name(){ return gsl_root_fdfsolver_name( get() ); }

      /**
       * C++ version of gsl_root_fdfsolver_root().
       * @return Current estimate of root.
       */
      double root(){ return gsl_root_fdfsolver_root( get() ); }

      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* newton(){ return gsl_root_fdfsolver_newton; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* secant(){ return gsl_root_fdfsolver_secant; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* steffenson(){ return gsl_root_fdfsolver_steffenson; }
    };

    /**
     * Functions to test the precision of the current solution.
     */
    namespace test {
      /**
       * C++ version of gsl_root_test_interval().
       * @param x_lower Lower bound of root interval.
       * @param x_upper Lower bound of root interval.
       * @param epsabs Absolute error.
       * @param epsrel Relative error.
       * @return GSL_SUCCESS if condition met.
       */
      inline int interval( double x_lower, double x_upper, double epsabs, double epsrel ){
	return gsl_root_test_interval( x_lower, x_upper, epsabs, epsrel ); }

      /**
       * C++ version of gsl_root_test_residual().
       * @param f Residual value.
       * @param epsabs Absolute error.
       * @return GSL_SUCCESS if condition met.
       */
      inline int residual( double f, double epsabs ){
	return gsl_root_test_residual( f, epsabs ); }

      /**
       * C++ version of gsl_root_test_delta().
       * @param x1 First solution value.
       * @param x0 Zeroth solution value.
       * @param epsabs UNDOCUMENTED
       * @param epsrel Relative error.
       * @return GSL_SUCCESS if condition met.
       */
      inline int delta( double x1, double x0, double epsabs, double epsrel ){
	return gsl_root_test_delta( x1, x0, epsabs, epsrel ); }
    }
  }
}
#endif
