/*
 * $Id: multiroot_function.hpp 235 2012-08-08 19:46:06Z jdl3 $
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

#ifndef CCGSL_MULTIROOT_FUNCTION_HPP
#define CCGSL_MULTIROOT_FUNCTION_HPP

#include<gsl/gsl_multiroots.h>

namespace gsl {
  namespace multiroot {
    /**
     * Class that extends gsl_multiroot_function so that it can be constructed
     * from arbitrary function objects. Since gsl::multiroot::function
     * is a subclass of gsl_multiroot_function,
     * a pointer to an object of this class can be used whenever a pointer
     * to a gsl_multiroot_function
     * object is required. Thus, pointers to objects of this class can be
     * used directly with the GNU
     * Scientific Library.
     *
     * A multiroot::function with argument a vector of length @em n should compute and return
     * @em n function values. In C++ it is straightforward to set up a class with
     * a member function that
     * implements multiroot::function::concept to get a suitable function object.
     * The multiroot::function can
     * then be constructed from a function object of this class.
     */
    class function : public gsl_multiroot_function {
    public:
      /**
       * This is an empty abstract base class. Its main purpose is to define the concept
       * of a class whose objects can be used to construct a function object. Any class that
       * implements f can be used in the constructor of function. This version allows 
       * gsl::vector instead of gsl_vector* with the small overhead of
       * copying two pointers at each function evaluation.
       */
      struct concept {
	/**
	 * The function.
	 * @param x The argument (vector)
	 * @param f Vector of same size as x to hold the return values.
	 * @return An integer that represents the error code; return 0 (GSL_SUCCESS) for
	 * success and some alue from the exception class for failure.
	 */
	virtual int f( gsl::vector const& x, gsl::vector& f ) = 0;
	/**
	 * This function should return the number of elements of x and f in f().
	 */
	virtual size_t size() const = 0;
      };
#ifndef DOXYGEN_SKIP
    private:
      /**
       * Used internally to point to function objects.
       */
      struct base_F {
	/**
	 * Typedef for function pointer.
	 */
	typedef int (*function_t)(gsl_vector const*,void*,gsl_vector*);
      };
      /**
       * Used internally to point to function objects.
       */
      template<typename T>
      struct F : public base_F {
      public:
	/**
	 * The constructor requires an object that implements concept.
	 * @param A function object that implements concept.
	 */
	F( T& t ) : t( t ){}
      public:
	/**
	 * A static function that gsl::function_f can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param f The values of the functions.
	 * @return The function result.
	 */
	static int fn( gsl_vector const* x, void* params, gsl_vector* fx ){
	  // share gsl_vector* and gsl::vector contents.
	  F<T>* ft = reinterpret_cast<F<T>*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->fv.wrap_gsl_vector_without_ownership( fx );
	  return ft->t.f( ft->xv, ft->fv ); }
      private:
	/**
	 * Reference to function object from which this was constructed.
	 */
	T& t;
	/**
	 * A gsl::vector object for x.
	 */
	gsl::vector xv;
	/**
	 * A gsl::vector object for f.
	 */
	gsl::vector fv;
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fn : public base_F {
      public:
	/**
	 * The constructor requires a function.
	 * @param f A function.
	 * @param n The number of functions and arguments.
	 */
	Fn( int (*const f)(gsl::vector const&,gsl::vector&), size_t n ) : f( f ), n( n ){}
	/**
	 * @return A function for gsl_function.
	 */
	function_t function(){ return &fn; }
      private:
	/**
	 * The original function.
	 */
	int (*const f)(gsl::vector const&,gsl::vector&);
      public:
	/**
	 * The number of functions and arguments.
	 */
	size_t const n;
	/**
	 * A static function that gsl::multiroot::function can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type Fn.
	 * @param f The values of the functions.
	 * @return The function return code.
	 */
	static int fn( gsl_vector const* x, void* params, gsl_vector* fx ){
	  // share gsl_vector* and gsl::vector contents.
	  Fn* ft = reinterpret_cast<Fn*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->fv.wrap_gsl_vector_without_ownership( fx );
	  return ft->f( ft->xv, ft->fv ); }
      private:
	/**
	 * A gsl::vector object for x.
	 */
	gsl::vector xv;
	/**
	 * A gsl::vector object for f.
	 */
	gsl::vector fv;
      };
      /**
       * Used internally to point to function objects.
       */
      template<typename T>
      struct gsl_F : public base_F {
      public:
	/**
	 * The constructor requires an object that implements concept.
	 * @param A function object that implements concept.
	 */
	gsl_F( T& t ) : t( t ){}
      public:
	/**
	 * A static function that gsl::function_f can use.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type gsl_F.
	 * @param f The values of the functions.
	 * @return The function result.
	 */
	static int fn( gsl_vector const* x,  void* params, gsl::vector* f ){
	  return reinterpret_cast<F<T>*>( params )->t.f( x, f ); }
      private:
	/**
	 * Reference to function object from which this was constructed.
	 */
	T& t;
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class gsl_Fn : public base_F {
      public:
	/**
	 * The constructor requires a function.
	 * @param f A function.
	 * @param n The number of functions and arguments.
	 */
	gsl_Fn( int (*const f)(gsl_vector const*,gsl_vector*), size_t n ) : f( f ), n( n ){}
	/**
	 * @return A function for gsl_function.
	 */
	function_t function(){ return &fn; }
      private:
	/**
	 * The original function.
	 */
	int (*const f)(gsl_vector const*,gsl_vector*);
      public:
	/**
	 * The number of functions and arguments.
	 */
	size_t const n;
	/**
	 * A static function that gsl::function_scl can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type gsl_Fn
	 * @return The function result.
	 */
	static int fn( gsl_vector const* x, void* params, gsl_vector* f ){
	  return reinterpret_cast<gsl_Fn*>( params )->f( x, f ); }
      };
#endif
      /**
       * This gives something for params to point to.
       */
      base_F* func;
      /**
       * The shared reference count: used for copying this.
       */
      size_t* count;
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      function(){
	func = 0;
	f = 0;
	n = 0;
	params = 0;
	count = 0; // initially nullptr will do
      }
      template<typename T>
      friend void function_constructor( function&, T& );
      /**
       * Could construct from a gsl_multiroot_function. This constructor is not actually visible, but
       * gsl::multiroot::function<T>() will construct the object correctly.
       * @param v The gsl_multiroot_function.
       */
      explicit function( gsl_multiroot_function& v ){
	// explicitly set function = 0 to indicate nothing to be deleted.
	func = 0;
	// copy
	f = v.f;
	n = v.n;
	params = v.params;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
      /**
       * Construct from an object that implements gsl::multiroot::function::concept.
       * @param t An object that implements gsl::multiroot::function::concept.
       */
      template<typename T>
      function( T& t ){ function_constructor<T>( *this, t ); }
      /**
       * Construct from a function with no parameters (but with n function values and arguments).
       * @param f The function to construct from.
       * @param n The number of functions and number of arguments of each.
       */
      function( int (* const f)(gsl::vector const&, gsl::vector&), size_t n ){
	func = 0;
	count = 0;
	func = new function::Fn( f, n );
	this->f = Fn::fn;
	this->n = n;
	params = func;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
       
      // Copy and move constructors
      // copy constructor
      /**
       * The copy constructor. This shares f. This constructor is not actually visible, but
       * gsl::multiroot::function::multiroot::function<T>() will construct the object correctly.
       * @param v The function to copy.
       */
      function( function const& v ) : func( v.func ), count( v.count ){
	f = v.f;
	n = v.n;
	params = v.params;
	if( count != 0 ) ++*count; // function is now shared.
      }
      // assignment operator
      /**
       * The assignment operator. This makes a shared copy.
       * @param v The function to copy
       */
      function& operator=( function const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  delete func;
	  delete count;
	}
	// Then copy
	func = v.func;
	f = v.f;
	n = v.n;
	params = v.params;
	// count = v.count;
	if( count != 0 ) ++*count; // function is now shared.
	return *this;
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The function to move.
       */
      function( function&& v ) : func( v.func ), count( nullptr ){
	std::swap( f, v.f );
	std::swap( n, v.n );
	std::swap( params, v.params );
	std::swap( count, v.count );
	v.func = nullptr;
      }
      /**
       * Move operator.
       * @param v The function to move.
       * @return A reference to f in this.
       */
      function& operator=( function&& v ){
	std::swap( func, v.func );
	std::swap( f, v.f );
	std::swap( n, v.n );
	std::swap( params, v.params );
	std::swap( count, v.count );
	return *this;
      }
#endif
      /**
       * The destructor unshares any shared resource.
       */
      ~function(){ 
	// first, possibly delete anything pointed to by f
	if( count == 0 or --*count == 0 ){
	  delete func;
	  delete count;
	}
      }
    };
#ifndef DOXYGEN_SKIP
    /**
     * Specialisation for copy constructor.
     * @param f The function.
     * @param v The function to copy to f.
     */ 
    template<>
    inline void function_constructor( function& f, function& v ){
      f.func = v.func; f.count = v.count; f.f = v.f; f.n = v.n;
      f.params = v.params; if( f.count != 0 ) ++*f.count;
      /* function is now shared. */ }
    /**
     * Specialisation for gsl_multiroot_function.
     * @param f The function.
     * @param v The function to use for f.
     */ 
    template<>
    inline void function_constructor( function& f, gsl_multiroot_function& v ){
      f.f = v.f; f.n = v.n; f.params = v.params; }
    /**
     * This function allows us to specialise constructors with one argument. It handles three
     * cases: constructing from an object that implements concept; constructing from a
     * gsl_multiroot_function object; and copy construction.
     * @param f The function.
     * @param t A n object that implements function::concept.
     */
    template<typename T>
    inline void function_constructor( function& f, T& t ){
      f.func = 0;
      f.count = 0;
      // try constructing from T
      f.func = new function::F<T>( t );
      f.f = &function::F<T>::fn;
      f.n = t.size();
      f.params = f.func;
      // just plausibly we could fail to allocate count: no further action needed.
      f.count = new size_t;
      *f.count = 1; // initially there is just one reference to function
    }
#endif

    /**
     * Make a gsl::multiroot::function from a function object
     * that implements gsl::multiroot::function::concept.
     * @param t The function object.
     */
    template<typename T>
    inline function make_function( T& t ){
      function fn( t );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
  }
}

#endif
