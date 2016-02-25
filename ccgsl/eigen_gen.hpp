/*
 * $Id: eigen_gen.hpp 125 2012-02-19 18:18:25Z jdl3 $
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

#ifndef CCGSL_EIGEN_GEN_HPP
#define CCGSL_EIGEN_GEN_HPP

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
     * Workspace for eigen_gen
     */
    class gen_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      gen_workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new gen_workspace with n elements
       * @param n The number of elements in the gen_workspace
       */
      explicit gen_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_gen_alloc( n );
	// just plausibly we could allocate gen_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_gen_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_gen_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_gen_workspace_free() to deallocate the memory.
       * @param v The gen_workspace
       */
      explicit gen_workspace( gsl_eigen_gen_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The gen_workspace to copy.
       */
      gen_workspace( gen_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The gen_workspace to copy
       */
      gen_workspace& operator=( gen_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_gen_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~gen_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_gen_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The gen_workspace to move.
	 */
	gen_workspace( gen_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The gen_workspace to move.
	 * @return A reference to this.
	 */
	gen_workspace& operator=( gen_workspace&& v ){
	  gen_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two gen_workspace are identically equal if their elements are identical.
       * @param v The gen_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( gen_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two gen_workspace are different equal if their elements are not identical.
       * @param v The gen_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( gen_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gen_workspace is nonnegative.
       * @param v The gen_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( gen_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gen_workspace is nonnegative.
       * @param v The gen_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( gen_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gen_workspace is nonnegative.
       * @param v The gen_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( gen_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a gen_workspace is nonnegative.
       * @param v The gen_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( gen_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the gen_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two gen_workspace. This works even if the gen_workspace have different sizes
       * because it swaps pointers.
       * @param v The gen_workspace to swap with @c this.
       */
      void swap( gen_workspace& v ){
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
      gsl_eigen_gen_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_gen_workspace.
       * @return the gsl_eigen_gen_workspace
       */
      gsl_eigen_gen_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_gen_workspace.
       * @return @c true or @c falses according as 
       * this is the only gen_workspace object sharing the gsl_eigen_gen_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many gen_workspace objects share this pointer.
       * @return the number of gen_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_gen_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_gen().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param alpha This is where the eigenvalues are stored
     * @param beta This is where the eigenvalues are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int gen( gsl::matrix& A, gsl::matrix& B, gsl::vector_complex& alpha, gsl::vector& beta,
		    gen_workspace& w ){ return gsl_eigen_gen( A.get(), B.get(), alpha.get(), beta.get(), w.get() ); }
    /**
     * Workspace for real genetric matrices (eigenvectors and eigenvalues)
     */
    class genv_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      genv_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new genv_workspace with n elements
       * @param n The number of elements in the genv_workspace
       */
      explicit genv_workspace( size_t const n ){
	ccgsl_pointer = gsl_eigen_genv_alloc( n );
	// just plausibly we could allocate genv_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_eigen_genv_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_eigen_genv_workspace. This is not usually a good idea. In this case
       * you should not use gsl_eigen_genv_workspace_free() to deallocate the memory.
       * @param v The genv_workspace
       */
      explicit genv_workspace( gsl_eigen_genv_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The genv_workspace to copy.
       */
      genv_workspace( genv_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The genv_workspace to copy
       */
      genv_workspace& operator=( genv_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_eigen_genv_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~genv_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_eigen_genv_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The genv_workspace to move.
       */
      genv_workspace( genv_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The genv_workspace to move.
       * @return A reference to this.
       */
      genv_workspace& operator=( genv_workspace&& v ){
	genv_workspace( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two genv_workspace are identically equal if their elements are identical.
       * @param v The genv_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( genv_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two genv_workspace are different equal if their elements are not identical.
       * @param v The genv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( genv_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a genv_workspace is nonnegative.
       * @param v The genv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( genv_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a genv_workspace is nonnegative.
       * @param v The genv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( genv_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a genv_workspace is nonnegative.
       * @param v The genv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( genv_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a genv_workspace is nonnegative.
       * @param v The genv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( genv_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the genv_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two genv_workspace. This works even if the genv_workspace have different sizes
       * because it swaps pointers.
       * @param v The genv_workspace to swap with @c this.
       */
      void swap( genv_workspace& v ){
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
      gsl_eigen_genv_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_eigen_genv_workspace.
       * @return the gsl_eigen_genv_workspace
       */
      gsl_eigen_genv_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_eigen_genv_workspace.
       * @return @c true or @c false according as 
       * this is the only genv_workspace object sharing the gsl_eigen_genv_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many genv_workspace objects share this pointer.
       * @return the number of genv_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_eigen_genv_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    /**
     * C++ version of gsl_eigen_genv().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param alpha This is where the eigenvalues are stored
     * @param beta This is where the eigenvalues are stored
     * @param evec This is where the eigenvectors are stored
     * @param w A workspace
     * @return Error code on failure
     */
    inline int genv( gsl::matrix& A, gsl::matrix& B, gsl::vector_complex& alpha,
		     gsl::vector& beta, gsl::matrix_complex& evec, genv_workspace& w ){
      return gsl_eigen_genv( A.get(), B.get(), alpha.get(), beta.get(), evec.get(), w.get() ); }
    /**
     * C++ version of gsl_eigen_gen_params().
     * @param compute_s An integer
     * @param compute_t An integer
     * @param balance An integer
     * @param w A workspace
     */
    inline void gen_params( int const compute_s, int const compute_t, int const balance,
			    gen_workspace& w ){
      gsl_eigen_gen_params( compute_s, compute_t, balance, w.get() ); }
    /**
     * C++ version of gsl_eigen_gen_QZ().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param alpha This is where the eigenvalues are stored
     * @param beta This is where the eigenvalues are stored
     * @param Q A matrix (should be square)
     * @param Z A matrix (should be square)
     * @param w A workspace
     * @return Error code on failure
     */
    inline int gen_QZ( gsl::matrix& A, gsl::matrix& B, gsl::vector_complex& alpha,
		       gsl::vector& beta, gsl::matrix& Q, gsl::matrix& Z, gen_workspace& w ){
      return gsl_eigen_gen_QZ( A.get(), B.get(), alpha.get(), beta.get(), Q.get(), Z.get(), w.get() ); }
    /**
     * C++ version of gsl_eigen_genv_QZ().
     * Computes the eigenvalues of A and stores them (unordered) in eval.
     * The diagonal and lower triangle of A are altered. The workspace should have size
     * @c n, where @c A has @c n rows and columns.
     * @param A A matrix (should be square)
     * @param B A matrix (should be square)
     * @param alpha This is where the eigenvalues are stored
     * @param beta This is where the eigenvalues are stored
     * @param evec This is where the eigenvectors are stored
     * @param Q A matrix (should be square)
     * @param Z A matrix (should be square)
     * @param w A workspace
     * @return Error code on failure
     */
    inline int genv_QZ( gsl::matrix& A, gsl::matrix& B, gsl::vector_complex& alpha,
			gsl::vector& beta, gsl::matrix_complex& evec,
			gsl::matrix& Q, gsl::matrix& Z, genv_workspace& w ){ return
	gsl_eigen_genv_QZ( A.get(), B.get(), alpha.get(), beta.get(), evec.get(), Q.get(), Z.get(), w.get() ); }
    }
}

#endif
