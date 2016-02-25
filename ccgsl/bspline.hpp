/*
 * $Id: bspline.hpp 257 2012-08-22 16:14:53Z jdl3 $
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

#ifndef CCGSL_BSPLINE_HPP
#define CCGSL_BSPLINE_HPP

#include<gsl/gsl_bspline.h>
#include"vector.hpp"
#include"matrix.hpp"

namespace gsl {
  /**
   * Basis splines.
   */
  namespace bspline {
    class workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new workspace of order k for data with nbreak
       * breakpoints
       * @param k The order of the basis spline
       * @param nbreak the number of breakpoints
       */
      explicit workspace( size_t const k, size_t const nbreak ){
	ccgsl_pointer = gsl_bspline_alloc( k, nbreak );
	// just plausibly we could allocate bspline but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_bspline_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_bspline_workspace.
       * This is not usually a good idea. In this case
       * we should not use gsl_bspline_free() to deallocate the memory.
       * @param v The gsl_bspline_workspace
       */
      explicit workspace( gsl_bspline_workspace* v ){
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
	  if( ccgsl_pointer != 0 ) gsl_bspline_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_bspline_free( ccgsl_pointer );
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
       * Two workspace are different if their elements are not identical.
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
       * for example, for checking, that a workspace is nonnegative.
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
       * for example, for checking, that a workspace is nonnegative.
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
       * Swap two workspace objects. This works even if the workspace have different sizes
       * because it swaps pointers.
       * @param v The workspace to swap with @c this.
       */
      void swap( workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_bspline_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_bspline_workspace.
       * @return the gsl_bspline
       */
      gsl_bspline_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_bspline.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_bspline.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_bspline_workspace.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
      /**
       * Get the knots vector. Do not confuse this with bspline::knots() which computes the
       * knots.
       * @return The knots vector
       */
      gsl::vector knots(){
	gsl::vector return_value;
	return_value.wrap_gsl_vector_without_ownership( ccgsl_pointer->knots );
	return return_value;
      }
    };

    /**
     * C++ version of gsl_bspline_ncoeffs().
     * @param w A gsl::bspline::workspace
     * @return The number of B-spline coefficients
     */
    inline size_t ncoeffs( workspace& w ){ return gsl_bspline_ncoeffs( w.get() ); }

    /**
     * C++ version of gsl_bspline_order().
     * @param w A gsl::bspline::workspace
     * @return The order of the B-spline
     */
    inline size_t order( workspace& w ){ return gsl_bspline_order( w.get() ); }

    /**
     * C++ version of gsl_bspline_nbreak().
     * @param w A gsl::bspline::workspace
     * @return The number of breaks
     */
    inline size_t nbreak( workspace& w ){ return gsl_bspline_nbreak( w.get() ); }

    /**
     * C++ version of gsl_bspline_breakpoint().
     * @param i An index
     * @param w A gsl::bspline::workspace
     * @return The ith breakpoint
     */
    inline double breakpoint( size_t i, workspace& w ){
      return gsl_bspline_breakpoint( i, w.get() ); }

    /**
     * C++ version of gsl_bspline_greville_abscissa().
     * @param i An index
     * @param w A gsl::bspline::workspace
     * @return The ith Greville abscissa for the given spline basis
     */
    inline double greville_abscissa( size_t i, workspace& w ){
      return gsl_bspline_greville_abscissa( i, w.get() ); }

    /**
     * C++ version of gsl_bspline_knots().
     * @param breakpts A vector of breakpoints
     * @param w A gsl::bspline::workspace
     * @return Error code on failure
     */
    inline int knots( vector const& breakpts, workspace& w ){
      return gsl_bspline_knots( breakpts.get(), w.get() ); }

    /**
     * C++ version of gsl_bspline_knots_uniform().
     * @param a Lower end of range
     * @param b Upper end of range
     * @param w A gsl::bspline::workspace
     * @return Error code on failure
     */
    inline int knots_uniform( double const a, double const b, workspace& w ){
      return gsl_bspline_knots_uniform( a, b, w.get() ); }

    /**
     * C++ version of gsl_bspline_eval().
     * @param x A position
     * @param B A vector to hold \f$B_1(x),\ldots,B_n(x)\f$, where \(n\) can be found
     * usning ncoeffs()
     * @param w A gsl::bspline::workspace
     * @return Error code on failure
     */
    inline int eval( double const x, vector& B, workspace& w ){
      return gsl_bspline_eval( x, B.get(), w.get() ); }

    /**
     * C++ version of gsl_bspline_eval_nonzero().
     * @param x A position
     * @param Bk A vector of lenth k
     * @param istart Index of first nonzero element
     * @param iend Index of last nonzero element
     * @param w A gsl::bspline::workspace
     * @return Error code on failure
     */
    inline int eval_nonzero( double const x, vector& Bk, size_t* istart, size_t* iend, workspace& w ){
      return gsl_bspline_eval_nonzero( x, Bk.get(), istart, iend, w.get() ); }

    
    /**
     * Workspace for computing the derivatives of a B-spline basis function of order k
     */
    class deriv_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      deriv_workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new deriv_workspace or order k
       * @param k The order of the B-spline
       */
      explicit deriv_workspace( size_t const k ){
	ccgsl_pointer = gsl_bspline_deriv_alloc( k );
	// just plausibly we could allocate deriv_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_bspline_deriv_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_bspline_deriv_workspace.
       * This is not usually a good idea. In this case
       * we should not use gsl_bspline_deriv_free() to deallocate the memory.
       * @param v The deriv_workspace
       */
      explicit deriv_workspace( gsl_bspline_deriv_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The deriv_workspace to copy.
       */
      deriv_workspace( deriv_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The deriv_workspace to copy
       */
      deriv_workspace& operator=( deriv_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_bspline_deriv_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~deriv_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_bspline_deriv_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The deriv_workspace to move.
	 */
	deriv_workspace( deriv_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The deriv_workspace to move.
	 * @return A reference to this.
	 */
	deriv_workspace& operator=( deriv_workspace&& v ){
	  deriv_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two deriv_workspace are identically equal if their elements are identical.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( deriv_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two deriv_workspace are different if their elements are not identical.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( deriv_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a deriv_workspace is nonnegative.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( deriv_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a deriv_workspace is nonnegative.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( deriv_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a deriv_workspace is nonnegative.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( deriv_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a deriv_workspace is nonnegative.
       * @param v The deriv_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( deriv_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the deriv_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two deriv_workspace objects. This works even if the deriv_workspace have different sizes
       * because it swaps pointers.
       * @param v The deriv_workspace to swap with @c this.
       */
      void swap( deriv_workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_bspline_deriv_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_bspline_deriv_workspace.
       * @return the gsl_bspline_deriv_workspace
       */
      gsl_bspline_deriv_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_bspline_deriv_workspace.
       * @return @c true or @c falses according as 
       * this is the only deriv_workspace object sharing the gsl_bspline_deriv_workspace.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many deriv_workspace objects share this pointer.
       * @return the number of deriv_workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_bspline_deriv_workspace.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    
    /**
     * C++ version of gsl_bspline_deriv_eval().
     * @param x A position
     * @param nderiv Highest derivative to compute for
     * @param dB Matrix to hold derivatives 
     * @param w A gsl::bspline::workspace
     * @param dw A gsl::bspline::deriv_workspace
     * @return Error code on failure
     */
    inline int deriv_eval( double const x, size_t const nderiv, matrix& dB,
			   workspace& w, deriv_workspace& dw ){
      return gsl_bspline_deriv_eval( x, nderiv, dB.get(), w.get(), dw.get() ); }

    /**
     * C++ version of gsl_bspline_deriv_eval_nonzero().
     * @param x A position
     * @param nderiv Highest derivative to compute for
     * @param dB Matrix to hold derivatives
     * @param istart Index of first nonzero element
     * @param iend Index of last nonzero element
     * @param w A gsl::bspline::workspace
     * @param dw A gsl::bspline::deriv_workspace
     * @return Error code on failure
     */
    inline int deriv_eval_nonzero( double const x, size_t const nderiv, matrix& dB, size_t* istart,
				   size_t* iend, workspace& w, deriv_workspace& dw ){
      return gsl_bspline_deriv_eval_nonzero( x, nderiv, dB.get(), istart, iend, w.get(),
					     dw.get() ); }
    
  }
}
#endif
