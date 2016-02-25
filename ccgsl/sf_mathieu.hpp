/*
 * $Id: sf_mathieu.hpp 293 2012-12-17 20:27:36Z jdl3 $
 * Copyright (C) 2010, 2011 John D Lamb
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

#ifndef CCGSL_SF_MATHIEU_HPP
#define CCGSL_SF_MATHIEU_HPP

#include<gsl/gsl_sf_mathieu.h>

#include<new>
#include<algorithm>
#include"mode.hpp"
#include"sf_result.hpp"

namespace gsl {
  namespace sf {
    /**
     * Angular and radial Mathieu functions.
     */
    namespace mathieu {
      /**
       * Workspace for general matrices.
       */
      class workspace {
      public:
	/**
	 * The default constructor is only really useful for assigning to.
	 */
	workspace(){
	  ccgsl_pointer = 0;
	  count = 0; // initially nullptr will do
	}
	// Refines random access container
	// Refines assignable
	/**
	 * The default constructor creates a new workspace with n elements
	 * @param nn An integer
	 * @param qq A real value
	 */
	explicit workspace( size_t const nn, double const qq ){
	  ccgsl_pointer = gsl_sf_mathieu_alloc( nn, qq );
	  // just plausibly we could allocate mathieu_workspace but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_sf_mathieu_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_sf_mathieu_workspace. This is not usually a good idea. In this case
	 * you should not use gsl_sf_mathieu_workspace_free() to deallocate the memory.
	 * @param v The workspace
	 */
	explicit workspace( gsl_sf_mathieu_workspace* v ){
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
	    if( ccgsl_pointer != 0 ) gsl_sf_mathieu_free( ccgsl_pointer );
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
	    if( ccgsl_pointer != 0 ) gsl_sf_mathieu_free( ccgsl_pointer );
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
	 * for example, for checking, that a workspace is nonnegative.
	 * @param v The workspace to be compared with @c this
	 * @return @c false or @c true according as @c this is greater than @c v
	 * lexicographically
	 */
	bool operator>( workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
	// operator<=
	/**
	 * A container needs to define an ordering for sorting. This uses
	 * standard lexicographical ordering and so is not useful,
	 * for example, for checking, that a workspace is nonnegative.
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
	/**
	 * The size of the workspace.
	 * @return The size of the workspace
	 */
	size_t size() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size; }
      private:
	/**
	 * The shared pointer
	 */
	gsl_sf_mathieu_workspace* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_sf_mathieu_workspace.
	 * @return the gsl_sf_mathieu_workspace
	 */
	gsl_sf_mathieu_workspace* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_sf_mathieu_workspace.
	 * @return @c true or @c falses according as 
	 * this is the only workspace object sharing the gsl_sf_mathieu_workspace
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
	 * to a gsl_sf_mathieu_workspace
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
      };
      /**
       * C++ version of gsl_sf_mathieu_a_array().
       * @param order_min An integer
       * @param order_max An integer
       * @param qq A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int a_array( int order_min, int order_max, double qq,
			  workspace& work, double result_array[] ){
	return gsl_sf_mathieu_a_array( order_min, order_max, qq, work.get(), result_array ); } 
      /**
       * C++ version of gsl_sf_mathieu_b_array().
       * @param order_min An integer
       * @param order_max An integer
       * @param qq A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int b_array( int order_min, int order_max, double qq,
			  workspace& work, double result_array[] ){
	return gsl_sf_mathieu_b_array( order_min, order_max, qq, work.get(), result_array ); } 
      /**
       * C++ version of gsl_sf_mathieu_a().
       * @param order An integer
       * @param qq A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int a( int order, double qq, result& result ){
	return gsl_sf_mathieu_a( order, qq, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_b().
       * @param order An integer
       * @param qq A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int b( int order, double qq, result& result ){
	return gsl_sf_mathieu_b( order, qq, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_a_coeff().
       * @param order An integer
       * @param qq A real value
       * @param aa A real value
       * @param coeff[] An array of doubles
       * @return Error code on failure
       */
      inline int a_coeff( int order, double qq, double aa, double coeff[] ){
	return gsl_sf_mathieu_a_coeff( order, qq, aa, coeff ); } 
      /**
       * C++ version of gsl_sf_mathieu_b_coeff().
       * @param order An integer
       * @param qq A real value
       * @param aa A real value
       * @param coeff[] An array of doubles
       * @return Error code on failure
       */
      inline int b_coeff( int order, double qq, double aa, double coeff[] ){
	return gsl_sf_mathieu_b_coeff( order, qq, aa, coeff ); } 
      /**
       * C++ version of gsl_sf_mathieu_ce().
       * @param order An integer
       * @param qq A real value
       * @param zz A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int ce( int order, double qq, double zz, result& result ){
	return gsl_sf_mathieu_ce( order, qq, zz, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_se().
       * @param order An integer
       * @param qq A real value
       * @param zz A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int se( int order, double qq, double zz, result& result ){
	return gsl_sf_mathieu_se( order, qq, zz, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_ce_array().
       * @param nmin An integer
       * @param nmax An integer
       * @param qq A real value
       * @param zz A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int ce_array( int nmin, int nmax, double qq, double zz,
			   workspace& work, double result_array[] ){
	return gsl_sf_mathieu_ce_array( nmin, nmax, qq, zz, work.get(), result_array ); } 
      /**
       * C++ version of gsl_sf_mathieu_se_array().
       * @param nmin An integer
       * @param nmax An integer
       * @param qq A real value
       * @param zz A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int se_array( int nmin, int nmax, double qq, double zz,
			   workspace& work, double result_array[] ){
	return gsl_sf_mathieu_se_array( nmin, nmax, qq, zz, work.get(), result_array ); } 
      /**
       * C++ version of gsl_sf_mathieu_Mc().
       * @param kind An integer
       * @param order An integer
       * @param qq A real value
       * @param zz A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int Mc( int kind, int order, double qq, double zz, result& result ){
	return gsl_sf_mathieu_Mc( kind, order, qq, zz, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_Ms().
       * @param kind An integer
       * @param order An integer
       * @param qq A real value
       * @param zz A real value
       * @param result A @c gsl::sf::result object
       * @return Error code on failure
       */
      inline int Ms( int kind, int order, double qq, double zz, result& result ){
	return gsl_sf_mathieu_Ms( kind, order, qq, zz, &result ); } 
      /**
       * C++ version of gsl_sf_mathieu_Mc_array().
       * @param kind An integer
       * @param nmin An integer
       * @param nmax An integer
       * @param qq A real value
       * @param zz A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int Mc_array( int kind, int nmin, int nmax, double qq, double zz,
			   workspace& work, double result_array[] ){
	return gsl_sf_mathieu_Mc_array( kind, nmin, nmax, qq, zz, work.get(), result_array ); } 
      /**
       * C++ version of gsl_sf_mathieu_Ms_array().
       * @param kind An integer
       * @param nmin An integer
       * @param nmax An integer
       * @param qq A real value
       * @param zz A real value
       * @param work A @c gsl::sf::mathieu::workspace object
       * @param result_array[] An array of doubles
       * @return Error code on failure
       */
      inline int Ms_array( int kind, int nmin, int nmax, double qq, double zz,
			   workspace& work, double result_array[] ){
	return gsl_sf_mathieu_Ms_array( kind, nmin, nmax, qq, zz, work.get(), result_array ); }
    }
  }
}

#endif
