/*
 * $Id: combination.hpp 316 2014-02-22 14:32:31Z jdl3 $
 * Copyright (C) 2012, 2014 John D Lamb
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

#ifndef CCGSL_COMBINATION_HPP
#define CCGSL_COMBINATION_HPP

#include<cstddef>
#include<new>
#include<algorithm>
#include<gsl/gsl_combination.h>
#include"exception.hpp"

namespace gsl {
  /**
   * This class handles GSL combination objects.
   */
  class combination {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    combination(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    /**
     * This constructor creates a new combination with n elements from which k are chosen
     * @param n The number of elements in the selection set
     * @param k The number of elements chosen
     * @param init Set to @em true to have the combination initialised
     */
    explicit combination( size_t const n, size_t const k, bool init = false ){
      ccgsl_pointer = gsl_combination_alloc( n, k );
      // just plausibly we could allocate combination but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_combination_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
      if( init ) this->init_first();
    }
    /**
     * Could construct from a gsl_combination. This is not usually a good idea. In this case
     * you should not use gsl_combination_free() to deallocate the memory.
     * @param v The combination
     */
    explicit combination( gsl_combination* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This shares the combination. Use clone() if you want a full copy.
     * @param v The combination to copy.
     */
    combination( combination const& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( v.count ){
      if( count != 0 ) ++*count; // combination is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The combination to copy
     */
    combination& operator=( combination const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_combination_free( ccgsl_pointer );
	delete count;
      }
      // Then copy
      ccgsl_pointer = v.ccgsl_pointer;
      count = v.count;
      if( count != 0 ) ++*count; // combination is now shared.
      return *this;
    }
    // clone()
    /**
     * The clone function. Use this if you want a copy of the combination that does
     * not share the underlying data.
     * @return a new copy of this.
     */
    combination clone() const {
      combination copy( get()->n, get()->k );
      // Now copy
      gsl_combination_memcpy( copy.get(), get() );
      // return new object
      return copy;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~combination(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_combination_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The combination to move.
     */
    combination( combination&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The combination to move.
     * @return A reference to this.
     */
    combination& operator=( combination&& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
      return *this;
    }
#endif
  private:
    /**
     * The shared pointer
     */
    gsl_combination* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_combination.
     * @return the gsl_combination
     */
    gsl_combination* get(){ return ccgsl_pointer; }
    /**
     * Get the gsl_combination.
     * @return the gsl_combination
     */
    gsl_combination const* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_combination.
     * @return @c true or @c falses according as 
     * this is the only combination object sharing the gsl_combination
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many combination objects share this pointer.
     * @return the number of combination objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_combination
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
  public:
    // GSL functions
    /**
     * C++ version of gsl_combination_calloc().
     * @param n The number of elements in the selection set
     * @param k The number of elements chosen
     * @return A new combination, initialised to \f$1,2,\ldots,k\f$.
     */
    static combination calloc( size_t const n, size_t const k ){
      return combination( gsl_combination_calloc( n, k ) ); }
    /**
     * C++ version of gsl_combination_init_first().
     */
    void init_first(){ gsl_combination_init_first( get() ); }
    /**
     * C++ version of gsl_combination_init_last().
     */
    void init_last(){ gsl_combination_init_last( get() ); }
    /**
     * C++ version of gsl_combination_memcpy().
     * @param src The combination to copy
     * @return Error code on failure
     */
    int memcpy( combination const& src ){ return gsl_combination_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_combination_fread().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fread( FILE* stream ){ return gsl_combination_fread( stream, get() ); }
    /**
     * C++ version of gsl_combination_fwrite().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_combination_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_combination_fscanf().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_combination_fscanf( stream, get() ); }
    /**
     * C++ version of gsl_combination_fprintf().
     * @param stream An output stream
     * @param format A print format: typically "%zu\n"
     * @return Error code on failure
     */
    int fprintf( FILE* stream, char const* format ) const {
      return gsl_combination_fprintf( stream, get(), format ); }
    /**
     * C++ version of gsl_combination_n(): Size of set.
     * @return The size.
     */
    size_t n() const { return get()->n; }
    /**
     * C++ version of gsl_combination_k(): Number of selections.
     * @return The size
     */
    size_t k() const { return get()->k; }
    /**
     * C++ version of gsl_combination_data().
     * @return Pointer to the array of elements
     */
    size_t* data() { return get()->data; }
    /**
     * C++ version of gsl_combination_valid().
     * @return Error code for valid
     */
    int valid() { return gsl_combination_valid( get() ); }
    /**
     * C++ version of gsl_combination_next().
     * @return Error code on failure
     */
    int next(){ return gsl_combination_next( get() ); }
    /**
     * C++ version of gsl_combination_prev().
     * @return Error code on failure
     */
    int prev(){ return gsl_combination_prev( get() ); }
    /**
     * C++ version of gsl_combination_get().
     * @param i index of element
     * @return Element at index @c i
     */
    size_t get( size_t const i ) const { return gsl_combination_get( get(), i ); }
    /**
     * C++ version of gsl_combination_get().
     * @param i index of element
     * @return Element at index @c i
     */
    size_t operator[]( size_t const i ) const { return gsl_combination_get( get(), i ); }
      private:
    /**
     * The container must have iterator types. We create a suitable class
     * here.
     */
    template<typename container, typename content, bool reverse> class iterator_base {
      friend class combination;
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
      // // type iterator_traits<combination>::difference_type
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
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAULT );
  	  return something;
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->k() ) ){
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
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Check that position make sense
  	if( position >= static_cast<difference_type>( v->k() ) ){
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
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// Check that position make sense
  	difference_type const p = reverse ? position - n : position + n;
  	if( p >= static_cast<difference_type>( v->k() ) ){
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
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return 0;
  	}
  	// Warn if iterators do not point to same combination
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different combinations", __FILE__, __LINE__,
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
       * if the iterators iterate over the same combination and the function calls
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
  	// Warn if iterators do not point to same combination
  	if( v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different combinations", __FILE__, __LINE__,
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
  	// Only makes sense if v points to a combination
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// increment position and check against size
  	if( reverse ){ if( position >= 0 ) --position; }
  	else { if( position < static_cast<difference_type>( v->k() ) ) ++position; }
      }
      /**
       * Decrement the iterator.
       * @return 0 for success, anything else for failure
       */
      void decrement(){
  	// Only makes sense if v points to a combination
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// decrement position and check against size
  	if( reverse ){ if( position < static_cast<difference_type>( v->k() ) ) ++position; }
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
  	  gsl_error( "combination not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	position += reverse ? -n : n;
      }
      /**
       * The iterator is default constructible.
       */
      iterator_base<container,content,reverse>(){ v = 0; }
      /**
       * This constructor allows combination to create non-default iterators.
       * @param v The combination that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_base<container,content,reverse>( container* v, difference_type position )
  	: v( v ), position( position ){}
      /**
       * Store a pointer to a combination we can iterate over: 0 if no combination.
       */
      container* v;
      /**
       * Mark position of iterator within combination.
       */
      difference_type position;
    };
    /**
     * A class template for the const iterators.
     */
    template<bool reverse> class const_iterator_t
      : public iterator_base<combination const,unsigned long,reverse>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      const_iterator_t<reverse>& operator=( const_iterator_t<reverse> const& i ){
  	iterator_base<combination const,unsigned long,reverse>::v = i.v;
  	iterator_base<combination const,unsigned long,reverse>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse>& operator++(){
  	iterator_base<combination const,unsigned long,reverse>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse> operator++( int ){
  	// return value
  	const_iterator_t<reverse> result( *this );
  	iterator_base<combination const,unsigned long,reverse>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse>& operator--(){
  	iterator_base<combination const,unsigned long,reverse>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse> operator--( int ){
  	// return value
  	const_iterator_t<reverse> result( *this );
  	iterator_base<combination const,unsigned long,reverse>::decrement();
  	return result;
      }
      /**
       * Difference type.
       */
      typedef typename iterator_base<combination const,unsigned long,reverse>::difference_type
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
  	return iterator_base<combination const,unsigned long,reverse>::operator-( i );
      }
      /**
       * The default constructor.
       */
      const_iterator_t<reverse>() : iterator_base<combination,unsigned long,reverse>(){}
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
  	// Warn if iterators do not point to same combination
  	if( this->v->ccgsl_pointer != i.v->ccgsl_pointer ){
  	  gsl_error( "trying to take difference of iterators for different combinations", __FILE__, __LINE__,
  		     exception::GSL_EFAILED );
  	  return false;
  	}
  	return reverse ? i.position < this->position : this->position < i.position;
      }
    protected:
      // We need a constructor for combination
      friend class combination;
      /**
       * This constructor allows combination to create non-default iterators.
       * @param v The combination that creates @c this
       * @param position The initial postion of the iterator
       */
      const_iterator_t<reverse>( combination const* v, difference_type position )
      : iterator_base<combination const,unsigned long,reverse>( v, position ){}
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
     * Get iterator pointing to first combination element.
     * @return iterator pointing to first combination element
     */
    const_iterator begin() const {
      return const_iterator( this, 0 );
    }
    // end()
    /**
     * Get iterator pointing beyond last combination element.
     * @return iterator pointing beyond last combination element
     */
    const_iterator end() const {
      if( ccgsl_pointer == 0 ) return const_iterator( this, 0 );
      return const_iterator( this, k() );
    }

  };

}
#endif
