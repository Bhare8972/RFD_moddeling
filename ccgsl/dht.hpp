/*
 * $Id: dht.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_DHT_HPP
#define CCGSL_DHT_HPP

#include<new>
#include<gsl/gsl_dht.h>

namespace gsl {
  /**
   * Discrete Hankel Transforms
   */
  class dht {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    dht(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new dht with n elements.
     * @param size The number of elements in the dht
     */
    explicit dht( size_t const size ){
      ccgsl_pointer = gsl_dht_alloc( size );
      // just plausibly we could allocate dht but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_dht_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * C++ version of gsl_dht_new().
     * @param size The size
     * @param nu parameter (undocumented)
     * @param xmax parameter (undocumented)
     */
    dht( size_t size, double nu, double xmax ){
      ccgsl_pointer = gsl_dht_new( size, nu, xmax );
      // just plausibly we could allocate dht but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_dht_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }

    /**
     * Could construct from a gsl_dht.
     * This is not usually a good idea. In this case
     * we should not use gsl_dht_free() to deallocate the memory.
     * @param v The dht
     */
    explicit dht( gsl_dht* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The dht to copy.
     */
    dht( dht const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The dht to copy
     */
    dht& operator=( dht const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_dht_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~dht(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_dht_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The dht to move.
     */
    dht( dht&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The dht to move.
     * @return A reference to this.
     */
    dht& operator=( dht&& v ){
      dht( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two dht are identically equal if their elements are identical.
     * @param v The dht to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( dht const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two dht are different if their elements are not identical.
     * @param v The dht to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( dht const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a dht is nonnegative.
     * @param v The dht to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( dht const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a dht is nonnegative.
     * @param v The dht to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( dht const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a dht is nonnegative.
     * @param v The dht to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( dht const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a dht is nonnegative.
     * @param v The dht to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( dht const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the dht is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two dht objects. This works even if the dht have different sizes
     * because it swaps pointers.
     * @param v The dht to swap with @c this.
     */
    void swap( dht& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_dht* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_dht.
     * @return the gsl_dht
     */
    gsl_dht* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_dht.
     * @return @c true or @c falses according as 
     * this is the only dht object sharing the gsl_dht.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many dht objects share this pointer.
     * @return the number of dht objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_dht.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * C++ version of gsl_dht_init().
     * @param nu parameter (undocumented)
     * @param xmax parameter (undocumented)
     * @return Error code on failure
     */
    int init( double nu, double xmax ){ return gsl_dht_init( get(), nu, xmax ); }

    /**
     * C++ version of gsl_dht_x_sample().
     * @param n An integer (undocumented)
     * @return The nth sample point in the unit interval
     */
    double x_sample( int n ) const { return gsl_dht_x_sample( get(), n ); }

    /**
     * C++ version of gsl_dht_k_sample().
     * @param n An integer (undocumented)
     * @return The nth sample point in k-space
     */
    double k_sample( int n ) const { return gsl_dht_k_sample( get(), n ); }

    /**
     * C++ version of gsl_dht_apply().
     * @param f_in An array with size the same as the size of the transform (this)
     * @param f_out An array with size the same as the size of the transform (this)
     * [return value]
     * @return Error code on failure
     */
    int apply( double* f_in, double* f_out ) const {
      return gsl_dht_apply( get(), f_in, f_out ); }
    
    /**
     * C++ version of gsl_dht_apply().
     * This version handles std::vector and gsl::vector.
     * @param f_in An array with size the same as the size of the transform (this)
     * @param f_out An array with size the same as the size of the transform (this)
     * [return value]
     * @return Error code on failure
     */
    template<typename ARRAY>
    int apply( ARRAY& f_in, ARRAY& f_out ) const {
      return gsl_dht_apply( get(), f_in.data(), f_out.data() ); }

  };
}
#endif
