/*
 * $Id: qrng.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_QRNG_HPP
#define CCGSL_QRNG_HPP

#include<new>
#include<gsl/gsl_qrng.h>
#include"exception.hpp"

namespace gsl {
  /**
   * Quasi random sequences
   */
  class qrng {
  public:
    /**
     * Convenient typedef
     */
    typedef gsl_qrng_type type;
    /**
     * The default constructor is only really useful for assigning to.
     */
    qrng(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new qrng with n elements.
     * @param T the type
     * @param dimension the dimension of the space
     */
    explicit qrng( type const* T, unsigned int const dimension ){
      ccgsl_pointer = gsl_qrng_alloc( T, dimension );
      // just plausibly we could allocate qrng but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_qrng_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_qrng.
     * This is not usually a good idea. In this case
     * we should not use gsl_qrng_free() to deallocate the memory.
     * @param v The qrng
     */
    explicit qrng( gsl_qrng* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The qrng to copy.
     */
    qrng( qrng const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The qrng to copy
     */
    qrng& operator=( qrng const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_qrng_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~qrng(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_qrng_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The qrng to move.
     */
    qrng( qrng&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The qrng to move.
     * @return A reference to this.
     */
    qrng& operator=( qrng&& v ){
      qrng( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two qrng are identically equal if their elements are identical.
     * @param v The qrng to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( qrng const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two qrng are different if their elements are not identical.
     * @param v The qrng to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( qrng const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a qrng is nonnegative.
     * @param v The qrng to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( qrng const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a qrng is nonnegative.
     * @param v The qrng to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( qrng const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a qrng is nonnegative.
     * @param v The qrng to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( qrng const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a qrng is nonnegative.
     * @param v The qrng to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( qrng const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the qrng is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two qrng objects. This works even if the qrng have different sizes
     * because it swaps pointers.
     * @param v The qrng to swap with @c this.
     */
    void swap( qrng& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_qrng* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_qrng.
     * @return the gsl_qrng
     */
    gsl_qrng* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_qrng.
     * @return @c true or @c falses according as 
     * this is the only qrng object sharing the gsl_qrng.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many qrng objects share this pointer.
     * @return the number of qrng objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_qrng.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * C++ version of gsl_qrng_memcpy().
     * @param src Source qrng
     * @return Error code on failure
     */
    int memcpy( qrng const& src ){ return gsl_qrng_memcpy( get(), src.get() ); }

    /**
     * C++ version of gsl_qrng_clone().
     * @return A cloned copy of @c this
     */
    qrng clone() const { return qrng( gsl_qrng_clone( get() ) ); }

    /**
     * C++ version of gsl_qrng_init().
     */
    void init(){ gsl_qrng_init( get() ); }

    /**
     * C++ version of gsl_qrng_name().
     * @return The name of the qrng type
     */
    char const* name() const { return gsl_qrng_name( get() ); }

    /**
     * C++ version of gsl_qrng_size().
     * @return The size of the qrng
     */
    size_t size() const { return gsl_qrng_size( get() ); }

    /**
     * C++ version of gsl_qrng_state().
     * @return The state of the qrng
     */
    void* state() const { return gsl_qrng_state( get() ); }

    /**
     * C++ version of gsl_qrng_get().
     * @param x[] An array of size dimension to hold as return value the next point
     * @return Error code on failure
     */
    int get( double x[] ) const { return gsl_qrng_get( get(), x ); }

    /**
     * C++ version of gsl_qrng_get().
     * This version works with gsl::vector and std::vector<double> arrays
     * @param x An array of size dimension to hold as return value the next point
     * @return Error code on failure
     */
    template<typename X>
    int get( X& x ) const {
      return gsl_qrng_get( get(), x.data() );
    }
    // Types
    /**
     * Quasi random number sequence static type.
     * @return the named quasi random sequence type
     */
    static type const* niederreiter_2(){ return gsl_qrng_niederreiter_2; }
    /**
     * Quasi random number sequence static type.
     * @return the named quasi random sequence type
     */
    static type const* sobol(){ return gsl_qrng_sobol; }
    /**
     * Quasi random number sequence static type.
     * @return the named quasi random sequence type
     */
    static type const* halton(){ return gsl_qrng_halton; }
    /**
     * Quasi random number sequence static type.
     * @return the named quasi random sequence type
     */
    static type const* reversehalton(){ return gsl_qrng_reversehalton; }
    
  };
}
#endif
