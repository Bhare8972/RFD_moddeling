/*
 * $Id: sum.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_SUM_HPP
#define CCGSL_SUM_HPP

#include<new>
#include<gsl/gsl_sum.h>

namespace gsl {
  /**
   * Series acceleration.
   */
  namespace sum {
    /**
     * The Levin u transform.
     */
    namespace levin_u {
      /**
       * Workspace for the Levin u transform.
       */
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
	 * The default constructor creates a new workspace with n elements.
	 * @param n The size of the workspace
	 */
	explicit workspace( size_t const n ){
	  ccgsl_pointer = gsl_sum_levin_u_alloc( n );
	  // just plausibly we could allocate workspace but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_sum_levin_u_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_sum_levin_u_workspace.
	 * This is not usually a good idea. In this case
	 * we should not use gsl_sum_levin_u_free() to deallocate the memory.
	 * @param v The workspace
	 */
	explicit workspace( gsl_sum_levin_u_workspace* v ){
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
	    if( ccgsl_pointer != 0 ) gsl_sum_levin_u_free( ccgsl_pointer );
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
	    if( ccgsl_pointer != 0 ) gsl_sum_levin_u_free( ccgsl_pointer );
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
	gsl_sum_levin_u_workspace* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_sum_levin_u_workspace.
	 * @return the gsl_sum_levin_u_workspace
	 */
	gsl_sum_levin_u_workspace* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_sum_levin_u_workspace.
	 * @return @c true or @c falses according as 
	 * this is the only workspace object sharing the gsl_sum_levin_u_workspace.
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
	 * to a gsl_sum_levin_u_workspace.
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t size() const { return ccgsl_pointer->size; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t i() const { return ccgsl_pointer->i; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t terms_used() const { return ccgsl_pointer->terms_used; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	double sum_plain() const { return ccgsl_pointer->sum_plain; }
      };
      /**
       * C++ version of gsl_sum_levin_u_accel().
       * @param array A series of terms
       * @param array_size The size of the series
       * @param w A workspace of size array_size
       * @param sum_accel The extrapolated sum
       * @param abserr Absolute error
       * @return Error code on failure
       */
      inline int accel( const double* array, size_t array_size, workspace& w, double* sum_accel,
			double* abserr ){
	return gsl_sum_levin_u_accel( array, array_size, w.get(), sum_accel, abserr );
      }
      /**
       * C++ version of gsl_sum_levin_u_accel().
       * This version handles std::vector and gsl::vector.
       * @param array A series of terms
       * @param w A workspace of size array_size
       * @param sum_accel The extrapolated sum
       * @param abserr Absolute error
       * @return Error code on failure
       */
      template<typename ARRAY>
      inline int accel( ARRAY const& array, workspace& w, double* sum_accel,
			double* abserr ){
	return gsl_sum_levin_u_accel( array.data(), array.size(), w.get(), sum_accel, abserr );
      }
    }
    /**
     * The Levin u transform without error estimation.
     */
    namespace levin_utrunc {
      /**
       * Workspace for the Levin u transform without error estimation.
       */
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
	 * The default constructor creates a new workspace with n elements.
	 * @param n The size of the workspace
	 */
	explicit workspace( size_t const n ){
	  ccgsl_pointer = gsl_sum_levin_utrunc_alloc( n );
	  // just plausibly we could allocate workspace but not count
	  try { count = new size_t; } catch( std::bad_alloc& e ){
	    // try to tidy up before rethrowing
	    gsl_sum_levin_utrunc_free( ccgsl_pointer );
	    throw e;
	  }
	  *count = 1; // initially there is just one reference to ccgsl_pointer
	}
	/**
	 * Could construct from a gsl_sum_levin_utrunc_workspace.
	 * This is not usually a good idea. In this case
	 * we should not use gsl_sum_levin_utrunc_free() to deallocate the memory.
	 * @param v The workspace
	 */
	explicit workspace( gsl_sum_levin_utrunc_workspace* v ){
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
	    if( ccgsl_pointer != 0 ) gsl_sum_levin_utrunc_free( ccgsl_pointer );
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
	    if( ccgsl_pointer != 0 ) gsl_sum_levin_utrunc_free( ccgsl_pointer );
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
	gsl_sum_levin_utrunc_workspace* ccgsl_pointer;
	/**
	 * The shared reference count
	 */
	size_t* count;
      public:
	// shared reference functions
	/**
	 * Get the gsl_sum_levin_utrunc_workspace.
	 * @return the gsl_sum_levin_utrunc_workspace
	 */
	gsl_sum_levin_utrunc_workspace* get() const { return ccgsl_pointer; }
	/**
	 * Find if this is the only object sharing the gsl_sum_levin_utrunc_workspace.
	 * @return @c true or @c falses according as 
	 * this is the only workspace object sharing the gsl_sum_levin_utrunc_workspace.
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
	 * to a gsl_sum_levin_utrunc_workspace.
	 */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	explicit
#endif
	operator bool() const { return ccgsl_pointer != 0; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t size() const { return ccgsl_pointer->size; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t i() const { return ccgsl_pointer->i; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	size_t terms_used() const { return ccgsl_pointer->terms_used; }
	/**
	 * Give access to underlying struct value
	 * @return value from workspace
	 */
	double sum_plain() const { return ccgsl_pointer->sum_plain; }
      };
      /**
       * C++ version of gsl_sum_levin_utrunc_accel().
       * @param array A series of terms
       * @param array_size The size of the series
       * @param w A workspace of size array_size
       * @param sum_accel The extrapolated sum
       * @param abserr_trunc Absolute error
       * @return Error code on failure
       */
      inline int accel( const double* array, size_t array_size, workspace& w, double* sum_accel,
			double* abserr_trunc ){
	return gsl_sum_levin_utrunc_accel( array, array_size, w.get(), sum_accel, abserr_trunc );
      }
      /**
       * C++ version of gsl_sum_levin_utrunc_accel().
       * This version handles std::vector and gsl::vector.
       * @param array A series of terms
       * @param w A workspace of size array_size
       * @param sum_accel The extrapolated sum
       * @param abserr_trunc Absolute error
       * @return Error code on failure
       */
      template<typename ARRAY>
      inline int accel( ARRAY const& array, workspace& w, double* sum_accel, double* abserr_trunc ){
	return gsl_sum_levin_utrunc_accel( array.data(), array.size(), w.get(), sum_accel, abserr_trunc );
      }
    }
  }
}
#endif
