/*
 * $Id: permutation.hpp 315 2014-02-22 14:31:35Z jdl3 $
 * Copyright (C) 2010, 2011, 2012, 2014 John D Lamb
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

#ifndef CCGSL_PERMUTATION_HPP
#define CCGSL_PERMUTATION_HPP

#include<cstddef>
#include<new>
#include<algorithm>
#include<gsl/gsl_permutation.h>
#include"exception.hpp"

namespace gsl {
  /**
   * This class handles GSL permutation objects.
   */
  class permutation {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    permutation(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    /**
     * This constructor creates a new permutation with n elements
     * @param n The number of elements in the permutation
     * @param init Set to @em true to have the permutation initialised
     */
    explicit permutation( size_t const n, bool init = false ){
      ccgsl_pointer = gsl_permutation_alloc( n );
      // just plausibly we could allocate permutation but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_permutation_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
      if( init ) this->init();
    }
    /**
     * Could construct from a gsl_permutation. This is not usually a good idea. In this case
     * you should not use gsl_permutation_free() to deallocate the memory.
     * @param v The permutation
     */
    explicit permutation( gsl_permutation* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This shares the permutation. Use clone() if you want a full copy.
     * @param v The permutation to copy.
     */
    permutation( permutation const& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( v.count ){
      if( count != 0 ) ++*count; // permutation is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The permutation to copy
     */
    permutation& operator=( permutation const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_permutation_free( ccgsl_pointer );
	delete count;
      }
      // Then copy
      ccgsl_pointer = v.ccgsl_pointer;
      count = v.count;
      if( count != 0 ) ++*count; // permutation is now shared.
      return *this;
    }
    // clone()
    /**
     * The clone function. Use this if you want a copy of the permutation that does
     * not share the underlying data.
     * @return a new copy of this.
     */
    permutation clone() const {
      permutation copy( get()->size );
      // Now copy
      gsl_permutation_memcpy( copy.get(), get() );
      // return new object
      return copy;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~permutation(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_permutation_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The permutation to move.
     */
    permutation( permutation&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The permutation to move.
     * @return A reference to this.
     */
    permutation& operator=( permutation&& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
      return *this;
    }
#endif
  private:
    /**
     * The shared pointer
     */
    gsl_permutation* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_permutation.
     * @return the gsl_permutation
     */
    gsl_permutation* get(){ return ccgsl_pointer; }
    /**
     * Get the gsl_permutation.
     * @return the gsl_permutation
     */
    gsl_permutation const* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_permutation.
     * @return @c true or @c falses according as 
     * this is the only permutation object sharing the gsl_permutation
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many permutation objects share this pointer.
     * @return the number of permutation objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_permutation
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
  public:
    // GSL functions
    /**
     * C++ version of gsl_permutation_calloc().
     * @param n The size of the permutation
     * @return A new permutation, initialised to the identity
     */
    static permutation calloc( size_t const n ){ return permutation( gsl_permutation_calloc( n ) ); }
    /**
     * C++ version of gsl_permutation_init().
     */
    void init(){ gsl_permutation_init( get() ); }
    /**
     * C++ version of gsl_permutation_memcpy().
     * @param src The permutation to copy
     * @return Error code on failure
     */
    int memcpy( permutation const& src ){ return gsl_permutation_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_permutation_fread().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fread( FILE* stream ){ return gsl_permutation_fread( stream, get() ); }
    /**
     * C++ version of gsl_permutation_fwrite().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_permutation_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_permutation_fscanf().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_permutation_fscanf( stream, get() ); }
    /**
     * C++ version of gsl_permutation_fprintf().
     * @param stream An output stream
     * @param format A print format: typically "%zu\n"
     * @return Error code on failure
     */
    int fprintf( FILE* stream, char const* format ) const {
      return gsl_permutation_fprintf( stream, get(), format ); }
    /**
     * C++ version of gsl_permutation_size().
     * @return The size
     */
    size_t size() const { return gsl_permutation_size( get() ); }
    /**
     * C++ version of gsl_permutation_data().
     * @return Pointer to the array of elements
     */
    size_t* data() const { return gsl_permutation_data( get() ); }
    /**
     * C++ version of gsl_permutation_swap().
     * @param i An element
     * @param j An element to swap with @c i
     * @return Error code on failure
     */
    int swap( size_t const i, size_t const j ){ return gsl_permutation_swap( get(), i, j ); }
    /**
     * C++ version of gsl_permutation_valid().
     * @return Error code for valid
     */
    int valid() const { return gsl_permutation_valid( get() ); }
    /**
     * C++ version of gsl_permutation_reverse().
     */
    void reverse(){ gsl_permutation_reverse( get() ); }
    /**
     * C++ version of gsl_permutation_inverse().
     * @param p Another permutation
     * @return Error code on failure
     */
    int inverse( permutation const& p ){ return gsl_permutation_inverse( get(), p.get() ); }
    /**
     * C++ version of gsl_permutation_next().
     * @return Error code on failure
     */
    int next(){ return gsl_permutation_next( get() ); }
    /**
     * C++ version of gsl_permutation_prev().
     * @return Error code on failure
     */
    int prev(){ return gsl_permutation_prev( get() ); }
    /**
     * C++ version of gsl_permutation_mul().
     * @param pa The first permutation
     * @param pb The second permutation (applied after @c pb)
     * @return Error code on failure
     */
    int mul( permutation const& pa, permutation const& pb ){
      return gsl_permutation_mul( get(), pa.get(), pb.get() ); }

    /**
     * C++ version of gsl_permutation_linear_to_canonical().
     * @param p A permutation
     * @return Error code on failure
     */
    int linear_to_canonical( permutation const& p ){
      return gsl_permutation_linear_to_canonical( get(), p.get() ); }
    /**
     * C++ version of gsl_permutation_canonical_to_linear().
     * @param q Apermutation
     * @return Error code on failure
     */
    int canonical_to_linear( permutation const& q ){
      return gsl_permutation_canonical_to_linear( get(), q.get() ); }
    /**
     * C++ version of gsl_permutation_inversions().
     * @return The number of inversions (pairs not in order)
     */
    size_t inversions() const { return gsl_permutation_inversions( get() ); }
    /**
     * C++ version of gsl_permutation_linear_cycles().
     * @return The number of cycles (linear form)
     */
    size_t linear_cycles() const { return gsl_permutation_linear_cycles( get() ); }
    /**
     * C++ version of gsl_permutation_canonical_cycles().
     * @return The number of cycles (canonical form)
     */
    size_t canonical_cycles(  ) const { return gsl_permutation_canonical_cycles( get() ); }
    /**
     * C++ version of gsl_permutation_get().
     * @param i index of element
     * @return Element at index @c i
     */
    size_t get( size_t const i ) const { return gsl_permutation_get( get(), i ); }
    /**
     * C++ version of gsl_permutation_get().
     * @param i index of element
     * @return Element at index @c i
     */
    size_t operator[]( size_t const i ) const { return gsl_permutation_get( get(), i ); }
  private:
    /**
     * The container must have iterator types. We create a suitable class
     * here.
     */
    template<typename container, typename content, bool reverse> class iterator_base {
      friend class permutation;
    public:
      /**
       * An iterator must have an iterator category.
       */
      typedef std::random_access_iterator_tag iterator_category;
      /**
       * An iterator must have a value type.
       */
      typedef size_t value_type;
      /**
       * An iterator must have a pointer typea.
       */
      typedef value_type* pointer;
      /**
       * An iterator must have a reference type.
       */
      typedef value_type& reference;
      // // type iterator_traits<permutation>::difference_type
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
  	// Always try to return something
  	static content something = 0;
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAULT );
  	  return something;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAULT );
  	  return something;
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference beyond rbegin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	if( position <= -1 ){
  	  gsl_error( "trying to dereference beyond begin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// position and v are valid: return data
  	return *(v->ccgsl_pointer->data + position);
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
  	  return 0;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference end()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	if( position <= -1 ){
  	  gsl_error( "trying to dereference rend()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// position and v are valid: return data
  	return *(v->ccgsl_pointer->data + position);
      }
      // // operator[]
      /**
       * Get element at i + n by reference ([] operator).
       * @param n The offset from i
       * @return a reference to content
       */
      reference operator[]( difference_type const n ) const {
  	// Always try to return something
  	static content something = 0;
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// Check that position make sense
  	difference_type const p = reverse ? position - n : position + n;
  	if( p >= static_cast<difference_type>( v->size() ) ){
  	  gsl_error( "trying to dereference beyond rbegin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	if( p <= -1 ){
  	  gsl_error( "trying to dereference beyond begin()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// p is a valid position
  	return *(v->ccgsl_pointer->data + p );
      }
      // // operator-: find distance between two iterators
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( iterator_base<container,content,reverse> const& i ) const {
  	// Warn if either iterator undefined
  	if( v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	} else if( v->ccgsl_pointer == 0 or i.v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Warn if iterators do not point to same permutation
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different permutations", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return 0;
  	}
  	return reverse ? i.position - position : position - i.position;
      }
      // // operator!=
      // // operator<
      /**
       * The == operator.
       * @param i The iterator being compared
       * @return @c true or @c false according as i != *this
       */
      bool operator==( iterator_base<container,content,reverse> const& i ) const {
	return this->v == i.v and this->position == i.position;
      }
      /**
       * The != operator.
       * @param i The iterator being compared
       * @return @c true or @c false according as i != *this
       */
      bool operator!=( iterator_base<container,content,reverse> const& i ) const {
	return not this->operator==( i );
      }
      /**
       * The < operator is used to compare iterators. This only makes sense
       * if the iterators iterate over the same permutation and the function calls
       * a GSL error handler and returns @c false if they do not.
       * @param i The iterator being compared
       * @return @c true or @c false according as i < j
       */
      bool operator<( iterator_base<container,content,reverse> const& i ) const {
  	// Warn if either iterator undefined
  	if( v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same permutation
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different permutations", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse ? i.position < position : position < i.position;
      }
    protected:
      /**
       * Increment the iterator.
       * @return 0 for success, anything else for failure
       */
      void increment(){
  	// Only makes sense if v points to a permutation
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// increment position and check against size
  	if( reverse ){ if( position >= 0 ) --position; }
  	else { if( position < static_cast<difference_type>( v->size() ) ) ++position; }
      }
      /**
       * Decrement the iterator.
       * @return 0 for success, anything else for failure
       */
      void decrement(){
  	// Only makes sense if v points to a permutation
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// decrement position and check against size
  	if( reverse ){ if( position < static_cast<difference_type>( v->size() ) ) ++position; }
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
  	  gsl_error( "permutation not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	position += reverse ? -n : n;
      }
      /**
       * The iterator is default constructible.
       */
      iterator_base<container,content,reverse>(){ v = 0; }
      /**
       * This constructor allows permutation to create non-default iterators.
       * @param v The permutation that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_base<container,content,reverse>( container* v, difference_type position )
  	: v( v ), position( position ){}
      /**
       * Store a pointer to a permutation we can iterate over: 0 if no permutation.
       */
      container* v;
      /**
       * Mark position of iterator within permutation.
       */
      difference_type position;
    };
    /**
     * A class template for the const iterators.
     */
    template<bool reverse> class const_iterator_t
      : public iterator_base<permutation const,unsigned long,reverse>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      const_iterator_t<reverse>& operator=( const_iterator_t<reverse> const& i ){
  	iterator_base<permutation const,unsigned long,reverse>::v = i.v;
  	iterator_base<permutation const,unsigned long,reverse>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse>& operator++(){
  	iterator_base<permutation const,unsigned long,reverse>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse> operator++( int ){
  	// return value
  	const_iterator_t<reverse> result( *this );
  	iterator_base<permutation const,unsigned long,reverse>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse>& operator--(){
  	iterator_base<permutation const,unsigned long,reverse>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse> operator--( int ){
  	// return value
  	const_iterator_t<reverse> result( *this );
  	iterator_base<permutation const,unsigned long,reverse>::decrement();
  	return result;
      }
      /**
       * Difference type.
       */
      typedef typename iterator_base<permutation const,unsigned long,reverse>::difference_type
  	difference_type;
      // // operator+=
      /**
       * The += operator.
       * @param n A difference_type value to be added to position of iterator
       * @return @c this
       */
      const_iterator_t<reverse>& operator+=( difference_type const n ){
  	this->shift( n );
  	return *this;
      }
      // // operator-=
      /**
       * The -= operator.
       * @param n A difference_type value to be subtracted from position of iterator
       * @return @c this
       */
      const_iterator_t<reverse>& operator-=( difference_type const n ){
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
      const_iterator_t<reverse> operator+( difference_type const n ) const {
  	const_iterator_t<reverse> result( *this );
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
      const_iterator_t<reverse> operator-( difference_type const n ) const {
  	const_iterator_t<reverse> result( *this );
  	result -= n;
  	return result;
      }
      /**
       * The - operator: find distance between two iterators
       * @param i A second iterator
       * @return (signed) distance between @c this and @c i
       */
      difference_type operator-( const_iterator_t<reverse> const& i ) const {
  	return iterator_base<permutation const,unsigned long,reverse>::operator-( i );
      }
      /**
       * The default constructor.
       */
      const_iterator_t<reverse>() : iterator_base<permutation,unsigned long,reverse>(){}
      /**
       * A copy constructor
       * @param i The iterator to copy
       */
      const_iterator_t<reverse>( const_iterator_t<reverse> const&i ){
  	const_iterator_t<reverse>::v = i.v;
  	const_iterator_t<reverse>::position = i.position;
      }
      /**
       * Comparison with const_iterator.
       * @param i another iterator
       * @return @c true or @c false according as this points to same element as @c i
       */
      bool operator==( const_iterator_t<reverse> const& i ) const {
       	return this->v == i.v and this->position == i.position;
      }
      /**
       * Comparison with const_iterator.
       * @param i another iterator
       * @return @c false or @c true according as this points to same element as @c i
       */
      bool operator!=( const_iterator_t<reverse> const& i ) const {
       	return not this->operator==( i );
      }
      /**
       * Comparison with const_iterator.
       * @param i another iterator
       * @return @c true or @c false according as this points to earlier element than @c i
       */
      bool operator<( const_iterator_t<reverse> const& i ) const {
  	// Warn if either iterator undefined
  	if( this->v == 0 or i.v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return false;
  	}
  	// Warn if iterators do not point to same permutation
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different permutations", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse ? i.position < this->position : this->position < i.position;
      }
    protected:
      // We need a constructor for permutation
      friend class permutation;
      /**
       * This constructor allows permutation to create non-default iterators.
       * @param v The permutation that creates @c this
       * @param position The initial postion of the iterator
       */
      const_iterator_t<reverse>( permutation const* v, difference_type position )
      : iterator_base<permutation const,unsigned long,reverse>( v, position ){}
    };
  public:
    /**
     * The const_iterator type.
     */
    typedef const_iterator_t<false> const_iterator;
    /**
     * The const_reverse_iterator type.
     */
    typedef const_iterator_t<true> const_reverse_iterator;
    // type difference_type
    /**
     * A container must have a difference_type.
     */
    typedef const_iterator::difference_type difference_type;
    // type size_type
    /**
     * A container must have a size_type.
     */
    typedef size_t size_type;
    // begin()
    /**
     * Get iterator pointing to first permutation element.
     * @return iterator pointing to first permutation element
     */
    const_iterator begin() const {
      return const_iterator( this, 0 );
    }
    // end()
    /**
     * Get iterator pointing beyond last permutation element.
     * @return iterator pointing beyond last permutation element
     */
    const_iterator end() const {
      if( ccgsl_pointer == 0 ) return const_iterator( this, 0 );
      return const_iterator( this, size() );
    }
  };

}
#endif
