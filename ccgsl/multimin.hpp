/*
 * $Id: multimin.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_MULTIMIN_HPP
#define CCGSL_MULTIMIN_HPP

#include<new>
#include"multimin_function_fdf.hpp"

namespace gsl {
  /**
   * Multidimensional minimisation.
   */
  namespace multimin {
    /**
     * C++ version of gsl_multimin_diff().
     * @param f function
     * @param x argument
     * @param g gradient
     * @return GSL_SUCCESS on success
     */
    inline int diff( gsl_multimin_function const* f, vector const& x, vector& g ){
      return gsl_multimin_diff( f, x.get(), g.get() ); }

    /**
     * Workspace for fminimizer. Also functions as a namespace for fminimizer functions.
     * This also allows for functions to be called on fminimizer as an object.
     */
    class fminimizer {
      /**
       * Typedef.
       */
      typedef gsl_multimin_fminimizer_type type;
    public:
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
       * The default constructor creates a new fminimizer with n elements.
       * @param T The type of the fminimizer
       * @param n The number of elements in the fminimizer (dimension of the function)
       */
      explicit fminimizer( type const* T, size_t const n ){
	ccgsl_pointer = gsl_multimin_fminimizer_alloc( T, n );
	// just plausibly we could allocate fminimizer but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multimin_fminimizer_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multimin_fminimizer.
       * This is not usually a good idea. In this case
       * we should not use gsl_multimin_fminimizer_free() to deallocate the memory.
       * @param v The fminimizer
       */
      explicit fminimizer( gsl_multimin_fminimizer* v ){
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
	  if( ccgsl_pointer != 0 ) gsl_multimin_fminimizer_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_multimin_fminimizer_free( ccgsl_pointer );
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
      gsl_multimin_fminimizer* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multimin_fminimizer.
       * @return the gsl_multimin_fminimizer
       */
      gsl_multimin_fminimizer* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multimin_fminimizer.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multimin_fminimizer.
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
       * to a gsl_multimin_fminimizer.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multimin_fminimizer_set().
       * @param s The fminimizer.
       * @param f The function.
       * @param x The initial starting point.
       * @param step_size The size of the first trial step.
       * @return Error code on failure.
       */
      inline static int set( fminimizer& s, gsl_multimin_function* f,
			  vector const& x, vector const& step_size ){
	return gsl_multimin_fminimizer_set( s.get(), f, x.get(), step_size.get() ); }
      
      /**
       * C++ version of gsl_multimin_fminimizer_name().
       * @param s The fminimizer.
       * @return The name of the fminimizer.
       */
      inline static char const* name( fminimizer const& s ){
	return gsl_multimin_fminimizer_name( s.get() ); } 
      
      /**
       * C++ version of gsl_multimin_fminimizer_iterate().
       * @param s The fminimizer.
       * @return Error code on failure.
       */
      inline static int iterate( fminimizer& s ){
	return gsl_multimin_fminimizer_iterate( s.get() ); }

      /**
       * C++ version of gsl_multimin_fminimizer_x().
       * @param s The fminimizer.
       * @return The estimate of x that minimises f.
       */
      inline static vector x( fminimizer const& s ){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fminimizer_x( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }
      /**
       * C++ version of gsl_multimin_fminimizer_minimum().
       * @param s The fminimizer.
       * @return The value of f at the value of x that is estimated to minimise f.
       */
      inline static double minimum( fminimizer const& s ){
	return gsl_multimin_fminimizer_minimum( s.get() );
      }
      
      /**
       * C++ version of gsl_multimin_fminimizer_size().
       * @param s The fminimizer.
       * @return The estimate of size.
       */
      inline static double size( fminimizer const& s ){
	return gsl_multimin_fminimizer_size( s.get() ); }
      /**
       * C++ version of gsl_multimin_fminimizer_set().
       * @param f The function.
       * @param x The initial starting point.
       * @param step_size The size of the first trial step.
       * @return Error code on failure.
       */
      int set( gsl_multimin_function* f, vector const& x, vector const& step_size  ){
	return gsl_multimin_fminimizer_set( get(), f, x.get(), step_size.get() ); }
      /**
       * C++ version of gsl_multimin_fminimizer_name().
       * @return The name of the fminimizer.
       */
      char const* name(){ return gsl_multimin_fminimizer_name( get() ); } 
      /**
       * C++ version of gsl_multimin_fminimizer_iterate().
       * @return Error code on failure.
       */
      int iterate(){
	return gsl_multimin_fminimizer_iterate( get() ); }
      /**
       * C++ version of gsl_multimin_fminimizer_x().
       * @return The estimate of x that minimises f.
       */
      vector x(){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fminimizer_x( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }
      /**
       * C++ version of gsl_multimin_fminimizer_minimum().
       * @return The value of f at the value of x that is estimated to minimise f.
       */
      double minimum(){
	return gsl_multimin_fminimizer_minimum( get() );
      }
      /**
       * C++ version of gsl_multimin_fminimizer_size().
       * @return The estimate of size.
       */
      double size(){
	return gsl_multimin_fminimizer_size( get() ); }

      // Fminimizer types
      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* nmsimplex(){ return gsl_multimin_fminimizer_nmsimplex; }
      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* nmsimplex2(){ return gsl_multimin_fminimizer_nmsimplex2; }
      /**
       * Static type.
       * @return the named fminimizer type
       */
      inline static type const* nmsimplex2rand(){ return gsl_multimin_fminimizer_nmsimplex2rand; }
    };
    
    /**
     * Functions to test the precision of the current solution.
     */
    namespace test {
      /**
       * C++ version of gsl_multimin_test_gradient().
       * @param g A gradient
       * @param epsabs An absolute tolerance
       * @return GSL_SUCCESS if test condition met; GSL_CONTINUE otherwise
       */
      inline int gradient( vector const& g, double epsabs ){
	return gsl_multimin_test_gradient( g.get(), epsabs ); }
      
      /**
       * C++ version of gsl_multimin_test_size().
       * @param size A size (if applicable)
       * @param epsabs An absolute tolerance
       * @return GSL_SUCCESS if test condition met; GSL_CONTINUE otherwise
       */
      inline int size( double const size, double epsabs ){
	return gsl_multimin_test_size( size, epsabs ); }
    }

    /**
     * Workspace for fdfminimizer. Also functions as a namespace for fdfminimizer functions.
     * This also allows for functions to be called on fminimizer as an object.
     */
    class fdfminimizer {
      /**
       * Typedef.
       */
      typedef gsl_multimin_fdfminimizer_type type;
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      fdfminimizer(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new fdfminimizer with n elements.
       * @param T The type of the fminimizer
       * @param n The number of elements in the fdfminimizer
       */
      explicit fdfminimizer( type const* T, size_t const n ){
	ccgsl_pointer = gsl_multimin_fdfminimizer_alloc( T, n );
	// just plausibly we could allocate fdfminimizer but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multimin_fdfminimizer_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multimin_fminimizer.
       * This is not usually a good idea. In this case
       * we should not use gsl_multimin_fminimizer_free() to deallocate the memory.
       * @param v The fdfminimizer
       */
      explicit fdfminimizer( gsl_multimin_fdfminimizer* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fdfminimizer to copy.
       */
      fdfminimizer( fdfminimizer const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fdfminimizer to copy
       */
      fdfminimizer& operator=( fdfminimizer const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_multimin_fdfminimizer_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fdfminimizer(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_multimin_fdfminimizer_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The fdfminimizer to move.
	 */
	fdfminimizer( fdfminimizer&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The fdfminimizer to move.
	 * @return A reference to this.
	 */
	fdfminimizer& operator=( fdfminimizer&& v ){
	  fdfminimizer( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fdfminimizer are identically equal if their elements are identical.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fdfminimizer const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fdfminimizer are different if their elements are not identical.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fdfminimizer const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfminimizer is nonnegative.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fdfminimizer const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfminimizer is nonnegative.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fdfminimizer const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfminimizer is nonnegative.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fdfminimizer const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfminimizer is nonnegative.
       * @param v The fdfminimizer to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fdfminimizer const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fdfminimizer is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fdfminimizer objects. This works even if the fdfminimizer have different sizes
       * because it swaps pointers.
       * @param v The fdfminimizer to swap with @c this.
       */
      void swap( fdfminimizer& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_multimin_fdfminimizer* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multimin_fminimizer.
       * @return the gsl_multimin_fminimizer
       */
      gsl_multimin_fdfminimizer* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multimin_fminimizer.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multimin_fminimizer.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fdfminimizer objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_multimin_fminimizer.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multimin_fdfminimizer_set().
       * @param s An fdfminimizer
       * @param fdf An fdfunction
       * @param x A vector argument
       * @param step_size The step size
       * @param tol The tolerance of the line search method
       * @return Error code on failure
       */
      inline static int set( fdfminimizer& s, function_fdf* fdf, vector const& x,
		      double step_size, double tol ){
	return gsl_multimin_fdfminimizer_set( s.get(), fdf, x.get(), step_size, tol ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_name().
       * @param s An fdfminimizer
       * @return The name of the minimizer
       */
      inline static char const* name( fdfminimizer const& s ){
	return gsl_multimin_fdfminimizer_name( s.get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_iterate().
       * @param s An fdfminimizer
       * @return Error code on failure
       */
      inline static int iterate( fdfminimizer& s ){
	return gsl_multimin_fdfminimizer_iterate( s.get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_restart().
       * @param s An fdfminimizer
       * @return Error code on failure
       */
      inline static int restart( fdfminimizer& s ){
	return gsl_multimin_fdfminimizer_restart( s.get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_x().
       * @param s An fdfminimizer
       * @return The current value of the argmin estimate
       */
      inline static vector x( fdfminimizer const& s ){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_x( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_dx().
       * @param s An fdfminimizer
       * @return The vlaue of dx
       */
      inline static vector dx( fdfminimizer const& s ){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_dx( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_gradient().
       * @param s An fdfminimizer
       * @return The current gradient estimate
       */
      inline static vector gradient( fdfminimizer const& s ){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_gradient( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_minimum().
       * @param s An fdfminimizer
       * @return The current estimate of the function minimum
       */
      inline static double minimum( fdfminimizer const& s ){
	return gsl_multimin_fdfminimizer_minimum( s.get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_set().
       * @param fdf An fdfunction
       * @param x A vector argument
       * @param step_size The step size
       * @param tol The tolerance of the line search method
       * @return Error code on failure
       */
      inline int set( function_fdf* fdf, vector const& x,
		      double step_size, double tol ){
	return gsl_multimin_fdfminimizer_set( get(), fdf, x.get(), step_size, tol ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_name().
       * @return The name of the minimizer
       */
      inline char const* name(){ return gsl_multimin_fdfminimizer_name( get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_iterate().
       * @return Error code on failure
       */
      inline int iterate(){
	return gsl_multimin_fdfminimizer_iterate( get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_restart().
       * @return Error code on failure
       */
      inline int restart(){
	return gsl_multimin_fdfminimizer_restart( get() ); }

      /**
       * C++ version of gsl_multimin_fdfminimizer_x().
       * @return The current value of the argmin estimate
       */
      inline vector x(){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_x( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_dx().
       * @return The vlaue of dx
       */
      inline vector dx(){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_dx( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_gradient().
       * @return The current gradient estimate
       */
      inline vector gradient(){
	vector v;
	v.wrap_gsl_vector_without_ownership( gsl_multimin_fdfminimizer_gradient( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( v );
#else
	return v;
#endif
      }

      /**
       * C++ version of gsl_multimin_fdfminimizer_minimum().
       * @return The current estimate of the function minimum
       */
      inline double minimum(){
	return gsl_multimin_fdfminimizer_minimum( get() ); }

      /**
       * Static type.
       * @return the named fdfminimizer type
       */
      inline static type const* steepest_descent(){ return gsl_multimin_fdfminimizer_steepest_descent; }
      /**
       * Static type.
       * @return the named fdfminimizer type
       */
      inline static type const* conjugate_pr(){ return gsl_multimin_fdfminimizer_conjugate_pr; }
      /**
       * Static type.
       * @return the named fdfminimizer type
       */
      inline static type const* conjugate_fr(){ return gsl_multimin_fdfminimizer_conjugate_fr; }
      /**
       * Static type.
       * @return the named fdfminimizer type
       */
      inline static type const* vector_bfgs(){ return gsl_multimin_fdfminimizer_vector_bfgs; }
      /**
       * Static type.
       * @return the named fdfminimizer type
       */
      inline static type const* vector_bfgs2(){ return gsl_multimin_fdfminimizer_vector_bfgs2; }
    };


  }
}
#endif
