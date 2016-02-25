/*
 * $Id: matrix.hpp 313 2014-02-22 14:29:57Z jdl3 $
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

#ifndef CCGSL_MATRIX_HPP
#define CCGSL_MATRIX_HPP

#include<gsl/gsl_matrix.h>
#include<new>

#include"exception.hpp"
#include"vector.hpp"

// This file is used as a template

namespace gsl {
  /**
   * This class handles matrix objects as shared handles. It models a random access container
   * so that STL functions work with matrix.
   *
   * Note that matrix_views are implemented as matrix objects here.
   *
   * Note that in C++11 it is possible to iterate over the rows of a matrix using
   * @code
   * for( auto row : matrix ){ ... }
   * @endcode
   * Otherwise,
   * @code
   * for( matrix::iterator i = matrix.begin(); i != matrix.end(); ++i ){
   *   vector row = *i;
   *   ...
   * }
   * @endcode
   * will achieve the same effect. But if the element pointed to by @c i
   * is used more than once, do not use
   * @code
   * i-> ...
   * @endcode
   * because each call of @c operator->() creates a new and different vector.
   */
  class matrix {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    matrix(){
      ccgsl_pointer = 0;
      count = 0; // initially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * This constructor creates a new matrix with n1 rows and n2 columns
     * @param n1 The number of rows in the matrix
     * @param n2 The number of columns in the matrix
     */
    explicit matrix( size_t const n1, size_t const n2 ){
      if( n1 > 0 and n2 > 0 ) ccgsl_pointer = gsl_matrix_alloc( n1, n2 );
      else { ccgsl_pointer = new gsl_matrix; ccgsl_pointer->size1 = n1;
	ccgsl_pointer->size2 = n2; ccgsl_pointer->data = 0; }
      // just plausibly we could allocate matrix but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n1 > 0 and n2 > 0 ) gsl_matrix_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_matrix. This is not usually a good idea. In this case
     * you should not use gsl_matrix_free() to deallocate the memory.
     * @param v The matrix
     */
    explicit matrix( gsl_matrix* v ){
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
    matrix( std::initializer_list<std::initializer_list<double> > initializer_list )
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
      ccgsl_pointer = gsl_matrix_alloc( n1, n2 );
      // just plausibly we could allocate matrix but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	if( n1 > 0 and n2 > 0 ) gsl_matrix_free( ccgsl_pointer );
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
     * The copy constructor. This shares the matrix. Use clone() if you want a full copy.
     * @param v The matrix to copy.
     */
    matrix( matrix const& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( v.count ){
      if( count != 0 ) ++*count; // matrix is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The matrix to copy
     */
    matrix& operator=( matrix const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 and ccgsl_pointer->size1 > 0 and ccgsl_pointer->size2 > 0 ) gsl_matrix_free( ccgsl_pointer );
	else delete ccgsl_pointer;
	delete count;
      }
      // Then copy
      ccgsl_pointer = v.ccgsl_pointer;
      count = v.count;
      if( count != 0 ) ++*count; // block is now shared.
      return *this;
    }
    // clone()
    /**
     * The clone function. Use this if you want a copy of the block that does
     * not share the underlying data.
     * @return a new copy of this.
     */
    matrix clone() const {
      matrix copy( get()->size1, get()->size2 );
      // Now copy
      gsl_matrix_memcpy( copy.get(), get() );
      // return new object
      return copy;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~matrix(){
      if( count != 0 and --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ){
	  if( ccgsl_pointer->size1 > 0 and ccgsl_pointer->size2 > 0 )
	    gsl_matrix_free( ccgsl_pointer );
	  else delete ccgsl_pointer; }
	delete count;
      }
    }

    // Allow possibility of assigning from gsl_matrix without sharing
    /**
     * This function is intended mainly for internal use. It allows @c this
     * to point to a gsl_matrix without the possibility deleting it when @c this
     * is no longer in scope. It is the responsibility of the programmer to delete
     * v. The function is used internally for converting a function that takes a
     * gsl::matrix* argument to one that takes a gsl_matrix* argument.
     * @param v The gsl_matrix
     */
    void wrap_gsl_matrix_without_ownership( gsl_matrix* v ){
      if( count != 0 and --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ){
	  if( ccgsl_pointer->size1 != 0 and ccgsl_pointer->size1 != 0 )
	    gsl_matrix_free( ccgsl_pointer );
	  else delete ccgsl_pointer; }
	delete count;
	count = 0;
      }
      ccgsl_pointer = v;
    }
    /**
     * Stop sharing ownership of the shared pointer.
     */
    void reset(){ matrix().swap( *this ); }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The matrix to move.
     */
    matrix( matrix&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The matrix to move.
     * @return A reference to this.
     */
    matrix& operator=( matrix&& v ){
      matrix( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
  private:
    /**
     * This is a pointer-like type for iterator return values.
     */
    struct vector_ptr : public vector {
      /**
       * Typically we have to construct from a vector.
       * @param v The vector to construct from.
       */
      vector_ptr( vector const& v ) : vector( v ){}
      /**
       * Dereference operator.
       * @return a vector
       */
      vector& operator*(){ return *this; }
      /**
       * Dereference operator.
       * @return a pointer to a vector
       */
      vector* operator->(){ return this; }
    };
    /**
     * We create a suitable class for iterator types
     * here.
     */
    template<typename container,typename content,bool reverse_t> class iterator_base {
      friend class vector;
    public:
      /**
       * An iterator must have an iterator category.
       */
      typedef std::bidirectional_iterator_tag iterator_category;
      /**
       * An iterator must have a value type.
       */
      typedef vector value_type;
      /**
       * An iterator must have a pointer typea.
       */
      typedef vector_ptr pointer;
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
	static vector_ptr something( something_base );
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
  	vector_ptr ptr( v->row( position ) );
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
    template<bool reverse_t> class iterator_t : public iterator_base<matrix,vector,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      iterator_t<reverse_t>& operator=( iterator_t<reverse_t> const& i ){
  	iterator_base<matrix,vector,reverse_t>::v = i.v;
  	iterator_base<matrix,vector,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator++(){
  	iterator_base<matrix,vector,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      iterator_t<reverse_t> operator++( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<matrix,vector,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      iterator_t<reverse_t>& operator--(){
  	iterator_base<matrix,vector,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this;
       */
      iterator_t<reverse_t> operator--( int ){
  	// return value
  	iterator_t<reverse_t> result( *this );
  	iterator_base<matrix,vector,reverse_t>::decrement();
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
      iterator_t<reverse_t>() : iterator_base<matrix,vector,reverse_t>(){}
    protected:
      friend class matrix;
      // We need a constructor for matrix
      /**
       * This constructor allows vector to create non-default iterators.
       * @param v The matrix that creates @c this
       * @param position The initial postion of the iterator
       */
      iterator_t<reverse_t>( matrix* v, size_t position )
      : iterator_base<matrix,vector,reverse_t>( v, position ){}
    };
    /**
     * A class template for the const iterators.
     */
    template<bool reverse_t> class const_iterator_t
      : public iterator_base<matrix const,vector const,reverse_t>{
    public:
      // // Refines output iterator
      // // operator=
      /**
       * We can assign one output iterator from another.
       * @param i The iterator to copy
       */
      const_iterator_t<reverse_t>& operator=( const_iterator_t<reverse_t> const& i ){
  	iterator_base<matrix const,vector const,reverse_t>::v = i.v;
  	iterator_base<matrix const,vector const,reverse_t>::position = i.position;
  	return *this;
      }
      // // Refines forward iterator
      // // operator++ (both)
      /**
       * The prefix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator++(){
  	iterator_base<matrix const,vector const,reverse_t>::increment();
  	return *this;
      }
      /**
       * The postfix ++ operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator++( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<matrix const,vector const,reverse_t>::increment();
  	return result;
      }
      // // Refines bidirectional iterator
      // // operator-- (both)
      /**
       * The prefix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t>& operator--(){
  	iterator_base<matrix const,vector const,reverse_t>::decrement();
  	return *this;
      }
      /**
       * The postfix -- operator.
       * @return @c this
       */
      const_iterator_t<reverse_t> operator--( int ){
  	// return value
  	const_iterator_t<reverse_t> result( *this );
  	iterator_base<matrix const,vector const,reverse_t>::decrement();
  	return result;
      }
      /**
       * The default constructor.
       */
      const_iterator_t<reverse_t>() : iterator_base<matrix,vector,reverse_t>(){}
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
      // We need a constructor for matrix
      friend class matrix;
      /**
       * This constructor allows vector to create non-default iterators.
       * @param v The vector that creates @c this
       * @param position The initial postion of the iterator
       */
      const_iterator_t<reverse_t>( matrix const* v, size_t position )
      : iterator_base<matrix const,vector const,reverse_t>( v, position ){}
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
     * Get iterator pointing to first vector element.
     * @return iterator pointing to first vector element
     */
    iterator begin(){
      return iterator( this, 0 );
    }
    /**
     * Get iterator pointing to first vector element.
     * @return iterator pointing to first vector element
     */
    const_iterator begin() const {
      return const_iterator( this, 0 );
    }
    // end()
    /**
     * Get iterator pointing beyond last vector element.
     * @return iterator pointing beyond last vector element
     */
    iterator end(){
      if( ccgsl_pointer == 0 ) return iterator( this, 0 );
      return iterator( this, size1() );
    }
    /**
     * Get iterator pointing beyond last vector element.
     * @return iterator pointing beyond last vector element
     */
    const_iterator end() const {
      if( ccgsl_pointer == 0 ) return const_iterator( this, 0 );
      return const_iterator( this, size1() );
    }
    // rbegin()
    /**
     * Get iterator pointing to first vector element.
     * @return iterator pointing to first vector element
     */
    reverse_iterator rbegin(){
      if( ccgsl_pointer ==0 ) return reverse_iterator( this, 0 );
      return reverse_iterator( this, size1() - 1 );
    }
    /**
     * Get iterator pointing to first vector element.
     * @return iterator pointing to first vector element
     */
    const_reverse_iterator rbegin() const {
      if( ccgsl_pointer == 0 ) return const_reverse_iterator( this, 0 );
      return const_reverse_iterator( this, size1() - 1 );
    }
    // rend()
    /**
     * Get iterator pointing beyond last vector element.
     * @return iterator pointing beyond last vector element
     */
    reverse_iterator rend(){
      return reverse_iterator( this, -1 );
    }
    /**
     * Get iterator pointing beyond last vector element.
     * @return iterator pointing beyond last vector element
     */
    const_reverse_iterator rend() const {
      return const_reverse_iterator( this, -1 );
    }
  public:
    // Sizes
    /**
     * The number of rows of the matrix
     * @return The number of rows of the matrix
     */
    size_t size1() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size1; }
    /**
     * The number of columns of the matrix
     * @return The number of columns of the matrix
     */
    size_t size2() const { return ccgsl_pointer == 0 ? 0 : ccgsl_pointer->size2; }
    /**
     * Give access to the data block. The data() and size() functions mimic the functions
     * of std::array<T> and std::vector<T>. This function can throw an exception or produce
     * a GSL error if the matrix size2 and tda are not equal.
     * However, new matrix objects are always initialised to have tda and size2 equal.
     * @return The data block.
     */
    double* data() { 
      if( ccgsl_pointer == 0 ) gsl_error( "null vector", __FILE__, __LINE__, GSL_EFAULT );
#ifndef GSL_RANGE_CHECK_OFF
      if( ccgsl_pointer->size2 != ccgsl_pointer->tda )
	gsl_error( "matrix size2 and tda do not match", __FILE__, __LINE__, GSL_EBADLEN );
#endif
      return ccgsl_pointer->data; }
    /**
     * Give access to the data block. The data() and size() functions mimic the functions
     * of std::array<T> and std::vector<T>. This function can throw an exception or produce
     * a GSL error if the matrix size1 and tda are not equal.
     * However, new matrix objects are always initialised to have tda and size2 equal.
     * @return The data block.
     */
    double const* data() const {
      if( ccgsl_pointer == 0 ) gsl_error( "null vector", __FILE__, __LINE__, GSL_EFAULT );
#ifndef GSL_RANGE_CHECK_OFF
      if( ccgsl_pointer->size2 != ccgsl_pointer->tda )
	gsl_error( "matrix size2 and tda do not match", __FILE__, __LINE__, GSL_EBADLEN );
#endif
      return ccgsl_pointer->data; }
    /**
     * Swap two matrix objects. This works even if the matrix objects have different sizes
     * because it swaps pointers.
     * @param m The matrix to swap with @c this.
     */
    void swap( matrix& m ){
      std::swap( ccgsl_pointer, m.ccgsl_pointer );
      std::swap( count, m.count );
    }
    // view operations
    /**
     * C++ version of gsl_matrix_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_row().
     * @param i A row index
     * @return A row as a vector
     */
    vector row( size_t const i ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_row( get(), i ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_column().
     * @param j A column index
     * @return A column as a vector
     */
    vector column( size_t const j ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_column( get(), j ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_diagonal().
     * @return The principal diagonal as a vector
     */
    vector
    diagonal(){ gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_diagonal( get() ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector subdiagonal( size_t const k ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_subdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector superdiagonal( size_t const k ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_superdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector
     */
    vector subrow( size_t const i, size_t const offset, size_t const n ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_subrow( get(), i, offset, n ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector
     */
    vector subcolumn( size_t const j, size_t const offset, size_t const n ){
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_subcolumn( get(), j, offset, n ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_view_array().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix view_array( double* base, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_view_array( base, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_view_array_with_tda().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix view_array_with_tda( double* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_view_vector().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix view_vector( vector& v, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_view_vector( v.get(), n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_view_vector_with_tda().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix view_vector_with_tda( vector& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix( m );
    }
    // const versions ...
    /**
     * C++ version of gsl_matrix_const_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix const const_submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ) const {
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_const_row().
     * @param i A row index
     * @return A row as a vector
     */
    vector const const_row( size_t const i ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_row( get(), i ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_column().
     * @param j A column index
     * @return A column as a vector
     */
    vector const const_column( size_t const j ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_column( get(), j ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_diagonal().
     * @return The principal diagonal as a vector
     */
    vector const const_diagonal() const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_diagonal( get() ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector const const_subdiagonal( size_t const k ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector const const_superdiagonal( size_t const k ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_superdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector
     */
    vector const const_subrow( size_t const i, size_t const offset, size_t const n ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subrow( get(), i, offset, n ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector
     */
    vector const const_subcolumn( size_t const j, size_t const offset, size_t const n ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subcolumn( get(), j, offset, n ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_submatrix().
     * @param i Index in @c this of first row of submatrix
     * @param j Index in @c this of first column of submatrix
     * @param n1 Number of rows of submatrix
     * @param n2 Number of columns of submatrix
     * @return The submatrix
     */
    matrix const submatrix( size_t const i, size_t const j, size_t const n1, size_t const n2 ) const {
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_submatrix( get(), i, j, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * Another C++ version of gsl_matrix_const_row().
     * @param i A row index
     * @return A row as a vector
     */
    vector const row( size_t const i ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_row( get(), i ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_column().
     * @param j A column index
     * @return A column as a vector
     */
    vector const column( size_t const j ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_column( get(), j ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_diagonal().
     * @return The principal diagonal as a vector
     */
    vector const diagonal() const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_diagonal( get() ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_subdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector const subdiagonal( size_t const k ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_superdiagonal().
     * @param k An index
     * @return Subdiagonal @c k as a vector
     */
    vector const superdiagonal( size_t const k ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_superdiagonal( get(), k ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_subrow().
     * @param i A row index
     * @param offset A column offset
     * @param n The number of elements
     * @return A subrow as a vector
     */
    vector const subrow( size_t const i, size_t const offset, size_t const n ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subrow( get(), i, offset, n ).vector;
      return vector( w );
    }
    /**
     * Another C++ version of gsl_matrix_const_subcolumn().
     * @param j A column index
     * @param offset A row offset
     * @param n The number of elements
     * @return A subcolumn as a vector
     */
    vector const subcolumn( size_t const j, size_t const offset, size_t const n ) const {
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_const_subcolumn( get(), j, offset, n ).vector;
      return vector( w );
    }
    /**
     * C++ version of gsl_matrix_const_view_array().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix const const_view_array( double const* base, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_view_array( base, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_const_view_array_with_tda().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix const
    const_view_array_with_tda( double const* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_const_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_const_view_vector().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix const const_view_vector( vector const& v, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_const_view_vector( v.get(), n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * C++ version of gsl_matrix_const_view_vector_with_tda().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix const
    const_view_vector_with_tda( vector const& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix( m );
    }
    /**
     * Another C++ version of gsl_matrix_const_view_array().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix const view_array( double const* base, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_view_array( base, n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * Another C++ version of gsl_matrix_const_view_array_with_tda().
     * @param base An array of type double
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix const
    view_array_with_tda( double const* base, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_const_view_array_with_tda( base, n1, n2, tda ).matrix;
      return matrix( m );
    }
    /**
     * Another C++ version of gsl_matrix_const_view_vector().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @return A matrix
     */
    static matrix const view_vector( vector const& v, size_t const n1, size_t const n2 ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m =  gsl_matrix_const_view_vector( v.get(), n1, n2 ).matrix;
      return matrix( m );
    }
    /**
     * Another C++ version of gsl_matrix_const_view_vector_with_tda().
     * @param v A vector
     * @param n1 The number of rows
     * @param n2 The number of columns
     * @param tda The number of columns in memory
     * @return A matrix
     */
    static matrix const
    view_vector_with_tda( vector const& v, size_t const n1, size_t const n2, size_t const tda ){
      gsl_matrix* m = static_cast<gsl_matrix*>( malloc( sizeof( gsl_matrix ) ) );
      *m = gsl_matrix_const_view_vector_with_tda( v.get(), n1, n2, tda ).matrix;
      return matrix( m );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_matrix* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_matrix.
     * @return the gsl_matrix
     */
    gsl_matrix* get(){ return ccgsl_pointer; }
    /**
     * Get the gsl_matrix.
     * @return the gsl_matrix
     */
    gsl_matrix const* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_matrix.
     * @return @c true or @c falses according as 
     * this is the only matrix object sharing the gsl_matrix
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many matrix objects share this pointer.
     * @return the number of matrix objects that share this pointer
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_matrix
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
    // GSL functions
    /**
     * C++ version of gsl_matrix_calloc(). This constructs a matrix object with entries
     * initialised to zero.
     * @param n1 The number of rows in the matrix
     * @param n2 The number of columns in the matrix
     * @return A matrix initialised to zero
     */
    static matrix calloc( size_t const n1, size_t const n2 ){ return matrix( gsl_matrix_calloc( n1, n2 ) ); }
    /**
     * C++ version of gsl_matrix_set_zero().
     */
    void set_zero(){ gsl_matrix_set_zero( get() ); }
    /**
     * C++ version of gsl_matrix_set_all().
     * @param x The value to which all elements are set
     */
    void set_all( double x ){ gsl_matrix_set_all( get(), x ); }
    /**
     * C++ version of gsl_matrix_memcpy().
     * @param src source matrix
     * @return error code on failure
     */
    int memcpy( matrix const& src ){ return gsl_matrix_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_matrix_max().
     * @return maximum element of matrix
     */
    double max() const { return gsl_matrix_max( get() ); }
    /**
     * C++ version of gsl_matrix_min().
     * @return minimum element of matrix
     */
    double min() const { return gsl_matrix_min( get() ); }
    /**
     * C++ version of gsl_matrix_minmax().
     * @param min_out minimum element of matrix
     * @param max_out maximum element of matrix
     */
    void minmax( double* min_out, double* max_out ) const {
      gsl_matrix_minmax( get(), min_out, max_out ); }
    /**
     * C++ version of gsl_matrix_add().
     * @param b matrix to add to this
     * @return error code on failure
     */
    int add( matrix const& b ){ return gsl_matrix_add( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_sub().
     * @param b matrix to subtract from this
     * @return error code on failure
     */
    int sub( matrix const& b ){ return gsl_matrix_sub( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_scale().
     * @param x constant to multiply this by
     * @return error code on failure
     */
    int scale( double const x ){ return gsl_matrix_scale( get(), x ); }
    /**
     * C++ version of gsl_matrix_add_constant().
     * @param x constant to add to each element of this
     * @return error code on failure
     */
    int add_constant( double const x ){ return gsl_matrix_add_constant( get(), x ); }
    /**
     * C++ version of gsl_matrix_isnull().
     * @return @c +1 or @c 0 according as elements are all zero or not
     */
    int isnull() const { return gsl_matrix_isnull( get() ); }
    /**
     * C++ version of gsl_matrix_ispos().
     * @return @c +1 or @c 0 according as elements are all positive or not
     */
    int ispos() const { return gsl_matrix_ispos( get() ); }
    /**
     * C++ version of gsl_matrix_isneg().
     * @return @c +1 or @c 0 according as elements are all negative or not
     */
    int isneg() const { return gsl_matrix_isneg( get() ); }
    /**
     * C++ version of gsl_matrix_isnonneg().
     * @return @c +1 or @c 0 according as elements are all nonnegative or not
     */
    int isnonneg() const { return gsl_matrix_isnonneg( get() ); }
    /**
     * C++ version of gsl_matrix_get().
     * @param i index of row
     * @param j index of column
     * @return value of element
     */
    double get( size_t const i, size_t const j ) const { return gsl_matrix_get( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_set().
     * @param i index of row
     * @param j index of column
     * @param x new value for element
     */
    void set( size_t const i, size_t const j, double x ){ gsl_matrix_set( get(), i, j, x ); }
    /**
     * C++ version of gsl_matrix_ptr().
     * @param i index of row
     * @param j index of column
     * @return pointer to element
     */
    double* ptr( size_t const i, size_t const j ){ return gsl_matrix_ptr( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_const_ptr().
     * @param i index of row
     * @param j index of column
     * @return pointer to element
     */
    double const* const_ptr( size_t const i, size_t const j ) const {
      return gsl_matrix_const_ptr( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_fread().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fread( FILE* stream ){ return gsl_matrix_fread( stream, get() ); }
    /**
     * C++ version of gsl_matrix_fwrite().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_matrix_fwrite( stream, get() ); }
    /**
     * C++ version of gsl_matrix_fscanf().
     * @param stream A C file stream
     * @return error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_matrix_fscanf( stream, get() ); }
    /**
     * C++ version of gsl_matrix_fprintf().
     * @param stream A C file stream
     * @param format %d, %e, %f or %g
     * @return error code on failure
     */
    int fprintf( FILE* stream, char const* format ) const {
      return gsl_matrix_fprintf( stream, get(), format ); }
    /**
     * C++ version of gsl_matrix_alloc_from_block().
     * @param b The block
     * @param offset The offset within the block
     * @param n1 The number of rows in the matrix
     * @param n2 The number of columns in the matrix
     * @param d2 undocumented
     */
    matrix( block& b, size_t const offset, size_t const n1, size_t const n2, size_t const d2 ){
      ccgsl_pointer = gsl_matrix_alloc_from_block( b.get(), offset, n1, n2, d2 );
      // just plausibly we could allocate vector but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
    	// try to tidy up before rethrowing
    	gsl_matrix_free( ccgsl_pointer );
    	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * C++ version of gsl_matrix_alloc_from_matrix().
     * @param m The matrix
     * @param k1 the row of @c m to take as row zero
     * @param k2 the column of @c m to take as column zero
     * @param n1 The number of rows in the matrix
     * @param n2 The number of columns in the matrix
     */
    matrix( matrix& m, size_t const k1, size_t const k2, size_t const n1, size_t const n2 ){
      ccgsl_pointer = gsl_matrix_alloc_from_matrix( m.get(), k1, k2, n1, n2 );
      // just plausibly we could allocate matrix but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
    	// try to tidy up before rethrowing
    	gsl_matrix_free( ccgsl_pointer );
    	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // More functions
    /**
     * C++ version of gsl_matrix_set_identity().
     */
    void set_identity(){ gsl_matrix_set_identity( get() ); }
    /**
     * C++ version of gsl_matrix_swap_rows().
     * @param i Index of first row
     * @param j Index of second row
     * @return error code on failure
     */
    int swap_rows( size_t const i, size_t const j ){ return gsl_matrix_swap_rows( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_swap_columns().
     * @param i Index of first column
     * @param j Index of second column
     * @return error code on failure
     */
    int swap_columns( size_t const i, size_t const j ){
      return gsl_matrix_swap_columns( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_swap_rowcol(). Swap row and column in place. Matrix must be square.
     * @param i index of row
     * @param j index of column
     * @return error code on failure
     */
    int swap_rowcol( size_t const i, size_t const j ){ return gsl_matrix_swap_rowcol( get(), i, j ); }
    /**
     * C++ version of gsl_matrix_transpose().
     * @return error code on failure.
     */
    int transpose(){ return gsl_matrix_transpose( get() ); }
    /**
     * C++ version of gsl_matrix_transpose_memcpy().
     * @param src matrix whose transpose it to be copied to @c this
     * @return error code on failure
     */
    int transpose_memcpy( matrix const& src ){
      return gsl_matrix_transpose_memcpy( get(), src.get() ); }
    /**
     * C++ version of gsl_matrix_max_index().
     * @param imax row index of the first maximum element in the matrix
     * @param jmax column index of the first maximum element in the matrix
     */
    void max_index( size_t* imax, size_t* jmax ) const {
      gsl_matrix_max_index( get(), imax, jmax ); }
    /**
     * C++ version of gsl_matrix_min_index().
     * @param imin row index of the first minimum element in the matrix
     * @param jmin column index of the first minimum element in the matrix
     */
    void min_index( size_t* imin, size_t* jmin ) const {
      gsl_matrix_min_index( get(), imin, jmin ); }
    /**
     * C++ version of gsl_matrix_minmax_index().
     * @param imin row index of the first minimum element in the matrix
     * @param jmin column index of the first minimum element in the matrix
     * @param imax row index of the first maximum element in the matrix
     * @param jmax column index of the first maximum element in the matrix
     */
    void minmax_index( size_t* imin, size_t* jmin, size_t* imax, size_t* jmax ) const {
      gsl_matrix_minmax_index( get(), imin, jmin, imax, jmax ); }
    /**
     * C++ version of gsl_matrix_mul_elements(). Multiply matrices elementwise.
     * @param b Another matrix
     * @return error code on failure
     */
    int
    mul_elements( matrix const& b ){
      return gsl_matrix_mul_elements( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_div_elements(). Divide each element of @c this by the
     * corrsponding element of @c b
     * @param b Another matrix
     * @return error code on failure
     */
    int div_elements( matrix const& b ){
      return gsl_matrix_div_elements( get(), b.get() ); }
    /**
     * C++ version of gsl_matrix_add_diagonal().
     * @param x A constant
     * @return error code on failure
     */
    int add_diagonal( double const x ){
      return gsl_matrix_add_diagonal( get(), x ); }
    /**
     * C++ version of gsl_matrix_get_row().
     * @param v A vector
     * @param i The index of the row
     * @return error code on failure
     */
    int get_row( vector& v, size_t const i ) const {
      return gsl_matrix_get_row( v.get(), get(), i ); }
    /**
     * C++ version of gsl_matrix_get_col().
     * @param v A vector
     * @param j The index of the column
     * @return error code on failure
     */
    int get_col( vector& v, size_t const j ) const {
      return gsl_matrix_get_col( v.get(), get(), j ); }
    /**
     * C++ version of gsl_matrix_set_row().
     * @param i The index of the row
     * @param v A vector
     * @return error code on failure
     */
    int set_row( size_t const i, vector const& v ){
      return gsl_matrix_set_row( get(), i, v.get() ); }
    /**
     * C++ version of gsl_matrix_set_col().
     * @param j The index of the column
     * @param v A vector
     * @return error code on failure
     */
    int set_col( size_t const j, vector const& v ){
      return gsl_matrix_set_col( get(), j, v.get() ); }
    // Extra functions for []
    /**
     * This function allows us to use a matrix like an array. Use with caution.
     * Although @c matrix[i][j] is possible, it is less efficient than
     * matrix::set(). The effect is the same as row().
     * @param i The index of the row
     * @return A vector representing a row
     */
    vector operator[]( size_t const i ){
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "matrix is null", __FILE__, __LINE__, exception::GSL_EFAULT );
  	return vector();
      }
#ifndef GSL_RANGE_CHECK_OFF
#ifndef __GXX_EXPERIMENTAL_CXX0X__
      static vector something;
#endif
      //Check that position make sense
      if( i >= size1() ){
  	gsl_error( "trying to read beyond last row of matrix",
		   __FILE__, __LINE__, exception::GSL_EINVAL );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( vector() );
#else
	return something;
#endif
      }
      // n is a valid position
#endif
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_row( ccgsl_pointer, i ).vector;
      return vector( w );
    }
    /**
     * This function allows us to use a matrix like an array. Use with caution.
     * Although @c matrix[i][j] is possible, it is much less efficient than
     * matrix::set(). The effect is the same as row()
     * @param i The index of the row
     * @return A vector representing a row
     */
    vector const operator[]( size_t const i ) const {
      // First check that iterator is initialised.
      if( ccgsl_pointer == 0 ){
  	gsl_error( "matrix is null", __FILE__, __LINE__, exception::GSL_EFAULT );
  	return vector();
      }
      gsl_vector* w = static_cast<gsl_vector*>( malloc( sizeof( gsl_vector ) ) );
      *w = gsl_matrix_row( ccgsl_pointer, i ).vector;
      return vector( w );
    }
  };

  // Extra functions for vector allocation from matrix objects
  inline vector vector::alloc_row_from_matrix( matrix& m, size_t const i ){
    return vector ( gsl_vector_alloc_row_from_matrix( m.get(), i ) ); }
  inline vector vector::alloc_col_from_matrix( matrix& m, size_t const i ){
    return vector ( gsl_vector_alloc_col_from_matrix( m.get(), i ) ); }
}
#endif
