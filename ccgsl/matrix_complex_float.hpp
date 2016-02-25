/*
 * $Id: matrix_complex_float.hpp 313 2014-02-22 14:29:57Z jdl3 $
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

#ifndef CCGSL_MATRIX_COMPLEX_FLOAT_HPP
#define CCGSL_MATRIX_COMPLEX_FLOAT_HPP

#include<gsl/gsl_matrix.h>
#include<new>
#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include<complex>
#endif

#include"exception.hpp"
#include"vector_complex_float.hpp"

// This file is a template
#define CCGSL_MTY 2

namespace gsl {
  /**
   * This class handles matrix_complex_float objects as shared handles. It models a random access container
   * so that STL functions work with matrix_complex_float.
   *
   * Note that matrix_views are implemented as matrix_complex_float objects here.
   *
   * Note that in C++11 it is possible to iterate over the rows of a matrix using
   * @code
   * for( auto row : matrix ){ ... }
   * @endcode
   * Otherwise,
   * @code
   * for( matrix_complex_float::iterator i = matrix.begin(); i != matrix.end(); ++i ){
   *   vector_complex_float row = *i;
   *   ...
   * }
   * @endcode
   * will achieve the same effect. But if the element pointed to by @c i
   * is used more than once, do not use
   * @code
   * i-> ...
   * @endcode
   * because each call of @c operator->() creates a new and different vector_complex.
   */
  class matrix_complex_float {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    matrix_complex_float(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new matrix_complex_float with n1 rows and n2 columns
     * @param n1 The number of rows in the matrix_complex_float
     * @param n2 The number of columns in the matrix_complex_float
     */
    explicit matrix_complex_float( size_t const n1, size_t const n2 ){
      if( n1 > 0 and n2 > 0 ) ccgsl_pointer = gsl_matrix_complex_float_alloc( n1, n2 );
      else { ccgsl_pointer = new gsl_matrix_complex_float; ccgsl_pointer->size1 = n1;
	ccgsl_pointer->size2 = n2; ccgsl_pointer->data = 0; }
      // just plausibly we could allocate matrix but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n1 > 0 and n2 > 0 ) gsl_matrix_complex_float_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_matrix_complex_float. This is not usually a good idea. In this case
     * you should not use gsl_matrix_complex_float_free() to deallocate the memory.
     * @param v The matrix_complex_float
     */
    explicit matrix_complex_float( gsl_matrix_complex_float* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Could construct from a std::initializer_list in C++11
     * @param initializer_list The initializer_list.
     */
    matrix_complex_float( std::initializer_list<std::initializer_list<std::complex<float> > > initializer_list )
      throw( exception ){
      size_t const n1 = initializer_list.size();
      size_t const n2 = initializer_list.begin()->size();
      for( auto l : initializer_list ){
	if( l.size() != n2 ){
	  gsl::exception e( "matrix rows have unequal sizes", __FILE__, __LINE__,
			    exception::GSL_EBADLEN );
	  throw( e );
	}
      }
      ccgsl_pointer = gsl_matrix_complex_float_alloc( n1, n2 );
      // just plausibly we could allocate matrix but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n1 > 0 and n2 > 0 ) gsl_matrix_complex_float_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
      // now copy
      int r = 0;
      for( auto row : initializer_list ){
	size_t c = 0;
	for( auto x : row ){ set( r, c, x ); ++c; }
	++r;
      }
    }
#endif
    // copy constructor
    /**
     * The copy constructor. This shares the matrix_complex_float. Use clone() if you want a full copy.
     * @param v The matrix_complex_float to copy.
     */
    matrix_complex_float( matrix_complex_float const& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( v.count ){
      if( count != 0 ) ++*count; // matrix_complex_float is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The matrix_complex_float to copy
     */
    matrix_complex_float& operator=( matrix_complex_float const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 and ccgsl_pointer->size1 > 0 and ccgsl_pointer->size2 > 0 ) gsl_matrix_complex_float_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	delete count;
      }
      // Then copy
      ccgsl_pointer = v.ccgsl_pointer;
      count = v.count;
      if( count != 0 ) ++*count; // block_complex_float is now shared.
      return *this;
    }
    // clone()
    /**
     * The clone function. Use this if you want a copy of the block_complex_float that does
     * not share the underlying data.
     * @return a new copy of this.
     */
    matrix_complex_float clone() const {
      matrix_complex_float copy( get()->size1, get()->size2 );
      // Now copy
      gsl_matrix_complex_float_memcpy( copy.get(), get() );
      // return new object
      return copy;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~matrix_complex_float(){
      if( count != 0 and --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ){
	  if( ccgsl_pointer->size1 > 0 and ccgsl_pointer->size2 > 0 ) gsl_matrix_complex_float_free( ccgsl_pointer );
	  else delete ccgsl_pointer; }
	delete count;
      }
    }
    /**
     * Stop sharing ownership of the shared pointer.
     */
    void reset(){ matrix_complex_float().swap( *this ); }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The matrix_complex_float to move.
     */
    matrix_complex_float( matrix_complex_float&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The matrix_complex_float to move.
     * @return A reference to this.
     */
    matrix_complex_float& operator=( matrix_complex_float&& v ){
      matrix_complex_float( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
  private:
    /**
     * This is a pointer-like type for iterator return values.
     */
    struct vector_complex_float_ptr : public vector_complex_float {
      /**
       * Typically we have to construct from a vector_complex_float.
       * @param v The vector_complex_float to construct from.
       */
      vector_complex_float_ptr( vector_complex_float const& v ) : vector_complex_float( v ){}
      /**
       * Dereference operator.
       * @return a vector_complex
       */
      vector_complex_float& operator*(){ return *this; }
      /**
       * Dereference operator.
       * @return a pointer to a vector_complex_float
       */
      vector_complex_float* operator->(){ return this; }
    };
    /**
     * We create a suitable class for iterator types
     * here.
     */
    template<typename container,typename content,bool reverse_t> class iterator_base {
      friend class vector_complex_float;
    public:
      /**
       * An iterator must have an iterator category.
       */
      typedef std::bidirectional_iterator_tag iterator_category;
      /**
       * An iterator must have a value type.
       */
      typedef vector_complex_float value_type;
      /**
       * An iterator must have a pointer typea.
       */
      typedef vector_complex_float_ptr pointer;
      /**
       * An iterator must have a reference type.
       */
      typedef value_type reference;
    public:
      // // operator*
      /**
       * Dereference the pointer.
       * @return a reference to content
       */
      reference operator*() const {
  	// Always try to return something
  	static content something;
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "matrix not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// Check that position make sense
  	if( position >= v->size1() ){
  	  gsl_error( "trying to dereference end()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// position and v are valid: return data
	return v->row( position );
      }
      // // operator->
      /**
       * Dereference the pointer.
       * @return a pointer to content
       */
      pointer operator->() const {
  	// Always try to return something
  	static content something_base;
	static vector_complex_float_ptr something( something_base );
  	// First check that iterator is initialised.
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "matrix not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// Check that position make sense
  	if( position >= v->size1() ){
  	  gsl_error( "trying to dereference end()", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return something;
  	}
  	// position and v are valid: return data
  	vector_complex_float_ptr ptr( v->row( position ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  	return std::move( ptr );
#else
  	return ptr;
#endif
      }
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
    protected:
      /**
       * Increment the iterator.
       * @return 0 for success, anything else for failure
       */
      void increment(){
  	// Only makes sense if v points to a matrix
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "matrix not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// increment position and check against size
  	if( reverse_t ){ if( position >= 0 ) --position; }
  	else { if( position < v->size1() ) ++position; }
      }
      /**
       * Decrement the iterator.
       * @return 0 for success, anything else for failure
       */
      void decrement(){
  	// Only makes sense if v points to a matrix
  	if( v == 0 ){
  	  gsl_error( "iterator not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	} else if( v->ccgsl_pointer == 0 ){
  	  gsl_error( "matrix not initialised", __FILE__, __LINE__, exception::GSL_EFAILED );
  	  return;
  	}
  	// decrement position and check against size
  	if( reverse_t ){ if( position < v->size1() ) ++position; }
  	else { if( position >= 0 ) --position; }
      }
      /**
       * The iterator is default constructible.
       */
      iterator_base<container,content,reverse_t>(){ v = 0; }
      /**
       * This constructor allows vector to create non-default iterators.
       * @param v The matrix that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_base<container,content,reverse_t>( container* v, size_t position )
  	: v( v ), position( position ){}
      /**
       * Store a pointer to a matrix we can iterate over: 0 if no matrix.
       */
      container* v;
      /**
       * Mark position of iterator within matrix.
       */
      size_t position;
    };
    // Need to know about const_iterator_t
    template<bool reverse_t> class const_iterator_t;
    /**
     * A class template for the two non-const iterators.
     */
    template<bool reverse_t> class iterator_t : public iterator_base<matrix_complex_float,vector_complex_float,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      iterator_t<reverse_t>& operator=( iterator_t<reverse_t> const& i ){
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::v = i.v;
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator++(){
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t> operator++( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator--(){
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this;
       */
      iterator_t<reverse_t> operator--( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<matrix_complex_float,vector_complex_float,reverse_t>::decrement();
  	return result;
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
       * The default constructor.
       */
      iterator_t<reverse_t>() : iterator_base<matrix_complex_float,vector_complex_float,reverse_t>(){}
    protected:
      friend class matrix_complex_float;
      // We need a constructor for matrix_complex_float
      /**
       * This constructor allows vector to create non-default iterators.
       * @param v The matrix_complex_float that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_t<reverse_t>( matrix_complex_float* v, size_t position )
      : iterator_base<matrix_complex_float,vector_complex_float,reverse_t>( v, position ){}
    };
    /**
     * A class template for the const iterators.
     */
    template<bool reverse_t> class const_iterator_t
      : public iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      const_iterator_t<reverse_t>& operator=( const_iterator_t<reverse_t> const& i ){
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::v = i.v;
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator++(){
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator++( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator--(){
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator--( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>::decrement();
  	return result;
      }
      /**
       * The default constructor.
       */
      const_iterator_t<reverse_t>() : iterator_base<matrix_complex_float,vector_complex_float,reverse_t>(){}
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
    protected:
      // We need a constructor for matrix_complex_float
      friend class matrix_complex_float;
      /**
       * This constructor allows vector to create non-default iterators.
       * @param v The vector_complex_float that creates @c this
       * @param position The initial postion of the iterator
       */
      const_iterator_t<reverse_t>( matrix_complex_float const* v, size_t position )
      : iterator_base<matrix_complex_float const,vector_complex_float const,reverse_t>( v, position ){}
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
    /**
     * A container must have a size_type.
     */
    typedef size_t size_type;
    // begin()
    /**
     * Get iterator pointing to first vector_complex_float element.
     * @return iterator pointing to first vector_complex_float element
     */
    iterator begin(){
      return iterator( this, 0 );
    }
    /**
     * Get iterator pointing to first vector_complex_float element.
     * @return iterator pointing to first vector_complex_float element
     */
    const_iterator begin() const {
      return const_iterator( this, 0 );
    }
    // end()
    /**
     * Get iterator pointing beyond last vector_complex_float element.
     * @return iterator pointing beyond last vector_complex_float element
     */
    iterator end(){
      if( ccgsl_pointer == 0 ) return iterator( this, 0 );
      return iterator( this, size1() );
    }
    /**
     * Get iterator pointing beyond last vector_complex_float element.
     * @return iterator pointing beyond last vector_complex_float element
     */
    const_iterator end() const {
      if( ccgsl_pointer == 0 ) return const_iterator( this, 0 );
      return const_iterator( this, size1() );
    }
    // rbegin()
    /**
     * Get iterator pointing to first vector_complex_float element.
     * @return iterator pointing to first vector_complex_float element
     */
    reverse_iterator rbegin(){
      if( ccgsl_pointer ==0 ) return reverse_iterator( this, 0 );
      return reverse_iterator( this, size1() - 1 );
    }
    /**
     * Get iterator pointing to first vector_complex_float element.
     * @return iterator pointing to first vector_complex_float element
     */
    const_reverse_iterator rbegin() const {
      if( ccgsl_pointer == 0 ) return const_reverse_iterator( this, 0 );
      return const_reverse_iterator( this, size1() - 1 );
    }
    // rend()
    /**
     * Get iterator pointing beyond last vector_complex_float element.
     * @return iterator pointing beyond last vector_complex_float element
     */
    reverse_iterator rend(){
      return reverse_iterator( this, -1 );
    }
    /**
     * Get iterator pointing beyond last vector_complex_float element.
     * @return iterator pointing beyond last vector_complex_float element
     */
    const_reverse_iterator rend() const {
      return const_reverse_iterator( this, -1 );
    }
  public:
    // Sizes
    /**
     * The number of rows of the matrix_complex_float
     * @return The number of rows of the matrix_complex_float
     */
    size_t size1() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size1; }
    /**
     * The number of columns of the matrix_complex_float
     * @return The number of columns of the matrix_complex_float
     */
    size_t size2() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size2; }
    /**
     * Swap two matrix_complex_float objects. This works even if the matrix_complex_float objects have different sizes
     * because it swaps pointers.
     * @param m The matrix_complex_float to swap with @c this.
     */
    void swap( matrix_complex_float& m ){
      gsl_matrix_complex_float* tmp = ccgsl_pointer; ccgsl_pointer = m.ccgsl_pointer; m.ccgsl_pointer = tmp;
      size_t* tmp2 = count; count = m.count; m.count = tmp2;
    }
    // view operations
    /**
     * C++ version of gsl_matrix_complex_float_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix_complex_float submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_row().
     * @param i A row index
     * @return A row as a vector_complex_float
     */
    vector_complex_float row( size_t const i ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_row( get(), i ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_column().
     * @param j A column index
     * @return A column as a vector_complex_float
     */
    vector_complex_float column( size_t const j ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_column( get(), j ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_diagonal().
     * @return The principal diagonal as a vector_complex_float
     */
    vector_complex_float
    diagonal(){ gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_diagonal( get() ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float subdiagonal( size_t const k ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_subdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float superdiagonal( size_t const k ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_superdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector_complex_float
     */
    vector_complex_float subrow( size_t const i, size_t const offset, size_t const n ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_subrow( get(), i, offset, n ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector_complex_float
     */
    vector_complex_float subcolumn( size_t const j, size_t const offset, size_t const n ){
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_subcolumn( get(), j, offset, n ).vector;
      return vector_complex_float( w );
    }
   /**
     * C++ version of gsl_matrix_complex_float_view_array().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float view_array( float* base, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_view_array( base, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_view_array_with_tda().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float view_array_with_tda( float* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_view_vector().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float view_vector( vector_complex_float& v, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_view_vector( v.get(), n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_view_vector_with_tda().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float view_vector_with_tda( vector_complex_float& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
    // const versions ...
    /**
     * C++ version of gsl_matrix_complex_float_const_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix_complex_float const const_submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ) const {
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_row().
     * @param i A row index
     * @return A row as a vector_complex_float
     */
    vector_complex_float const const_row( size_t const i ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_row( get(), i ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_column().
     * @param j A column index
     * @return A column as a vector_complex_float
     */
    vector_complex_float const const_column( size_t const j ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_column( get(), j ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_diagonal().
     * @return The principal diagonal as a vector_complex_float
     */
    vector_complex_float const const_diagonal() const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_diagonal( get() ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float const const_subdiagonal( size_t const k ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float const const_superdiagonal( size_t const k ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_superdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector_complex_float
     */
    vector_complex_float const const_subrow( size_t const i, size_t const offset, size_t const n ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subrow( get(), i, offset, n ).vector;
      return vector_complex_float( w );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector_complex_float
     */
    vector_complex_float const const_subcolumn( size_t const j, size_t const offset, size_t const n ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subcolumn( get(), j, offset, n ).vector;
      return vector_complex_float( w );
    }
   /**
     * C++ version of gsl_matrix_complex_float_const_view_array().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float const const_view_array( float const* base, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_view_array( base, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_view_array_with_tda().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float const
    const_view_array_with_tda( float const* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_const_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_view_vector().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float const const_view_vector( vector_complex_float const& v, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_const_view_vector( v.get(), n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * C++ version of gsl_matrix_complex_float_const_view_vector_with_tda().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float const
    const_view_vector_with_tda( vector_complex_float const& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix_complex_float const submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ) const {
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_row().
     * @param i A row index
     * @return A row as a vector_complex_float
     */
    vector_complex_float const row( size_t const i ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_row( get(), i ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_column().
     * @param j A column index
     * @return A column as a vector_complex_float
     */
    vector_complex_float const column( size_t const j ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_column( get(), j ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_diagonal().
     * @return The principal diagonal as a vector_complex_float
     */
    vector_complex_float const diagonal() const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_diagonal( get() ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float const subdiagonal( size_t const k ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector_complex_float
     */
    vector_complex_float const superdiagonal( size_t const k ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_superdiagonal( get(), k ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector_complex_float
     */
    vector_complex_float const subrow( size_t const i, size_t const offset, size_t const n ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subrow( get(), i, offset, n ).vector;
      return vector_complex_float( w );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector_complex_float
     */
    vector_complex_float const subcolumn( size_t const j, size_t const offset, size_t const n ) const {
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_const_subcolumn( get(), j, offset, n ).vector;
      return vector_complex_float( w );
    }
   /**
     * Another C++ version of gsl_matrix_complex_float_const_view_array().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float const view_array( float const* base, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_view_array( base, n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_view_array_with_tda().
     * @param base An array of type float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float const
    view_array_with_tda( float const* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_const_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_view_vector().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix_complex_float
     */
    static matrix_complex_float const view_vector( vector_complex_float const& v, size_t const n1, size_t const n2 ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m =  gsl_matrix_complex_float_const_view_vector( v.get(), n1, n2 ).matrix;
      return matrix_complex_float( m );
    }
    /**
     * Another C++ version of gsl_matrix_complex_float_const_view_vector_with_tda().
     * @param v A vector_complex_float
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix_complex_float
     */
    static matrix_complex_float const
    view_vector_with_tda( vector_complex_float const& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix_complex_float* m = static_cast<gsl_matrix_complex_float*>( malloc( sizeof( gsl_matrix_complex_float ) ) );
      *m = gsl_matrix_complex_float_const_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix_complex_float( m );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_matrix_complex_float* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_matrix_complex_float.
     * @return the gsl_matrix_complex_float
     */
    gsl_matrix_complex_float* get(){ return ccgsl_pointer; }
    /**
     * Get the gsl_matrix_complex_float.
     * @return the gsl_matrix_complex_float
     */
    gsl_matrix_complex_float const* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_matrix_complex_float.
     * @return @c true or @c falses according as 
     * this is the only matrix_complex_float object sharing the gsl_matrix_complex_float
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many matrix_complex_float objects share this pointer.
     * @return the number of matrix_complex_float objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_matrix_complex_float
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
    // GSL functions
    /**
     * C++ version of gsl_matrix_complex_float_calloc(). This constructs a matrix_complex_float object with entries
     * initialised to zero.
     * @param n1 The number of rows in the matrix_complex_float
     * @param n2 The number of columns in the matrix_complex_float
     * @return A matrix_complex_float initialised to zero
     */
    static matrix_complex_float calloc( size_t const n1, size_t const n2 ){ return matrix_complex_float( gsl_matrix_complex_float_calloc( n1, n2 ) ); }
    /**
     * C++ version of gsl_matrix_complex_float_set_zero().
     */
    void set_zero(){ gsl_matrix_complex_float_set_zero( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_set_all().
     * @param x The value to which all elements are set
     */
    void set_all( complex_float x ){ gsl_matrix_complex_float_set_all( get(), x ); }
    /**
     * C++ version of gsl_matrix_complex_float_memcpy().
     * @param src source matrix_complex_float
     * @return error code on failure
     */
    int memcpy( matrix_complex_float const& src ){ return gsl_matrix_complex_float_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_add().
     * @param b matrix_complex_float to add to this
     * @return error code on failure
     */
    int add( matrix_complex_float const& b ){ return gsl_matrix_complex_float_add( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_sub().
     * @param b matrix_complex_float to subtract from this
     * @return error code on failure
     */
    int sub( matrix_complex_float const& b ){ return gsl_matrix_complex_float_sub( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_scale().
     * @param x constant to multiply this by
     * @return error code on failure
     */
    int scale( complex_float const x ){ return gsl_matrix_complex_float_scale( get(), x ); }
    /**
     * C++ version of gsl_matrix_complex_float_add_constant().
     * @param x constant to add to each element of this
     * @return error code on failure
     */
    int add_constant( complex_float const x ){ return gsl_matrix_complex_float_add_constant( get(), x ); }
    /**
     * C++ version of gsl_matrix_complex_float_isnull().
     * @return @c +1 or @c 0 according as elements are all zero or not
     */
    int isnull() const { return gsl_matrix_complex_float_isnull( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_ispos().
     * @return @c +1 or @c 0 according as elements are all positive or not
     */
    int ispos() const { return gsl_matrix_complex_float_ispos( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_isneg().
     * @return @c +1 or @c 0 according as elements are all negative or not
     */
    int isneg() const { return gsl_matrix_complex_float_isneg( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_isnonneg().
     * @return @c +1 or @c 0 according as elements are all nonnegative or not
     */
    int isnonneg() const { return gsl_matrix_complex_float_isnonneg( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_get().
     * @param i index of row
     * @param j index of column
     * @return value of element
     */
    complex_float get( size_t const i, size_t const j ) const { return gsl_matrix_complex_float_get( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_complex_float_set().
     * @param i index of row
     * @param j index of column
     * @param x new value for element
     */
    void set( size_t const i, size_t const j, complex_float x ){ gsl_matrix_complex_float_set( get(), i, j, x ); }
    /**
     * C++ version of gsl_matrix_complex_float_ptr().
     * @param i index of row
     * @param j index of column
     * @return pointer to element
     */
    complex_float_ptr ptr( size_t const i, size_t const j ){
      if( i >= ccgsl_pointer->size1 )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      if( j >= ccgsl_pointer->size2 )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      return complex_float_ptr( get()->data + CCGSL_MTY * (i * get()->tda + j ) ); }
    /**
     * C++ version of gsl_matrix_complex_float_const_ptr().
     * @param i index of row
     * @param j index of column
     * @return pointer to element
     */
    complex_float_ptr const const_ptr( size_t const i, size_t const j ) const {
      if( i >= ccgsl_pointer->size1 )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      if( j >= ccgsl_pointer->size2 )
	gsl_error( "Index out of range", __FILE__, __LINE__, exception::GSL_EINVAL );
      return complex_float_ptr( get()->data + CCGSL_MTY * (i * get()->tda + j ) ); }
    /**
     * C++ version of gsl_matrix_complex_float_fread().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fread( FILE* stream ){ return gsl_matrix_complex_float_fread( stream, get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_fwrite().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_matrix_complex_float_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_fscanf().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_matrix_complex_float_fscanf( stream, get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_fprintf().
     * @param stream A C file stream
     * @param format %d, %e, %f or %g
     * @return error code on failure
     */
    int fprintf( FILE* stream, char const* format ) const {
      return gsl_matrix_complex_float_fprintf( stream, get(), format ); }
    /**
     * C++ version of gsl_matrix_complex_float_alloc_from_block().
     * @param b The block_complex_float
     * @param offset The offset within the block_complex_float
     * @param n1 The number of rows in the matrix_complex_float
     * @param n2 The number of columns in the matrix_complex_float
     * @param d2 undocumented
     */
    matrix_complex_float( block_complex_float& b, size_t const offset, size_t const n1, size_t const n2, size_t const d2 ){
      ccgsl_pointer = gsl_matrix_complex_float_alloc_from_block( b.get(), offset, n1, n2, d2 );
      // just plausibly we could allocate vector_complex_float but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
    	// try to tidy up before rethrowing
    	gsl_matrix_complex_float_free( ccgsl_pointer );
    	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * C++ version of gsl_matrix_complex_float_alloc_from_matrix().
     * @param m The matrix_complex_float
     * @param k1 the row of @c m to take as row zero
     * @param k2 the column of @c m to take as column zero
     * @param n1 The number of rows in the matrix_complex_float
     * @param n2 The number of columns in the matrix_complex_float
     */
    matrix_complex_float( matrix_complex_float& m, size_t const k1, size_t const k2, size_t const n1, size_t const n2 ){
      ccgsl_pointer = gsl_matrix_complex_float_alloc_from_matrix( m.get(), k1, k2, n1, n2 );
      // just plausibly we could allocate matrix_complex_float but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
    	// try to tidy up before rethrowing
    	gsl_matrix_complex_float_free( ccgsl_pointer );
    	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // More functions
    /**
     * C++ version of gsl_matrix_complex_float_set_identity().
     */
    void set_identity(){ gsl_matrix_complex_float_set_identity( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_swap_rows().
     * @param i Index of first row
     * @param j Index of second row
     * @return error code on failure
     */
    int swap_rows( size_t const i, size_t const j ){ return gsl_matrix_complex_float_swap_rows( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_complex_float_swap_columns().
     * @param i Index of first column
     * @param j Index of second column
     * @return error code on failure
     */
    int swap_columns( size_t const i, size_t const j ){
      return gsl_matrix_complex_float_swap_columns( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_complex_float_swap_rowcol(). Swap row and column in place.
     * Matrix must be square.
     * @param i index of row
     * @param j index of column
     * @return error code on failure
     */
    int swap_rowcol( size_t const i, size_t const j ){ return gsl_matrix_complex_float_swap_rowcol( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_complex_float_transpose().
     * @return error code on failure.
     */
    int transpose(){ return gsl_matrix_complex_float_transpose( get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_transpose_memcpy().
     * @param src matrix_complex_float whose transpose it to be copied to @c this
     * @return error code on failure
     */
    int transpose_memcpy( matrix_complex_float const& src ){
      return gsl_matrix_complex_float_transpose_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_mul_elements(). Multiply matrices elementwise.
     * @param b Another matrix_complex_float
     * @return error code on failure
     */
    int
    mul_elements( matrix_complex_float const& b ){
          return gsl_matrix_complex_float_mul_elements( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_div_elements(). Divide each element of @c this by the
     * corrsponding element of @c b
     * @param b Another matrix_complex_float
     * @return error code on failure
     */
    int div_elements( matrix_complex_float const& b ){
      return gsl_matrix_complex_float_div_elements( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_add_diagonal().
     * @param x A constant
     * @return error code on failure
     */
    int add_diagonal( complex_float const x ){
      return gsl_matrix_complex_float_add_diagonal( get(), x ); }
    /**
     * C++ version of gsl_matrix_complex_float_get_row().
     * @param v A vector_complex_float
     * @param i The index of the row
     * @return error code on failure
     */
    int get_row( vector_complex_float& v, size_t const i ) const {
      return gsl_matrix_complex_float_get_row( v.get(), get(), i ); }
    /**
     * C++ version of gsl_matrix_complex_float_get_col().
     * @param v A vector_complex_float
     * @param j The index of the column
     * @return error code on failure
     */
    int get_col( vector_complex_float& v, size_t const j ) const {
      return gsl_matrix_complex_float_get_col( v.get(), get(), j ); }
    /**
     * C++ version of gsl_matrix_complex_float_set_row().
     * @param i The index of the row
     * @param v A vector_complex_float
     * @return error code on failure
     */
    int set_row( size_t const i, vector_complex_float const& v ){
      return gsl_matrix_complex_float_set_row( get(), i, v.get() ); }
    /**
     * C++ version of gsl_matrix_complex_float_set_col().
     * @param j The index of the column
     * @param v A vector_complex_float
     * @return error code on failure
     */
    int set_col( size_t const j, vector_complex_float const& v ){
      return gsl_matrix_complex_float_set_col( get(), j, v.get() ); }
    // Extra functions for []
    /**
     * This function allows us to use a matrix_complex_float like an array. Use with caution.
     * Although @c matrix_complex_float[i][j] is possible, it is much less efficient than
     * matrix_complex_float::set(). The effect is the same as row()
     * @param i The index of the row
     * @return A vector_complex_float representing a row
     */
    vector_complex_float operator[]( size_t const i ){
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "matrix_complex_float is null", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return vector_complex_float();
      }
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_row( ccgsl_pointer, i ).vector;
      return vector_complex_float( w );
    }
    /**
     * This function allows us to use a matrix_complex_float like an array. Use with caution.
     * Although @c matrix_complex_float[i][j] is possible, it is much less efficient than
     * matrix_complex_float::set(). The effect is the same as row()
     * @param i The index of the row
     * @return A vector_complex_float representing a row
     */
    vector_complex_float const operator[]( size_t const i ) const {
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "matrix_complex_float is null", __FILE__, __LINE__, exception::GSL_EFAILED );
  	return vector_complex_float();
      }
      gsl_vector_complex_float* w = static_cast<gsl_vector_complex_float*>( malloc( sizeof( gsl_vector_complex_float ) ) );
      *w = gsl_matrix_complex_float_row( ccgsl_pointer, i ).vector;
      return vector_complex_float( w );
    }
  };

  // Extra functions for vector_complex_float allocation from matrix_complex_float objects
  inline vector_complex_float vector_complex_float::alloc_row_from_matrix( matrix_complex_float& m, size_t const i ){
    return vector_complex_float ( gsl_vector_complex_float_alloc_row_from_matrix( m.get(), i ) ); }
  inline vector_complex_float vector_complex_float::alloc_col_from_matrix( matrix_complex_float& m, size_t const i ){
    return vector_complex_float ( gsl_vector_complex_float_alloc_col_from_matrix( m.get(), i ) ); }
}
#undef CCGSL_MTY
#endif
