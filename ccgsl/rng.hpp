/*
 * $Id: rng.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_RNG_HPP
#define CCGSL_RNG_HPP

#include<new>
#include<algorithm>
#include<gsl/gsl_rng.h>

namespace gsl {
  /**
   * Random number generator
   */
  class rng {
  public:
    /**
     * Define gsl::rng::static type.
     */
    typedef gsl_rng_type const* type;
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const borosh13(){ return gsl_rng_borosh13; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const coveyou(){ return gsl_rng_coveyou; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const cmrg(){ return gsl_rng_cmrg; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const fishman18(){ return gsl_rng_fishman18; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const fishman20(){ return gsl_rng_fishman20; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const fishman2x(){ return gsl_rng_fishman2x; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const gfsr4(){ return gsl_rng_gfsr4; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const knuthran(){ return gsl_rng_knuthran; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const knuthran2(){ return gsl_rng_knuthran2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const knuthran2002(){ return gsl_rng_knuthran2002; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const lecuyer21(){ return gsl_rng_lecuyer21; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const minstd(){ return gsl_rng_minstd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const mrg(){ return gsl_rng_mrg; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const mt19937(){ return gsl_rng_mt19937; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const mt19937_1999(){ return gsl_rng_mt19937_1999; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const mt19937_1998(){ return gsl_rng_mt19937_1998; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const r250(){ return gsl_rng_r250; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ran0(){ return gsl_rng_ran0; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ran1(){ return gsl_rng_ran1; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ran2(){ return gsl_rng_ran2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ran3(){ return gsl_rng_ran3; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const rand(){ return gsl_rng_rand; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const rand48(){ return gsl_rng_rand48; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random128_bsd(){ return gsl_rng_random128_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random128_glibc2(){ return gsl_rng_random128_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random128_libc5(){ return gsl_rng_random128_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random256_bsd(){ return gsl_rng_random256_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random256_glibc2(){ return gsl_rng_random256_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random256_libc5(){ return gsl_rng_random256_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random32_bsd(){ return gsl_rng_random32_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random32_glibc2(){ return gsl_rng_random32_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random32_libc5(){ return gsl_rng_random32_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random64_bsd(){ return gsl_rng_random64_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random64_glibc2(){ return gsl_rng_random64_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random64_libc5(){ return gsl_rng_random64_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random8_bsd(){ return gsl_rng_random8_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random8_glibc2(){ return gsl_rng_random8_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random8_libc5(){ return gsl_rng_random8_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random_bsd(){ return gsl_rng_random_bsd; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random_glibc2(){ return gsl_rng_random_glibc2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const random_libc5(){ return gsl_rng_random_libc5; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const randu(){ return gsl_rng_randu; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranf(){ return gsl_rng_ranf; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlux(){ return gsl_rng_ranlux; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlux389(){ return gsl_rng_ranlux389; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlxd1(){ return gsl_rng_ranlxd1; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlxd2(){ return gsl_rng_ranlxd2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlxs0(){ return gsl_rng_ranlxs0; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlxs1(){ return gsl_rng_ranlxs1; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranlxs2(){ return gsl_rng_ranlxs2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const ranmar(){ return gsl_rng_ranmar; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const slatec(){ return gsl_rng_slatec; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const taus(){ return gsl_rng_taus; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const taus2(){ return gsl_rng_taus2; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const taus113(){ return gsl_rng_taus113; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const transputer(){ return gsl_rng_transputer; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const tt800(){ return gsl_rng_tt800; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const uni(){ return gsl_rng_uni; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const uni32(){ return gsl_rng_uni32; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const vax(){ return gsl_rng_vax; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const waterman14(){ return gsl_rng_waterman14; }
    /**
     * Random number static type.
     * @return the named random number type
     */
    static type const zuf(){ return gsl_rng_zuf; }
    /**
     * Random number static type. This must be Defult because default is reserved.
     * @return the named random number type
     */
    static type const Default(){ return gsl_rng_default; }
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    rng(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    // Refines assignable
    /**
     * C++ version of gsl_rng_alloc().
     * @param T A random number generator type.
     */
    explicit rng( gsl_rng_type const* T ){
      ccgsl_pointer = gsl_rng_alloc( T );
      // just plausibly we could allocate mathieu_rng but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_rng_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_rng. This is not usually a good idea. In this case
     * you should not use gsl_rng() to deallocate the memory.
     * @param v The rng
     */
    explicit rng( gsl_rng* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the rng.
     * @param v The rng to copy.
     */
    rng( rng const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The rng to copy
     */
    rng& operator=( rng const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_rng_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~rng(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_rng_free( ccgsl_pointer );
	delete count;
      }
    }
    // Refines equality comparable
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The rng to move.
     */
    rng( rng&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The rng to move.
     * @return A reference to this.
     */
    rng& operator=( rng&& v ){
      rng( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // == operator
    /**
     * Two rng are identically equal if their elements are identical.
     * @param v The rng to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( rng const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two rng are different equal if their elements are not identical.
     * @param v The rng to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( rng const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for eaxmple, for checking, that a rng is nonnegative.
     * @param v The rng to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( rng const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a rng is nonnegative.
     * @param v The rng to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( rng const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a rng is nonnegative.
     * @param v The rng to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( rng const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for eaxmple, for checking, that a rng is nonnegative.
     * @param v The rng to be compared with @c this
     * @return @c false or @c true according as @c this is no
     * less than @c v lexicographically
     */
    bool operator>=( rng const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the rng is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two rng. This works even if the rng have different sizes
     * because it swaps pointers.
     * @param v The rng to swap with @c this.
     */
    void swap( rng& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_rng* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_rng.
     * @return the gsl_rng
     */
    gsl_rng* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_rng.
     * @return @c true or @c falses according as
     * this is the only rng object sharing the gsl_rng
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many rng objects share this pointer.
     * @return the number of rng objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_rng
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
    // Functions
    /**
     * C++ version of gsl_rng_types_setup().
     * @return Array of available types
     */
    static type* types_setup(){ return gsl_rng_types_setup(); }
    /**
     * C++ version of gsl_rng_memcpy().
     * @param src Source rng
     * @return Error code on failure
     */
     int memcpy( rng const& src ){ return gsl_rng_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_rng_clone().
     * @return A cloned copy of @c this
     */
     rng clone() const { return rng( gsl_rng_clone( get() ) ); }
    /**
     * C++ version of gsl_rng_set().
     * @param seed A seed for @c this
     */
     void set( unsigned long int seed ) const { gsl_rng_set( get(), seed ); }
    /**
     * C++ version of gsl_rng_max().
     * The random variate can take a number up to and including @c max.
     * @return The maximum integer value for @c this
     */
     unsigned long int max() const { return gsl_rng_max( get() ); }
    /**
     * C++ version of gsl_rng_min().
     * @return The maximum integer value for @c this
     */
     unsigned long int min() const { return gsl_rng_min( get() ); }
    /**
     * C++ version of gsl_rng_name().
     * @return The name of @c this
     */
     char const* name() const { return gsl_rng_name( get() ); }
    /**
     * C++ version of gsl_rng_fread().
     * @param stream A C file stream
     * @return Error code on failure
     */
     int fread( FILE* stream ){ return gsl_rng_fread( stream, get() ); }
    /**
     * C++ version of gsl_rng_fwrite().
     * @param stream A C file stream
     * @return Error code on failure
     */
     int fwrite( FILE* stream ) const { return gsl_rng_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_rng_size().
     * @return The size of the random number generator
     */
     size_t size() const { return gsl_rng_size( get() ); }
    /**
     * C++ version of gsl_rng_state().
     * @return The state of the random number generator
     */
     void* state() const { return gsl_rng_state( get() ); }
    /**
     * C++ version of gsl_rng_print_state().
     */
     void print_state() const { gsl_rng_print_state( get() ); }
    /**
     * C++ version of gsl_rng_env_setup().
     * @return The default type for a @c rng
     */
    static type env_setup(){ return gsl_rng_env_setup(); }
    /**
     * C++ version of gsl_rng_get().
     * @return The integer value of @c this
     */
     unsigned long int get_value() const { return gsl_rng_get( get() ); }
    /**
     * C++ version of gsl_rng_uniform().
     * @return A uniform random number in [0,1)
     */
     double uniform() const { return gsl_rng_uniform( get() ); }
    /**
     * C++ version of gsl_rng_uniform_pos().
     * @return A uniform random number in (0,1)
     */
     double uniform_pos() const { return gsl_rng_uniform_pos( get() ); }
    /**
     * C++ version of gsl_rng_uniform_int().
     * @param n A positive integer
     * @return A uniform integer in 0, ..., n - 1
     */
     unsigned long int uniform_int( unsigned long int n ) const {
      return gsl_rng_uniform_int( get(), n ); }
  };
}

#endif
