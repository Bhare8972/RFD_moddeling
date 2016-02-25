/*
 * $Id: eigen_symm.hpp 125 2012-02-19 18:18:25Z jdl3 $
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

#ifndef CCGSL_EIGEN_SYMM_HPP
#define CCGSL_EIGEN_SYMM_HPP

#include<gsl/gsl_eigen.h>
#include<new>
#include<iterator>

#include"vector.hpp"
#include"matrix.hpp"

namespace gsl {
  /**
   * This namespace contains functions for calculating eigenvectors and eigenvalues.
   */
  namespace eigen {
    /**
     * Workspace for real symmetric matrices.
     */
    class symm_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      symm_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new symm_workspace with n elements
       * @param n The number of elements in the symm_workspace
       */
      explicit symm_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_symm_alloc( n );
	// just plausibly we could allocate symm_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_symm_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_symm_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_symm_workspace_free() to deallocate the memory.
       * @param v The symm_workspace
       */
      explicit symm_workspace( gsl_eigen_symm_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The symm_workspace to copy.
       */
      symm_workspace( symm_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The symm_workspace to copy
       */
      symm_workspace& operator=( symm_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_symm_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~symm_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_symm_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The symm_workspace to move.
       */
      symm_workspace( symm_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The symm_workspace to move.
       * @return A reference to this.
       */
      symm_workspace& operator=( symm_workspace&& v ){
	symm_workspace( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two symm_workspace are identically equal if their elements are identical.
       * @param v The symm_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( symm_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two symm_workspace are different equal if their elements are not identical.
       * @param v The symm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( symm_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symm_workspace is nonnegative.
       * @param v The symm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( symm_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symm_workspace is nonnegative.
       * @param v The symm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( symm_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symm_workspace is nonnegative.
       * @param v The symm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( symm_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symm_workspace is nonnegative.
       * @param v The symm_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( symm_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the symm_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two symm_workspace. This works even if the symm_workspace have different sizes
       * because it swaps pointers.
       * @param v The symm_workspace to swap with @c this.
       */
      void swap( symm_workspace& v ){
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
      gsl_eigen_symm_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_symm_workspace.
       * @return the gsl_eigen_symm_workspace
       */
      gsl_eigen_symm_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_symm_workspace.
       * @return @c true or @c falses according as 
       * this is the only symm_workspace object sharing the gsl_eigen_symm_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many symm_workspace objects share this pointer.
       * @return the number of symm_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_symm_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_symm().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param eval This is where the eigenvalues are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int symm( gsl::matrix& A, gsl::vector& eval, symm_workspace& w ){
      return gsl_eigen_symm( A.get(), eval.get(), w.get() ); }
    /**
     * Workspace for real symmetric matrices (eigenvectors and eigenvalues)
     */
    class symmv_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      symmv_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new symmv_workspace with n elements
       * @param n The number of elements in the symmv_workspace
       */
      explicit symmv_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_symmv_alloc( n );
	// just plausibly we could allocate symmv_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_symmv_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_symmv_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_symmv_workspace_free() to deallocate the memory.
       * @param v The symmv_workspace
       */
      explicit symmv_workspace( gsl_eigen_symmv_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The symmv_workspace to copy.
       */
      symmv_workspace( symmv_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The symmv_workspace to copy
       */
      symmv_workspace& operator=( symmv_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_symmv_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~symmv_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_symmv_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The symmv_workspace to move.
       */
      symmv_workspace( symmv_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The symmv_workspace to move.
       * @return A reference to this.
       */
      symmv_workspace& operator=( symmv_workspace&& v ){
	symmv_workspace( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two symmv_workspace are identically equal if their elements are identical.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( symmv_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two symmv_workspace are different equal if their elements are not identical.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( symmv_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symmv_workspace is nonnegative.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( symmv_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symmv_workspace is nonnegative.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( symmv_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symmv_workspace is nonnegative.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( symmv_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a symmv_workspace is nonnegative.
       * @param v The symmv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( symmv_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the symmv_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two symmv_workspace. This works even if the symmv_workspace have different sizes
       * because it swaps pointers.
       * @param v The symmv_workspace to swap with @c this.
       */
      void swap( symmv_workspace& v ){
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
      gsl_eigen_symmv_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_symmv_workspace.
       * @return the gsl_eigen_symmv_workspace
       */
      gsl_eigen_symmv_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_symmv_workspace.
       * @return @c true or @c falses according as 
       * this is the only symmv_workspace object sharing the gsl_eigen_symmv_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many symmv_workspace objects share this pointer.
       * @return the number of symmv_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_symmv_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_symmv().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param eval This is where the eigenvalues are stored
     * @param evec This is where the eigenvectors are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int symmv( gsl::matrix& A, gsl::vector& eval, gsl::matrix& evec, symmv_workspace& w ){
      return gsl_eigen_symmv( A.get(), eval.get(), evec.get(), w.get() ); }
  }
}

#endif
