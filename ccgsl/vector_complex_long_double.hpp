/*
 * $Id: vector_complex_long_double.hpp 314 2014-02-22 14:31:16Z jdl3 $
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

#ifndef CCGSL_VECTOR_COMPLEX_LONG_DOUBLE_HPP
#define CCGSL_VECTOR_COMPLEX_LONG_DOUBLE_HPP

#include<gsl/gsl_vector_complex_long_double.h>
#include<new>
#include<iterator>
#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include<complex>
#endif

#include"exception.hpp"
#include"block_complex_long_double.hpp"
#include"vector_long_double.hpp"

// This file is a template
#define CCGSL_MTY 2

namespace gsl {
  // declare matrix_complex_long_double class
  class matrix_complex_long_double;
  /**
   * This class handles vector_complex_long_double objects as shared handles. It models a random access container
   * so that STL functions work with vector_complex_long_double.
   *
   * Note that vector_complex_long_double_views are implemented as vector_complex_long_double objects here.
   */
  class vector_complex_long_double {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    vector_complex_long_double(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new vector_complex_long_double with n elements
     * @param n The number of elements in the vector_complex_long_double
     */
    explicit vector_complex_long_double( size_t const n ){
      if( n > 0 ) ccgsl_pointer = gsl_vector_complex_long_double_alloc( n );
      else { ccgsl_pointer = new gsl_vector_complex_long_double; ccgsl_pointer->size = 0; ccgsl_pointer->data = 0; }
      // just plausibly we could allocate vector_complex_long_double but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n > 0 ) gsl_vector_complex_long_double_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_vector_complex_long_double. This is not usually a good idea. In this case
     * you should not use gsl_vector_complex_long_double_free() to deallocate the memory.
     * @param v The vector_complex_long_double
     */
    explicit vector_complex_long_double( gsl_vector_complex_long_double* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Could construct from a std::initializer_list in C++11
     * @param initializer_list The initializer_list.
     */
    vector_complex_long_double( std::initializer_list<std::complex<long double> > initializer_list ){
      size_t const n = initializer_list.size();
      ccgsl_pointer = gsl_vector_complex_long_double_alloc( n );
      // just plausibly we could allocate vector but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n > 0 ) gsl_vector_complex_long_double_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
      // now copy
      auto p = begin();
      for( auto x : initializer_list ){
	*p = complex_long_double( std::move( x ) ); ++p;
      }
    }
#endif
    // copy constructor
    /**
     * The copy constructor. This shares the vector_complex_long_double. Use clone() if you want a full copy.
     * @param v The vector_complex_long_double to copy.
     */
    vector_complex_long_double( vector_complex_long_double const& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( v.count ){
      if( count != 0 ) ++*count; // vector_complex_long_double is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The vector_complex_long_double to copy
     */
    vector_complex_long_double& operator=( vector_complex_long_double const& v ){
      // first, possibly delete anything pointed to by @c this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ){
	  if( ccgsl_pointer->size > 0 ) gsl_vector_complex_long_double_free( ccgsl_pointer );
	  else delete ccgsl_pointer; }
	delete count;
      }
      // Then copy
      ccgsl_pointer = v.ccgsl_pointer;
      count = v.count;
      if( count != 0 ) ++*count; // block_complex_long_double is now shared.
      return *this;
    }
    // clone()
    /**
     * The clone function. Use this if you want a copy of the block_complex_long_double that does
     * not share the underlying data.
     * @return a new copy of @c this.
     */
    vector_complex_long_double clone() const {
      vector_complex_long_double copy( get()->size );
      // Now copy
      gsl_vector_complex_long_double_memcpy( copy.get(), get() );
      // return new object
      return copy;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~vector_complex_long_double(){
      if( --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ){
	  if( ccgsl_pointer->size > 0 ) gsl_vector_complex_long_double_free( ccgsl_pointer );
	  else delete ccgsl_pointer; }
	delete count;
      }
    }
    // Refines equality comparable
    // == operator
    /**
     * Two vector_complex_long_double objects are identically equal if their elements are identical.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( vector_complex_long_double const& v ) const {
      // trivially equal if gsl_vector_complex_long_double*s are identical
      if( ccgsl_pointer == v.ccgsl_pointer ) return true;
      // trivially not equal if one is zero: != should be same as xor here
      if( (ccgsl_pointer == 0) != (v.ccgsl_pointer == 0 ) ) return false;
      // trivially not equal if sizes are different
      if( ccgsl_pointer->size != v.ccgsl_pointer->size ) return false;
      // check elementwise for equality
      for( size_t i = 0; i < CCGSL_MTY * ccgsl_pointer->size; ++i )
  	if( *(ccgsl_pointer->data + i) != *(v.ccgsl_pointer->data + i) )
	  return false; 
      return true;
    }
    /**
     * Stop sharing ownership of the shared pointer.
     */
    void reset(){ vector_complex_long_double().swap( *this ); }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The vector_complex_long_double to move.
     */
    vector_complex_long_double( vector_complex_long_double&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The vector_complex_long_double to move.
     * @return A reference to this.
     */
    vector_complex_long_double& operator=( vector_complex_long_double&& v ){
      vector_complex_long_double( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // != operator
    /**
     * Two vector_complex_long_double objects are different equal if their elements are not identical.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( vector_complex_long_double const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a vector_complex_long_double is nonnegative.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( vector_complex_long_double const& v ) const {
      // null vector_complex_long_double comes first
      if( ccgsl_pointer == 0 ) return v.ccgsl_pointer != 0;
      // if v is null then this > v
      if( v.ccgsl_pointer == 0 ) return false;
      // Now compare elementwise
      size_t const  size = ccgsl_pointer->size;
      size_t const  v_size = v.ccgsl_pointer->size;
      size_t const min = size > v_size ? size : v_size;
      for( size_t i = 0; i < min; ++i ){
	complex_long_double const t = gsl_vector_complex_long_double_get( ccgsl_pointer, i );
	complex_long_double const u =gsl_vector_complex_long_double_get( v.ccgsl_pointer, i );
	if( t < u ) return true;
	if( u < t ) return false;
      }
      // elements match.
      return size < v_size;
    }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a vector_complex_long_double is nonnegative.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( vector_complex_long_double const& v ) const {
      // null vector_complex_long_double comes first
      if( ccgsl_pointer == 0 ) return false;
      // if v is null then this > v
      if( v.ccgsl_pointer == 0 ) return true;
      // Now compare elementwise
      size_t const  size = ccgsl_pointer->size;
      size_t const  v_size = v.ccgsl_pointer->size;
      size_t const min = size > v_size ? size : v_size;
      for( size_t i = 0; i < min; ++i ){
	complex_long_double const t = gsl_vector_complex_long_double_get( ccgsl_pointer, i );
	complex_long_double const u =gsl_vector_complex_long_double_get( v.ccgsl_pointer, i );
	if( t > u ) return true;
	if( u > t ) return false;
      }
      // elements match.
      return size > v_size;
    }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a vector_complex_long_double is nonnegative.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( vector_complex_long_double const& v ) const {
      return operator<( v ) or operator==( v );
    }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a vector_complex_long_double is nonnegative.
     * @param v The vector_complex_long_double to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */ 
    bool operator>=( vector_complex_long_double const& v ) const {
      return operator>( v ) or operator==( v );
    }
    // Refines container
    // type value_type
    /**
     * A container must have a value_type.
     */
    typedef complex_long_double value_type;
    // type reference
    /**
     * A container must have a reference type.
     */
    typedef  complex_long_double_ref reference;
    // type const_reference
    /**
     * A container must have a constant reference type.
     */
    typedef  reference const const_reference;
    // type pointer
    /**
     * A container must have a pointer type.
     */
    typedef  complex_long_double_ptr pointer;
    // type const_pointer
    /**
     * A container must have a constant pointer type.
     */
    typedef  complex_long_double_ptr const const_pointer;
    // type iterator
  private:
    /**
     * The container must have iterator types. We create a suitable class
     * here.
     */
    template<typename container, typename content,bool reverse_t>
    class iterator_base {
      friend class vector_complex_long_double;
    public:
      /**
       * An iterator must have a value_type.
       */
      typedef complex_long_double value_type;
      /**
       * An iterator must have a reference type.
       */
      typedef complex_long_double_ref reference;
      /**
       * An iterator must have a pointer type.
       */
      typedef complex_long_double_ptr pointer;
      /**
       * An iterator must have a pointer type.
       */
      typedef std::random_access_iterator_tag iterator_category;
      // // type iterator_traits<block_complex_long_double>::difference_type
      /**
       * An iterator must have a difference_type.
       */
      typedef ptrdiff_t difference_type;
    public:
      // // operator*
      /**
       * Dereference the pointer.
       * @return a reference to content
       */
      reference operator*() const {
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return complex_long_double_ref( 0 );
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return complex_long_double_ref( 0 );
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference beyond rbegin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return complex_long_double_ref( 0 );
  	}
  	if( position <= -1 ){
  	  gsl_error( "trying to dereference beyond begin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return complex_long_double_ref( 0 );
  	}
  	// position and v are valid: return data
  	return complex_long_double_ref( v->ccgsl_pointer->data + CCGSL_MTY * position * v->ccgsl_pointer->stride );
      }
      // // operator->
      /**
       * Dereference the pointer.
       * @return a pointer to content
       */
      pointer operator->() const {
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return pointer( 0 );
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return pointer( 0 );
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference end()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return pointer( 0 );
  	}
  	if( position <= -1 ){
  	  gsl_error( "trying to dereference rend()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return pointer( 0 );
  	}
  	// position and v are valid: return data
  	return pointer( v->ccgsl_pointer->data + CCGSL_MTY * position * v->ccgsl_pointer->stride );
      }
      // // operator[]
      /**
       * Get element at i + n by reference ([] operator).
       * @param n The offset from i
       * @return a reference to content
       */
      reference operator[]( difference_type const n ) const {
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return reference( 0 );
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return reference( 0 );
  	}
  	// Check that position make sense
  	difference_type const p = reverse_t ? position - n : position + n;
  	if( p >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference beyond rbegin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return reference( 0 );
  	}
  	if( p <= -1 ){
  	  gsl_error( "trying to dereference beyond begin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return reference( 0 );
  	}
  	// p is a valid position
  	return reference( v->ccgsl_pointer->data + CCGSL_MTY * p * v->ccgsl_pointer->stride );
      }
      // // operator-: find distance between two iterators
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( iterator_base<container,content,reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	} else if( v->ccgsl_pointer == 0 or i.v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return 0;
  	}
  	return reverse_t ? i.position - position : position - i.position;
      }
      // // operator!=
      // // operator<
      /**
       * The == operator.
       * @param i The iterator being compared
       * @return @c true or @c false according as i != *this
       */
      bool operator==( iterator_base<container,content,reverse_t> const& i ) const {
	return this->v == i.v and this->position == i.position;
      }
      /**
       * The != operator.
       * @param i The iterator being compared
       * @return @c true or @c false according as i != *this
       */
      bool operator!=( iterator_base<container,content,reverse_t> const& i ) const {
	return not this->operator==( i );
      }
      /**
       * The < operator is used to compare iterators. This only makes sense
       * if the iterators iterate over the same vector_complex_long_double and the function calls
       * a GSL error handler and returns @c false if they do not.
       * @param i The iterator being compared
       * @return @c true or @c false according as i < j
       */
      bool operator<( iterator_base<container,content,reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse_t ? i.position < position : position < i.position;
      }
    protected:
      /**
       * Increment the iterator.
       * @return 0 for success, anything else for failure
       */
      void increment(){
  	// Only makes sense if v points to a vector_complex_long_double
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// increment position and check against size
  	if( reverse_t ){ if( position >= 0 ) --position; }
  	else { if( position < static_cast<difference_type>( v->size() ) ) ++position; }
      }
      /**
       * Decrement the iterator.
       * @return 0 for success, anything else for failure
       */
      void decrement(){
  	// Only makes sense if v points to a vector_complex_long_double
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// decrement position and check against size
  	if( reverse_t ){ if( position < static_cast<difference_type>( v->size() ) ) ++position; }
  	else { if( position >= 0 ) --position; }
      }
      /**
       * Shift iterator n places
       * @param n A difference_type value to be added to position of iterator
       */
      void shift( difference_type const n ){
  	// Warn if iterator undefined
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	position += reverse_t ? -n : n;
      }
      /**
       * The iterator is default constructible.
       */
      iterator_base<container,content,reverse_t>(){ v = 0; }
      /**
       * This constructor allows vector_complex_long_double to create non-default iterators.
       * @param v The vector_complex_long_double that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_base<container,content,reverse_t>( container* v, difference_type position )
  	: v( v ), position( position ){}
      /**
       * Store a pointer to a vector_complex_long_double we can iterate over: 0 if no vector_complex_long_double.
       */
      container* v;
      /**
       * Mark position of iterator within vector_complex_long_double.
       */
      difference_type position;
    };
    // Need to know about const_iterator_t
    template<bool reverse_t> class const_iterator_t;
    /**
     * A class template for the two non-const iterators.
     */
    template<bool reverse_t> class iterator_t : public iterator_base<vector_complex_long_double,long double,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      iterator_t<reverse_t>& operator=( iterator_t<reverse_t> const& i ){
  	iterator_base<vector_complex_long_double,long double,reverse_t>::v = i.v;
  	iterator_base<vector_complex_long_double,long double,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator++(){
  	iterator_base<vector_complex_long_double,long double,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t> operator++( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<vector_complex_long_double,long double,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator--(){
  	iterator_base<vector_complex_long_double,long double,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this;
       */
      iterator_t<reverse_t> operator--( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<vector_complex_long_double,long double,reverse_t>::decrement();
  	return result;
      }
      /**
       * Difference type.
       */
      typedef typename iterator_base<vector_complex_long_double,long double,reverse_t>::difference_type
  	difference_type;
      // // operator+=
      /**
       * The += operator.
       * @param n A difference_type value to be added to position of iterator
       * @return @c this
       */
      iterator_t<reverse_t>& operator+=( difference_type const n ){
  	this->shift( n );
  	return *this;
      }
      // // operator-=
      /**
       * The -= operator.
       * @param n A difference_type value to be subtracted from position of iterator
       * @return @c this
       */
      iterator_t<reverse_t>& operator-=( difference_type const n ){
  	this->shift( -n );
  	return *this;
      }
      // // operator+ (n+i)(i+n)
      /**
       * The + operator.
       * @param n A difference_type value to be added
       * @return A new iterator
       * @see ccgsl::operator+();
       */
      iterator_t<reverse_t> operator+( difference_type const n ) const {
  	iterator_t<reverse_t> result( *this );
  	result.shift( n );
  	return result;
      }
      // // operator- (n-i)(i-n)(i-j)
      /**
       * The - operator: subtract distance from iterator
       * @param n A difference_type value to be subtracted
       * @return A new iterator
       * @see ccgsl::operator-();
       */
      iterator_t<reverse_t> operator-( difference_type const n ) const {
  	iterator_t<reverse_t> result( *this );
  	result.shift( -n );
  	return result;
      }
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( iterator_t<reverse_t> const& i ) const {
  	return iterator_base<vector_complex_long_double,long double,reverse_t>::operator-( i );
      }
      /**
       * The - operator: find distance between two iterators
       * @param i A const iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( const_iterator_t<reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	} else if( this->v->ccgsl_pointer == 0 or i.v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return 0;
  	}
  	return reverse_t ? i.position - this->position : this->position - i.position;
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to same element as @c i
       */
      bool operator==( const_iterator_t<reverse_t> const& i ) const {
       	return this->v == i.v and this->position == i.position;
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c false or @c true according as @c this points to same element as @c i
       */
      bool operator!=( const_iterator_t<reverse_t> const& i ) const {
       	return not this->operator==( i );
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to earlier element than @c i
       */
      bool operator<( const_iterator_t<reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse_t ? i.position < this->position : this->position < i.position;
      }
      /**
       * The default constructor.
       */
      iterator_t<reverse_t>() : iterator_base<vector_complex_long_double,long double,reverse_t>(){}
    protected:
      friend class vector_complex_long_double;
      // We need a constructor for vector_complex_long_double
      /**
       * This constructor allows vector_complex_long_double to create non-default iterators.
       * @param v The vector_complex_long_double that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_t<reverse_t>( vector_complex_long_double* v, difference_type position )
      : iterator_base<vector_complex_long_double,long double,reverse_t>( v, position ){}
    };
    /**
     * A class template for the const iterators.
     */
    template<bool reverse_t> class const_iterator_t
      : public iterator_base<vector_complex_long_double const,long double,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      const_iterator_t<reverse_t>& operator=( const_iterator_t<reverse_t> const& i ){
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::v = i.v;
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator++(){
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator++( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator--(){
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator--( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<vector_complex_long_double const,long double,reverse_t>::decrement();
  	return result;
      }
      /**
       * Difference type.
       */
      typedef typename iterator_base<vector_complex_long_double const,long double,reverse_t>::difference_type
  	difference_type;
      // // operator+=
      /**
       * The += operator.
       * @param n A difference_type value to be added to position of iterator
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator+=( difference_type const n ){
  	this->shift( n );
  	return *this;
      }
      // // operator-=
      /**
       * The -= operator.
       * @param n A difference_type value to be subtracted from position of iterator
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator-=( difference_type const n ){
  	this->shift( -n );
  	return *this;
      }
      // // operator+ (n+i)(i+n)
      /**
       * The + operator.
       * @param n A difference_type value to be added
       * @return A new iterator
       * @see ccgsl::operator+();
       */
      const_iterator_t<reverse_t> operator+( difference_type const n ) const {
  	const_iterator_t<reverse_t> result( *this );
  	result += n;
  	return result;
      }
      // // operator- (n-i)(i-n)(i-j)
      /**
       * The - operator: subtract distance from iterator
       * @param n A difference_type value to be subtracted
       * @return A new iterator
       * @see ccgsl::operator-();
       */
      const_iterator_t<reverse_t> operator-( difference_type const n ) const {
  	const_iterator_t<reverse_t> result( *this );
  	result -= n;
  	return result;
      }
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( const_iterator_t<reverse_t> const& i ) const {
  	return iterator_base<vector_complex_long_double const,long double,reverse_t>::operator-( i );
      }
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( iterator_t<reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	} else if( this->v->ccgsl_pointer == 0 or i.v->ccgsl_pointer == 0 ){
  	  gsl_error( "vector_complex_long_double not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return 0;
  	}
  	return reverse_t ? i.position - this->position : this->position - i.position;
      }
      /**
       * The default constructor.
       */
      const_iterator_t<reverse_t>() : iterator_base<vector_complex_long_double,long double,reverse_t>(){}
      /**
       * A copy constructor
       * @param i The non-const iterator to copy
       */
      const_iterator_t<reverse_t>( iterator_t<reverse_t> const&i ){
  	const_iterator_t<reverse_t>::v = i.v;
  	const_iterator_t<reverse_t>::position = i.position;
      }
      /**
       * Comparison with non-const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to same element as @c i
       */
      bool operator==( iterator_t<reverse_t> const& i ) const {
       	return this->v == i.v and this->position == i.position;
      }
      /**
       * Comparison with non-const iterator.
       * @param i another iterator
       * @return @c false or @c true according as @c this points to same element as @c i
       */
      bool operator!=( iterator_t<reverse_t> const& i ) const {
       	return not this->operator==( i );
      }
      /**
       * Comparison with non-const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to earlier element than @c i
       */
      bool operator<( iterator_t<reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same vector_complex_long_double
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex_long_double objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse_t ? i.position < this->position : this->position < i.position;
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to same element as @c i
       */
      bool operator==( const_iterator_t<reverse_t> const& i ) const {
       	return this->v == i.v and this->position == i.position;
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c false or @c true according as @c this points to same element as @c i
       */
      bool operator!=( const_iterator_t<reverse_t> const& i ) const {
       	return not this->operator==( i );
      }
      /**
       * Comparison with const iterator.
       * @param i another iterator
       * @return @c true or @c false according as @c this points to earlier element than @c i
       */
      bool operator<( const_iterator_t<reverse_t> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same vector_complex
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different vector_complex objects", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse_t ? i.position < this->position : this->position < i.position;
      }
    protected:
      // We need a constructor for vector_complex_long_double
      friend class vector_complex_long_double;
      /**
       * This constructor allows vector_complex_long_double to create non-default iterators.
       * @param v The vector_complex_long_double that creates @c this
       * @param position The initial postion of the iterator
       */
      const_iterator_t<reverse_t>( vector_complex_long_double const* v, difference_type position )
      : iterator_base<vector_complex_long_double const,long double,reverse_t>( v, position ){}
    };
  public:
    /**
     * The const_iterator type.
     */
    typedef const_iterator_t<false> const_iterator;
    /**
     * The iterator type.
     */
    typedef iterator_t<false> iterator;
    /**
     * The const_reverse_t type.
     */
    typedef const_iterator_t<true> const_reverse_iterator;
    /**
     * The reverse_iterator type.
     */
    typedef iterator_t<true> reverse_iterator;
    // type difference_type
    /**
     * A container must have a difference_type.
     */
    typedef  const_iterator::difference_type difference_type;
    // type size_type
    /**
     * A container must have a size_type.
     */
    typedef  size_t size_type;
    // begin()
    /**
     * Get iterator pointing to first vector_complex_long_double element.
     * @return iterator pointing to first vector_complex_long_double element
     */
    iterator begin(){
      return iterator( this, 0 );
    }
    /**
     * Get iterator pointing to first vector_complex_long_double element.
     * @return iterator pointing to first vector_complex_long_double element
     */
    const_iterator begin() const {
      return const_iterator( this, 0 );
    }
    // end()
    /**
     * Get iterator pointing beyond last vector_complex_long_double element.
     * @return iterator pointing beyond last vector_complex_long_double element
     */
    iterator end(){
      if( ccgsl_pointer == 0 ) return iterator( this, 0 );
      return iterator( this, size() );
    }
    /**
     * Get iterator pointing beyond last vector_complex_long_double element.
     * @return iterator pointing beyond last vector_complex_long_double element
     */
    const_iterator end() const {
      if( ccgsl_pointer == 0 ) return const_iterator( this, 0 );
      return const_iterator( this, size() );
    }
    // size()
    /**
     * The size (number of elements) of the vector_complex_long_double.
     * @return The size of the vector_complex_long_double
     */
    size_type size() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size; }
    /**
     * Give access to the data block. The data() and size() functions mimic the functions
     * of std::array<T> and std::vector<T>. This function can throw an exception or produce
     * a GSL error if the vector stride is not 1. However, new vector objects are always initialised
     * to have a stride of 1.
     * @return The data block.
     */
    long double* data() { 
      if( ccgsl_pointer == 0 ) gsl_error( "null vector", __FILE__, __LINE__, GSL_EFAULT );
#ifndef GSL_RANGE_CHECK_OFF
      if( ccgsl_pointer->stride != 1 )
	gsl_error( "vector does not have stride of size 1", __FILE__, __LINE__, GSL_EBADLEN );
#endif
      return ccgsl_pointer->data; }
    /**
     * Give access to the data block. The data() and size() functions mimic the functions
     * of std::array<T> and std::vector<T>. This function can throw an exception or produce
     * a GSL error if the vector stride is not 1. However, new vector objects are always initialised
     * to have a stride of 1.
     * @return The data block.
     */
    long double const* data() const {
      if( ccgsl_pointer == 0 ) gsl_error( "null vector", __FILE__, __LINE__, GSL_EFAULT );
#ifndef GSL_RANGE_CHECK_OFF
      if( ccgsl_pointer->stride != 1 )
	gsl_error( "vector does not have stride of size 1", __FILE__, __LINE__, GSL_EBADLEN );
#endif
      return ccgsl_pointer->data; }
    // max_size()
    /**
     * The max size (number of elements) of the vector_complex_long_double. Identical to
     * size but required for a container.
     * @return The size of the vector_complex_long_double
     */
    size_type max_size() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size; }
    // empty()
    /**
     * Find if the vector_complex_long_double is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0 or ccgsl_pointer->size == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two vector_complex_long_double objects. This works even if the vector_complex_long_double objects have different sizes
     * because it swaps pointers.
     * @param v The vector_complex_long_double to swap with @c this.
     */
    void swap( vector_complex_long_double& v ){
      gsl_vector_complex_long_double* tmp = ccgsl_pointer; ccgsl_pointer = v.ccgsl_pointer; v.ccgsl_pointer = tmp;
      size_t* tmp2 = count; count = v.count; v.count = tmp2;
    }
    // Refines reversible container
    // rbegin()
    /**
     * Get iterator pointing to first vector_complex_long_double element.
     * @return iterator pointing to first vector_complex_long_double element
     */
    reverse_iterator rbegin(){
      if( ccgsl_pointer ==0 ) return reverse_iterator( this, 0 );
      return reverse_iterator( this, size() - 1 );
    }
    /**
     * Get iterator pointing to first vector_complex_long_double element.
     * @return iterator pointing to first vector_complex_long_double element
     */
    const_reverse_iterator rbegin() const {
      if( ccgsl_pointer ==0 ) return const_reverse_iterator( this, 0 );
      return const_reverse_iterator( this, size() - 1 );
    }
    // rend()
    /**
     * Get iterator pointing beyon last vector_complex_long_double element.
     * @return iterator pointing beyond last vector_complex_long_double element
     */
    reverse_iterator rend(){
      return reverse_iterator( this, -1 );
    }
    /**
     * Get iterator pointing beyon last vector_complex_long_double element.
     * @return iterator pointing beyond last vector_complex_long_double element
     */
    const_reverse_iterator rend() const {
      return const_reverse_iterator( this, -1 );
    }
    // operator[]
    /**
     * Get element at position @c n by reference ([] operator).
     * @param n The position of the element
     * @return a reference to a complex_long_double
     */
    complex_long_double_ref operator[]( size_t const n ){
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "vector_complex_long_double is null", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return complex_long_double_ref( 0 );
      }
#ifndef GSL_RANGE_CHECK_OFF
      // Check that position make sense
      if( n >= size() ){
  	gsl_error( "trying to read beyond end of vector_complex_long_double", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return complex_long_double_ref( 0 );
      }
      // n is a valid position
#endif
      return complex_long_double_ref( ccgsl_pointer->data + CCGSL_MTY * n * ccgsl_pointer->stride );
    }
    /**
     * Get element at position @c n by reference ([] operator).
     * @param n The position of the element
     * @return a reference to a complex_long_double
     */
    complex_long_double_ref const  operator[]( size_t const n ) const {
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "vector_complex_long_double is null", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return complex_long_double_ref( 0 );
      }
#ifndef GSL_RANGE_CHECK_OFF
      // Check that position make sense
      if( n >= size() ){
  	gsl_error( "trying to read beyond end of vector_complex_long_double", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return complex_long_double_ref( 0 );
      }
      // n is a valid position
#endif
      return complex_long_double_ref( ccgsl_pointer->data + n * ccgsl_pointer->stride );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_vector_complex_long_double* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_vector_complex_long_double.
     * @return the gsl_vector_complex_long_double
     */
    gsl_vector_complex_long_double* get() { return ccgsl_pointer; }
    /**
     * Get the gsl_vector_complex_long_double.
     * @return the gsl_vector_complex_long_double
     */
    gsl_vector_complex_long_double const* get() const { return ccgsl_pointer; }
    /**
     * Find if @c this is the only object sharing the gsl_vector_complex_long_double.
     * @return @c true or @c falses according as 
     * this is the only vector_complex_long_double object sharing the gsl_vector_complex_long_double
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many vector_complex_long_double objects share this pointer.
     * @return the number of vector_complex_long_double objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as @c this contains a pointer
     * to a gsl_vector_complex_long_double
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    // GSL functions
    /**
     * C++ version of gsl_vector_complex_long_double_calloc(). This constructs a vector_complex_long_double object with entries
     * initialised to zero.
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double initialised to zero
     */
    static vector_complex_long_double calloc( size_t const n ){ return vector_complex_long_double( gsl_vector_complex_long_double_calloc( n ) ); }
    /**
     * C++ version of gsl_vector_complex_long_double_set_zero().
     */
    void set_zero(){ gsl_vector_complex_long_double_set_zero( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_set_all().
     * @param x The value to which all elements are set
     */
    void set_all( complex_long_double x ){ gsl_vector_complex_long_double_set_all( get(), x ); }
    /**
     * C++ version of gsl_vector_complex_long_double_set_basis(). Creates a basis vector_complex_long_double with one nonzero element.
     * @param i The element to be set to 1.
     * @return error code on failure
     */
    int set_basis( size_t i ){ return gsl_vector_complex_long_double_set_basis( get(), i ); }
    /**
     * C++ version of gsl_vector_complex_long_double_memcpy().
     * @param src source vector_complex_long_double
     * @return error code on failure
     */
    int memcpy( vector_complex_long_double const& src ){ return gsl_vector_complex_long_double_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_reverse().
     * @return error code on failure
     */
    int reverse(){ return gsl_vector_complex_long_double_reverse( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_swap_elements().
     * @param i first element
     * @param j second element
     * @return error code on failure
     */
    int swap_elements( size_t const i, size_t const j ){
      return gsl_vector_complex_long_double_swap_elements( get(), i, j ); }
    /**
     * C++ version of gsl_vector_complex_long_double_add().
     * @param b vector_complex_long_double to add to @c this
     * @return error code on failure
     */
    int add( vector_complex_long_double const& b ){ return gsl_vector_complex_long_double_add( get(), b.get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_sub().
     * @param b vector_complex_long_double to subtract from @c this
     * @return error code on failure
     */
    int sub( vector_complex_long_double const& b ){ return gsl_vector_complex_long_double_sub( get(), b.get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_mul().
     * @param b vector_complex_long_double to multiply elementwise with @c this
     * @return error code on failure
     */
    int mul( vector_complex_long_double const& b ){ return gsl_vector_complex_long_double_mul( get(), b.get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_div().
     * @param b vector_complex_long_double to dividev @c this by elementwise
     * @return error code on failure
     */
    int div( vector_complex_long_double const& b ){ return gsl_vector_complex_long_double_div( get(), b.get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_scale().
     * @param x constant to multiply @c this by
     * @return error code on failure
     */
    int scale( complex_long_double const x ){ return gsl_vector_complex_long_double_scale( get(), x ); }
    /**
     * C++ version of gsl_vector_complex_long_double_add_constant().
     * @param x constant to add to each element of @c this
     * @return error code on failure
     */
    int add_constant( complex_long_double const x ){ return gsl_vector_complex_long_double_add_constant( get(), x ); }
    /**
     * C++ version of gsl_vector_complex_long_double_isnull().
     * @return @c +1 or @c 0 according as elements are all zero or not
     */
    int isnull() const { return gsl_vector_complex_long_double_isnull( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_ispos().
     * @return @c +1 or @c 0 according as elements are all positive or not
     */
    int ispos() const { return gsl_vector_complex_long_double_ispos( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_isneg().
     * @return @c +1 or @c 0 according as elements are all negative or not
     */
    int isneg() const { return gsl_vector_complex_long_double_isneg( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_isnonneg().
     * @return @c +1 or @c 0 according as elements are all nonnegative or not
     */
    int isnonneg() const { return gsl_vector_complex_long_double_isnonneg( get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_get().
     * @param i index of element to get
     * @return value of element
     */
    complex_long_double get( size_t const i ) const { return gsl_vector_complex_long_double_get( get(), i ); }
    /**
     * C++ version of gsl_vector_complex_long_double_set().
     * @param i index to set
     * @param x new value for element
     */
    void set( size_t const i, complex_long_double x ){ gsl_vector_complex_long_double_set( get(), i, x ); }
    /**
     * C++ version of gsl_vector_complex_long_double_ptr().
     * @param i index of element to get
     * @return pointer to element
     */
    complex_long_double_ptr ptr( size_t const i ){
      if( i >= ccgsl_pointer->size )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      return complex_long_double_ptr( ccgsl_pointer->data + CCGSL_MTY * i ); }
    /**
     * C++ version of gsl_vector_complex_long_double_const_ptr().
     * @param i index of element to get
     * @return pointer to element
     */
    complex_long_double_ptr const const_ptr( size_t const i ){
      if( i >= ccgsl_pointer->size )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      return complex_long_double_ptr( ccgsl_pointer->data + CCGSL_MTY * i ); }
    /**
     * C++ version of gsl_vector_complex_long_double_fread().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fread( FILE* stream ){ return gsl_vector_complex_long_double_fread( stream, get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_fwrite().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_vector_complex_long_double_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_fscanf().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_vector_complex_long_double_fscanf( stream, get() ); }
    /**
     * C++ version of gsl_vector_complex_long_double_fprintf().
     * @param stream A C file stream
     * @param format %d, %e, %f or %g
     * @return error code on failure
     */
    int fprintf( FILE* stream, char const* format ) const {
      return gsl_vector_complex_long_double_fprintf( stream, get(), format ); }
    /**
     * C++ version of gsl_vector_complex_long_double_alloc_from_block().
     * @param b The block_complex_long_double
     * @param offset The offset within the block_complex_long_double
     * @param n The number of elements
     * @param stride The stride
     */
    vector_complex_long_double( block_complex_long_double& b, size_t const offset, size_t const n, size_t const stride = 1 ){
      ccgsl_pointer = gsl_vector_complex_long_double_alloc_from_block( b.get(), offset, n, stride );
      // just plausibly we could allocate vector_complex_long_double but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_vector_complex_long_double_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * C++ version of gsl_vector_complex_long_double_alloc_from_vector().
     * @param v The vector_complex_long_double
     * @param offset The offset
     * @param n The number of elements
     * @param stride The stride
     */
    vector_complex_long_double( vector_complex_long_double& v, size_t const offset, size_t const n, size_t const stride = 1 ){
      ccgsl_pointer = gsl_vector_complex_long_double_alloc_from_vector( v.get(), offset, n, stride );
      // just plausibly we could allocate vector_complex_long_double but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_vector_complex_long_double_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * C++ version of gsl_vector_complex_real().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The real parts of each element 
     */
    gsl::vector_long_double real(){
      return gsl::vector_long_double::view_array_with_stride( data(), CCGSL_MTY, size()  ); }
    /**
     * C++ version of gsl_vector_complex_const_real().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The real parts of each element 
     */
    gsl::vector_long_double const const_real() const {
      return gsl::vector_long_double::const_view_array_with_stride( data(), CCGSL_MTY, size()  ); }
    /**
     * Another C++ version of gsl_vector_complex_const_real().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The real parts of each element 
     */
    gsl::vector_long_double const real() const {
      return gsl::vector_long_double::const_view_array_with_stride( data(), CCGSL_MTY, size()  ); }
    /**
     * C++ version of gsl_vector_complex_imag().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The imaginary parts of each element 
     */
    gsl::vector_long_double imag(){
      return gsl::vector_long_double::view_array_with_stride( data() + CCGSL_MTY / 2, CCGSL_MTY, size()  ); }
    /**
     * C++ version of gsl_vector_complex_const_imag().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The imaginary parts of each element 
     */
    gsl::vector_long_double const const_imag() const {
      return gsl::vector_long_double::const_view_array_with_stride( data() + CCGSL_MTY / 2, CCGSL_MTY, size()  ); }
    /**
     * Another C++ version of gsl_vector_complex_const_imag().
     * Note that this is a view: if the complex_vector from which the return value is
     * constructed is deleted then the return value should not be used.
     * @return The imaginary parts of each element 
     */
    gsl::vector_long_double const imag() const {
      return gsl::vector_long_double::const_view_array_with_stride( data() + CCGSL_MTY / 2, CCGSL_MTY, size()  ); }
    /**
     * C++ version of gsl_vector_complex_long_double_view_array().
     * @param v An array of type long double
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double view_array( long double* v, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_view_array( v, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_view_array_with_stride().
     * @param base An array of type long double
     * @param stride The stride
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double view_array_with_stride( long double* base, size_t stride, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_view_array_with_stride( base, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_const_view_array().
     * @param v An array of type long double
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double const const_view_array( long double const* v, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_view_array( v, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_const_view_array_with_stride().
     * @param base An array of type long double
     * @param stride The stride
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double const const_view_array_with_stride( long double const* base, size_t stride, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_view_array_with_stride( base, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * Another C++ version of gsl_vector_complex_long_double_const_view_array().
     * @param v An array of type long double
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double const view_array( long double const* v, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_view_array( v, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * Another C++ version of gsl_vector_complex_long_double_const_view_array_with_stride().
     * @param base An array of type long double
     * @param stride The stride
     * @param n The size of the vector_complex_long_double
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double const view_array_with_stride( long double const* base, size_t stride, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_view_array_with_stride( base, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_subvector().
     * @param i The offset
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double subvector( size_t i, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_subvector( get(), i, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_subvector_with_stride().
     * @param i The offset
     * @param stride The stride
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double subvector_with_stride( size_t i, size_t stride, size_t n ){
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_subvector_with_stride( get(), i, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_const_subvector().
     * @param i The offset
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double const const_subvector( size_t i, size_t n ) const {
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_subvector( get(), i, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * C++ version of gsl_vector_complex_long_double_const_subvector_with_stride().
     * @param i The offset
     * @param stride The stride
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double const const_subvector_with_stride( size_t i, size_t stride, size_t n ) const {
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_subvector_with_stride( get(), i, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * Another C++ version of gsl_vector_complex_long_double_const_subvector().
     * @param i The offset
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double const subvector( size_t i, size_t n ) const {
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_subvector( get(), i, n ).vector;
      return vector_complex_long_double( w );
    }
    /**
     * Another C++ version of gsl_vector_complex_long_double_const_subvector_with_stride().
     * @param i The offset
     * @param stride The stride
     * @param n The size
     * @return A subvector
     */
    vector_complex_long_double const subvector_with_stride( size_t i, size_t stride, size_t n ) const {
      gsl_vector_complex_long_double* w = static_cast<gsl_vector_complex_long_double*>( malloc( sizeof( gsl_vector_complex_long_double ) ) );
      *w = gsl_vector_complex_long_double_const_subvector_with_stride( get(), i, stride, n ).vector;
      return vector_complex_long_double( w );
    }
    // Extra allocators from matrix_complex_long_double objects. The definition must come after matrix_complex_long_double.
    /**
     * C++ version of gsl_vector_complex_long_double_alloc_row_from_matrix().
     * @param m A matrix_complex_long_double
     * @param i A row
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double alloc_row_from_matrix( matrix_complex_long_double& m, size_t const i );
    /**
     * C++ version of gsl_vector_complex_long_double_alloc_col_from_matrix().
     * @param m A matrix
     * @param j A column
     * @return A vector_complex_long_double
     */
    static vector_complex_long_double alloc_col_from_matrix( matrix_complex_long_double& m, size_t const j );
  };

  /**
   * Allows constant to be added to iterator.
   * @param n The constant
   * @param i The iterator
   * @see vector_complex_long_double::iterator::operator+()
   */
  inline vector_complex_long_double::iterator operator+
  ( vector_complex_long_double::iterator::difference_type const n, vector_complex_long_double::iterator const& i ){ return i + n; }
  
  /**
   * Allows constant to be added to iterator.
   * @param n The constant
   * @param i The iterator
   * @see vector_complex_long_double::iterator::operator+()
   */
  inline vector_complex_long_double::const_iterator operator+
  ( vector_complex_long_double::const_iterator::difference_type const n, vector_complex_long_double::const_iterator const& i ){ return i + n; }
  
  /**
   * Allows constant to be added to iterator.
   * @param n The constant
   * @param i The iterator
   * @see vector_complex_long_double::iterator::operator+()
   */
  inline vector_complex_long_double::reverse_iterator operator+
  ( vector_complex_long_double::reverse_iterator::difference_type const n, vector_complex_long_double::reverse_iterator const& i ){
    return i + n; }
  
  /**
   * Allows constant to be added to iterator.
   * @param n The constant
   * @param i The iterator
   * @see vector_complex_long_double::iterator::operator+()
   */
  inline vector_complex_long_double::const_reverse_iterator operator+
  ( vector_complex_long_double::const_reverse_iterator::difference_type const n, vector_complex_long_double::const_reverse_iterator const& i ){
    return i + n; }
  
}
#undef CCGSL_MTY
#endif
