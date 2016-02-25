/*
 * $Id: eigen_gensymm.hpp 125 2012-02-19 18:18:25Z jdl3 $
 * Copyright (C) 2010, 2011, 2012 John D Lamb
 * Enum copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

#ifndef CCGSL_EIGEN_GENSYMM_HPP
#define CCGSL_EIGEN_GENSYMM_HPP

#include<gsl/gsl_eigen.h>
#include<new>
#include<iterator>

#include"vector.hpp"
#include"vector_complex.hpp"
#include"matrix.hpp"

namespace gsl {
  /**
   * This namespace contains functions for calculating eigenvectors and eigenvalues.
   */
  namespace eigen {
    /**
     * Workspace for gensymmetric matrices.
     */
    class gensymm_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      gensymm_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new gensymm_workspace with n elements
       * @param n The number of elements in the gensymm_workspace
       */
      explicit gensymm_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_gensymm_alloc( n );
	// just plausibly we could allocate gensymm_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_gensymm_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_gensymm_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_gensymm_workspace_free() to deallocate the memory.
       * @param v The gensymm_workspace
       */
      explicit gensymm_workspace( gsl_eigen_gensymm_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The gensymm_workspace to copy.
       */
      gensymm_workspace( gensymm_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The gensymm_workspace to copy
       */
      gensymm_workspace& operator=( gensymm_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_gensymm_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~gensymm_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_gensymm_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The gensymm_workspace to move.
       */
      gensymm_workspace( gensymm_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The gensymm_workspace to move.
       * @return A reference to this.
       */
      gensymm_workspace& operator=( gensymm_workspace&& v ){
	gensymm_workspace( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two gensymm_workspace are identically equal if their elements are identical.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( gensymm_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two gensymm_workspace are different equal if their elements are not identical.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( gensymm_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymm_workspace is nonnegative.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( gensymm_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymm_workspace is nonnegative.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( gensymm_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymm_workspace is nonnegative.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( gensymm_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymm_workspace is nonnegative.
       * @param v The gensymm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( gensymm_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the gensymm_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two gensymm_workspace. This works even if the gensymm_workspace have different sizes
       * because it swaps pointers.
       * @param v The gensymm_workspace to swap with @c this.
       */
      void swap( gensymm_workspace& v ){
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
      gsl_eigen_gensymm_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_gensymm_workspace.
       * @return the gsl_eigen_gensymm_workspace
       */
      gsl_eigen_gensymm_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_gensymm_workspace.
       * @return @c true or @c falses according as 
       * this is the only gensymm_workspace object sharing the gsl_eigen_gensymm_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many gensymm_workspace objects share this pointer.
       * @return the number of gensymm_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_gensymm_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_gensymm().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param eval This is where the eigenvalues are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int gensymm( gsl::matrix& A, gsl::matrix& B,
			gsl::vector& eval, gensymm_workspace& w ){
      return gsl_eigen_gensymm( A.get(), B.get(), eval.get(), w.get() ); }
    /**
     * Workspace for real gensymmetric matrices (eigenvectors and eigenvalues)
     */
    class gensymmv_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      gensymmv_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new gensymmv_workspace with n elements
       * @param n The number of elements in the gensymmv_workspace
       */
      explicit gensymmv_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_gensymmv_alloc( n );
	// just plausibly we could allocate gensymmv_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_gensymmv_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_gensymmv_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_gensymmv_workspace_free() to deallocate the memory.
       * @param v The gensymmv_workspace
       */
      explicit gensymmv_workspace( gsl_eigen_gensymmv_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The gensymmv_workspace to copy.
       */
      gensymmv_workspace( gensymmv_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The gensymmv_workspace to copy
       */
      gensymmv_workspace& operator=( gensymmv_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_gensymmv_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~gensymmv_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_gensymmv_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The gensymmv_workspace to move.
       */
      gensymmv_workspace( gensymmv_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The gensymmv_workspace to move.
       * @return A reference to this.
       */
      gensymmv_workspace& operator=( gensymmv_workspace&& v ){
	gensymmv_workspace( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two gensymmv_workspace are identically equal if their elements are identical.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( gensymmv_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two gensymmv_workspace are different equal if their elements are not identical.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( gensymmv_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymmv_workspace is nonnegative.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( gensymmv_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymmv_workspace is nonnegative.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( gensymmv_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymmv_workspace is nonnegative.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( gensymmv_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gensymmv_workspace is nonnegative.
       * @param v The gensymmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( gensymmv_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the gensymmv_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two gensymmv_workspace. This works even if the gensymmv_workspace have different sizes
       * because it swaps pointers.
       * @param v The gensymmv_workspace to swap with @c this.
       */
      void swap( gensymmv_workspace& v ){
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
      gsl_eigen_gensymmv_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_gensymmv_workspace.
       * @return the gsl_eigen_gensymmv_workspace
       */
      gsl_eigen_gensymmv_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_gensymmv_workspace.
       * @return @c true or @c false according as 
       * this is the only gensymmv_workspace object sharing the gsl_eigen_gensymmv_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many gensymmv_workspace objects share this pointer.
       * @return the number of gensymmv_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_gensymmv_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_gensymmv().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param eval This is where the eigenvalues are stored
     * @param evec This is where the eigenvectors are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int gensymmv( gsl::matrix& A, gsl::matrix& B, gsl::vector& eval,
			 gsl::matrix& evec, gensymmv_workspace& w ){
      return gsl_eigen_gensymmv( A.get(), B.get(), eval.get(), evec.get(), w.get() ); }
  }
}

#endif
