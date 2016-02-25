/*
 * $Id: multiroots.hpp 171 2012-06-19 19:15:15Z jdl3 $
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

#ifndef CCGSL_MULTIROOTS_HPP
#define CCGSL_MULTIROOTS_HPP

#include<gsl/gsl_multiroots.h>
#include"vector.hpp"
#include"multiroot_function.hpp"
#include"matrix.hpp"
#include"multiroot_function_fdf.hpp"

namespace gsl {
  /**
   * Namespace for multidimensional root finding.
   */
  namespace multiroot {
    /**
     * C++ version of gsl_multiroot_fdjacobian().
     * @param F The function.
     * @param x The function arguments.
     * @param f The function values at x.
     * @param epsrel relative error.
     * @param jacobian The Jacobian (return value).
     * @return Error code on failure.
     */
    inline int fdjacobian( gsl_multiroot_function* F, vector const& x, vector const& f,
			   double epsrel, matrix& jacobian ){
      return gsl_multiroot_fdjacobian( F, x.get(), f.get(), epsrel, jacobian.get() ); }

    /**
     * Workspace for solving multidimensional root finding problems. Also functions as a namespace
     * for fsolver functions. This also allows for functions to be called on fsolver as an object.
     */
    class fsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_multiroot_fsolver_type type;
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
       * The default constructor creates a new fsolver with n elements.
       * @param T The fsolver type.
       * @param n The number of elements in the fsolver.
       */
      explicit fsolver( type const* T, size_t const n ){
	ccgsl_pointer = gsl_multiroot_fsolver_alloc( T, n );
	// just plausibly we could allocate fsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multiroot_fsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multiroot_fsolver.
       * This is not usually a good idea. In this case
       * we should not use gsl_multiroot_fsolver_free() to deallocate the memory.
       * @param v The fsolver
       */
      explicit fsolver( gsl_multiroot_fsolver* v ){
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
	  if( ccgsl_pointer != 0 ) gsl_multiroot_fsolver_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_multiroot_fsolver_free( ccgsl_pointer );
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
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
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
       * Swap two fsolver objects. This works even if the fsolver have different sizes
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
      gsl_multiroot_fsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multiroot_fsolver.
       * @return the gsl_multiroot_fsolver
       */
      gsl_multiroot_fsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multiroot_fsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multiroot_fsolver.
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
       * to a gsl_multiroot_fsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multiroot_fsolver_set().
       * @param s The fsolver.
       * @param f The function.
       * @param x The initial guess of the root.
       * @return Error code on failure.
       */
      inline static int set( fsolver& s, function* f, vector const& x ){
	return gsl_multiroot_fsolver_set( s.get(), f, x.get() ); }

      /**
       * C++ version of gsl_multiroot_fsolver_iterate().
       * @param s The fsolver.
       * @return Error code on failure.
       */
      inline static int iterate( fsolver& s ){
	return gsl_multiroot_fsolver_iterate( s.get() ); }

      /**
       * C++ version of gsl_multiroot_fsolver_name().
       * @param s The fsolver.
       * @return The name of the solver.
       */
      inline static std::string name( fsolver const& s ){
	return std::string( gsl_multiroot_fsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_multiroot_fsolver_root().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fsolver.
       * @return The current estimate of the root.
       */
      inline static vector root( fsolver const& s ){
	vector result( gsl_multiroot_fsolver_root( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fsolver_dx().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fsolver.
       * @return The last step taken by the solver.
       */
      inline static vector dx( fsolver const& s ){
	vector result( gsl_multiroot_fsolver_dx( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fsolver_f().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fsolver.
       * @return The function value at the current estimate of the root.
       */
      inline static vector f( fsolver const& s ){
	vector result( gsl_multiroot_fsolver_f( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }
      
      // Member functions

      /**
       * C++ version of gsl_multiroot_fsolver_set().
       * @param f The function.
       * @param x The initial guess of the root.
       * @return Error code on failure.
       */
      int set( function* f, vector const& x ){
	return gsl_multiroot_fsolver_set( get(), f, x.get() ); }

      /**
       * C++ version of gsl_multiroot_fsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){
	return gsl_multiroot_fsolver_iterate( get() ); }

      /**
       * C++ version of gsl_multiroot_fsolver_name().
       * @return The name of the solver.
       */
      char const* name(){ return gsl_multiroot_fsolver_name( get() ); }

      /**
       * C++ version of gsl_multiroot_fsolver_root().
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the root.
       */
      vector const root(){
	vector result;
	result.wrap_gsl_vector_without_ownership( gsl_multiroot_fsolver_root( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fsolver_dx().
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The last step taken by the solver.
       */
      vector const dx(){
	vector result;
	result.wrap_gsl_vector_without_ownership( gsl_multiroot_fsolver_dx( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fsolver_f().
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The function value at the current estimate of the root.
       */
      vector const f(){
	vector result;
	result. wrap_gsl_vector_without_ownership( gsl_multiroot_fsolver_f( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      // Solver types
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* dnewton(){ return gsl_multiroot_fsolver_dnewton; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* broyden(){ return gsl_multiroot_fsolver_broyden; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* hybrid(){ return gsl_multiroot_fsolver_hybrid; }
      /**
       * Static type.
       * @return the named fsolver type
       */
      inline static type const* hybrids(){ return gsl_multiroot_fsolver_hybrids; }
    };

    /**
     * Workspace for solving multidimensional root finding problems. Also functions as a namespace
     * for fdfsolver functions. This also allows for functions to be called on fdfsolver as an object.
     */
    class fdfsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_multiroot_fdfsolver_type type;
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
       * The default constructor creates a new fdfsolver with n elements.
       * @param T The fdfsolver type.
       * @param n The number of elements in the fdfsolver.
       */
      explicit fdfsolver( type const* T, size_t const n ){
	ccgsl_pointer = gsl_multiroot_fdfsolver_alloc( T, n );
	// just plausibly we could allocate fdfsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multiroot_fdfsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multiroot_fdfsolver.
       * This is not usually a good idea. In this case
       * we should not use gsl_multiroot_fdfsolver_free() to deallocate the memory.
       * @param v The fdfsolver
       */
      explicit fdfsolver( gsl_multiroot_fdfsolver* v ){
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
	  if( ccgsl_pointer != 0 ) gsl_multiroot_fdfsolver_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_multiroot_fdfsolver_free( ccgsl_pointer );
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
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fdfsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fdfsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fdfsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
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
       * Swap two fdfsolver objects. This works even if the fdfsolver have different sizes
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
      gsl_multiroot_fdfsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multiroot_fdfsolver.
       * @return the gsl_multiroot_fdfsolver
       */
      gsl_multiroot_fdfsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multiroot_fdfsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multiroot_fdfsolver.
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
       * to a gsl_multiroot_fdfsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multiroot_fdfsolver_set().
       * @param s The fdfsolver.
       * @param f The function.
       * @param x The initial guess of the root.
       * @return Error code on failure.
       */
      inline static int set( fdfsolver& s, function_fdf* f, vector const& x ){
	return gsl_multiroot_fdfsolver_set( s.get(), f, x.get() ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_iterate().
       * @param s The fdfsolver.
       * @return Error code on failure.
       */
      inline static int iterate( fdfsolver& s ){
	return gsl_multiroot_fdfsolver_iterate( s.get() ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_name().
       * @param s The fdfsolver.
       * @return The name of the solver.
       */
      inline static std::string name( fdfsolver const& s ){
	return std::string( gsl_multiroot_fdfsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_root().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fdfsolver.
       * @return The current estimate of the root.
       */
      inline static vector root( fdfsolver const& s ){
	vector result( gsl_multiroot_fdfsolver_root( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fdfsolver_dx().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fdfsolver.
       * @return The last step taken by the solver.
       */
      inline static vector dx( fdfsolver const& s ){
	vector result( gsl_multiroot_fdfsolver_dx( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fdfsolver_f().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fdfsolver.
       * @return The function value at the current estimate of the root.
       */
      inline static vector f( fdfsolver const& s ){
	vector result( gsl_multiroot_fdfsolver_f( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }
      
      // Member functions

      /**
       * C++ version of gsl_multiroot_fdfsolver_set().
       * @param f The function.
       * @param x The initial guess of the root.
       * @return Error code on failure.
       */
      int set( function_fdf* f, vector const& x ){
	return gsl_multiroot_fdfsolver_set( get(), f, x.get() ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){
	return gsl_multiroot_fdfsolver_iterate( get() ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_name().
       * @return The name of the solver.
       */
      char const* name(){ return gsl_multiroot_fdfsolver_name( get() ); }

      /**
       * C++ version of gsl_multiroot_fdfsolver_root().
       * Note that it is @c this fdfsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the root.
       */
      vector const root(){
	vector result;
	result.wrap_gsl_vector_without_ownership( gsl_multiroot_fdfsolver_root( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fdfsolver_dx().
       * Note that it is @c this fdfsolver and not the return value
       * that owns the data in the return value.
       * @return The last step taken by the solver.
       */
      vector const dx(){
	vector result;
	result.wrap_gsl_vector_without_ownership( gsl_multiroot_fdfsolver_dx( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      /**
       * C++ version of gsl_multiroot_fdfsolver_f().
       * Note that it is @c this fdfsolver and not the return value
       * that owns the data in the return value.
       * @return The function value at the current estimate of the root.
       */
      vector const f(){
	vector result;
	result. wrap_gsl_vector_without_ownership( gsl_multiroot_fdfsolver_f( get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      // Solver types
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* hybridsj(){ return gsl_multiroot_fdfsolver_hybridsj; }
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* hybridj(){ return gsl_multiroot_fdfsolver_hybridj; }
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* newton(){ return gsl_multiroot_fdfsolver_newton; }
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* gnewton(){ return gsl_multiroot_fdfsolver_gnewton; }
    };

    /**
     * Functions to test the precision of the current solution.
     */
    namespace test {
      /**
       * C++ version of gsl_multiroot_test_delta().
       * @param dx Last solver step.
       * @param x Current estimate of root.
       * @param epsabs Absolute error.
       * @param epsrel Relative error.
       * @return GSL_SUCCESS if condition met.
       */
      inline int delta( vector const& dx, vector const& x, double epsabs, double epsrel ){
	return gsl_multiroot_test_delta( dx.get(), x.get(), epsabs, epsrel ); }
      
      /**
       * C++ version of gsl_multiroot_test_residual().
       * @param f Function values at current estimate of root.
       * @param epsabs Absolute error.
       * @return GSL_SUCCESS if condition met.
       */
      inline int residual( vector const& f, double epsabs ){
	return gsl_multiroot_test_residual( f.get(), epsabs ); }
    }
  }
}
#endif
