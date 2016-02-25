/*
 * $Id: multifit_function_fdf.hpp 296 2013-01-02 12:17:04Z jdl3 $
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

#ifndef CCGSL_MULTIFIT_FUNCTION_FDF_HPP
#define CCGSL_MULTIFIT_FUNCTION_FDF_HPP

#include<gsl/gsl_multifit_nlin.h>
#include"vector.hpp"
#include"matrix.hpp"

namespace gsl {
  namespace multifit {
    /**
     * Class that extends gsl_multifit_function_fdf so that it can be constructed
     * from arbitrary function objects. Since gsl::multifit::function_fdf
     * is a subclass of gsl_multifit_function_fdf,
     * a pointer to an object of this class can be used whenever a pointer
     * to a gsl_multifit_function_fdf
     * object is required. Thus, pointers to objects of this class can be
     * used directly with the GNU
     * Scientific Library.
     *
     * A multifit::function_fdf with argument a vector of length @em n should compute and return
     * @em n function values. In C++ it is straightforward to set up a class with
     * a member function that
     * implements multifit::function_fdf::concept to get a suitable function object.
     * The multifit::function_fdf can
     * then be constructed from a function object of this class.
     */
    class function_fdf : public gsl_multifit_function_fdf {
    public:
      /**
       * This is an empty abstract base class. Its main purpose is to define the concept
       * of a class whose objects can be used to construct a function object. Any class that
       * implements f, df and fdf can be used in the constructor of function. This version allows 
       * gsl::vector instead of gsl_vector* with the small overhead of
       * copying two pointers at each function evaluation.
       */
      struct concept {
	/**
	 * The function.
	 * @param x The argument (vector)
	 * @param f Vector of same size as x to hold the return values.
	 * @return An integer that represents the error code; return 0 (GSL_SUCCESS) for
	 * success and some value from the exception class for failure.
	 */
	virtual double f( gsl::vector const& x, gsl::vector& f ) = 0;
	/**
	 * The derivatives (as Jacobian matrix)
	 * @param x The argument (vector)
	 * @param J (Jacobian) matrix of size \f$n\times n\f$ where \f$n\f$ is the size of x;
	 * this holds the return values.
	 * @return An integer that represents the error code; return 0 (GSL_SUCCESS) for
	 * success and some alue from the exception class for failure.
	 */
	virtual double df( gsl::vector const& x, gsl::matrix& J ) = 0;
	/**
	 * The function (returned as a vector) and derivatives (as Jacobian matrix)
	 * @param x The argument (vector)
	 * @param f Vector of same size as x to hold the return values.
	 * @param J (Jacobian) matrix of size \f$n\times n\f$ where \f$n\f$ is the size of x;
	 * this holds the return values.
	 * @return An integer that represents the error code; return 0 (GSL_SUCCESS) for
	 * success and some alue from the exception class for failure.
	 */
	virtual double fdf( gsl::vector const& x, gsl::vector& f, gsl::matrix& J ) = 0;
	/**
	 * This function should return the number of elements of x and f in f().
	 */
	virtual size_t size() const = 0;
	/**
	 * This function should return the number of components of x (independent variables).
	 */
	virtual size_t num_parameters() const = 0;
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
	/**
	 * Typedef for derivatives pointer.
	 */
	typedef int (*dfunction_t)(gsl_vector const*,void*,gsl_matrix*);
	/**
	 * Typedef for function and derivatives pointer.
	 */
	typedef int (*fdfunction_t)(gsl_vector const*,void*,gsl_vector*,gsl_matrix*);
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
	 * A static function that gsl::function_fdf can use.
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
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int dfn( gsl_vector const* x, void* params, gsl_matrix* dfx ){
	  // share gsl_vector* and gsl::vector contents.
	  F<T>* ft = reinterpret_cast<F<T>*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->dfv.wrap_gsl_matrix_without_ownership( dfx );
	  return ft->t.df( ft->xv, ft->dfv ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param f The values of the functions.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int fdfn( gsl_vector const* x, void* params, gsl_vector* fx, gsl_matrix* dfx ){
	  // share gsl_vector* and gsl::vector contents.
	  F<T>* ft = reinterpret_cast<F<T>*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->fv.wrap_gsl_vector_without_ownership( fx );
	  ft->dfv.wrap_gsl_matrix_without_ownership( dfx );
	  return ft->t.fdf( ft->xv, ft->fv, ft->dfv ); }
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
	/**
	 * A gsl::matrix object for df.
	 */
	gsl::matrix dfv;
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
	Fn( int (*const f)(gsl::vector const&,gsl::vector&),
	    int (*const df)(gsl::vector const&,gsl::matrix&),
	    int (*const fdf)(gsl::vector const&,gsl::vector&,gsl::matrix&),
	    size_t n ) : f( f ), df( df ), fdf( fdf ), n( n ){}
	/**
	 * @return A function for gsl_function.
	 */
	function_t function(){ return &fn; }
	/**
	 * @return A function for gsl_function.
	 */
	dfunction_t dfunction(){ return &dfn; }
	/**
	 * @return A function for gsl_function.
	 */
	fdfunction_t fdfunction(){ return &fdfn; }
      private:
	/**
	 * The original function.
	 */
	int (*const f)(gsl::vector const&,gsl::vector&);
	/**
	 * The original derivatives.
	 */
	int (*const df)(gsl::vector const&,gsl::matrix&);
	/**
	 * The original function and derivatives.
	 */
	int (*const fdf)(gsl::vector const&,gsl::vector&,gsl::matrix&);
      public:
	/**
	 * The number of functions and arguments.
	 */
	size_t const n;
	/**
	 * A static function that gsl::multifit::function_fdf can use.
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
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int dfn( gsl_vector const* x, void* params, gsl_matrix* dfx ){
	  // share gsl_vector* and gsl::vector contents.
	  Fn* ft = reinterpret_cast<Fn*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->dfv.wrap_gsl_matrix_without_ownership( dfx );
	  return ft->df( ft->xv, ft->dfv ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param f The values of the functions.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int fdfn( gsl_vector const* x, void* params, gsl_vector* fx, gsl_matrix* dfx ){
	  // share gsl_vector* and gsl::vector contents.
	  Fn* ft = reinterpret_cast<Fn*>( params );
	  ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  ft->fv.wrap_gsl_vector_without_ownership( fx );
	  ft->dfv.wrap_gsl_matrix_without_ownership( dfx );
	  return ft->fdf( ft->xv, ft->fv, ft->dfv ); }
      private:
	/**
	 * A gsl::vector object for x.
	 */
	gsl::vector xv;
	/**
	 * A gsl::vector object for f.
	 */
	gsl::vector fv;
	/**
	 * A gsl::vector object for df.
	 */
	gsl::matrix dfv;
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
	 * A static function that gsl::function_fdf can use.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type gsl_F.
	 * @param f The values of the functions.
	 * @return The function result.
	 */
	static int fn( gsl_vector const* x, void* params, gsl_vector* f ){
	  return reinterpret_cast<F<T>*>( params )->t.f( x, f ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int dfn( gsl_vector const* x, void* params, gsl_matrix* df ){
	  return reinterpret_cast<F<T>*>( params )->t.df( x, df ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * This function must not be called in two threads at once.
	 * @param x The arguments of the functions.
	 * @param params Always a pointer to an object of type F.
	 * @param f The values of the functions.
	 * @param df The derivatives of the functions.
	 * @return The function result.
	 */
	static int fdfn( gsl_vector const* x, void* params, gsl_vector* f, gsl_matrix* df ){
	  return reinterpret_cast<F<T>*>( params )->t.fdf( x, f, df ); }
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
	 * @param df The function derivatives
	 * @param fdf The function values and derivatives
	 * @param n The number of functions and arguments.
	 */
	gsl_Fn( int (*const f)(gsl_vector const*,gsl_vector*),
		int (*const df)(gsl_vector const*,gsl_matrix*),
		int (*const fdf)(gsl_vector const*,gsl_vector*,gsl_matrix*),
		size_t n ) : f( f ), df( df ), fdf( fdf ), n( n ){}
	/**
	 * @return A function for gsl_function.
	 */
	function_t function(){ return &fn; }
	/**
	 * @return A function for gsl_function.
	 */
	dfunction_t dfunction(){ return &dfn; }
	/**
	 * @return A function for gsl_function.
	 */
	fdfunction_t fdfunction(){ return &fdfn; }
      private:
	/**
	 * The original function.
	 */
	int (*const f)(gsl_vector const*,gsl_vector*);
	/**
	 * The original function derivatives.
	 */
	int (*const df)(gsl_vector const*,gsl_matrix*);
	/**
	 * The original function and derivatives.
	 */
	int (*const fdf)(gsl_vector const*,gsl_vector*,gsl_matrix*);
      public:
	/**
	 * The number of functions and arguments.
	 */
	size_t const n;
	/**
	 * A static function that gsl::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type gsl_Fn
	 * @params f The function result.
	 * @return The function result.
	 */
	static int fn( gsl_vector const* x, void* params, gsl_vector* f ){
	  return reinterpret_cast<gsl_Fn*>( params )->f( x, f ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type gsl_Fn
	 * @params df The function derivatives.
	 * @return The function result.
	 */
	static int dfn( gsl_vector const* x, void* params, gsl_matrix* df ){
	  return reinterpret_cast<gsl_Fn*>( params )->df( x, df ); }
	/**
	 * A static function that gsl::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type gsl_Fn
	 * @params f The function result.
	 * @params df The function derivatives.
	 * @return The function result.
	 */
	static int fdfn( gsl_vector const* x, void* params, gsl_vector* f, gsl_matrix* df ){
	  return reinterpret_cast<gsl_Fn*>( params )->fdf( x, f, df ); }
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
      function_fdf(){
	func = 0;
	f = 0;
	df = 0;
	fdf = 0;
	n = 0;
	p = 0;
	params = 0;
	count = 0; // initially nullptr will do
      }
      template<typename T>
      friend void function_constructor( function_fdf&, T& );
      /**
       * Could construct from a gsl_multifit_function_fdf.
       * This constructor is not actually visible, but
       * gsl::multifit::function_fdf<T>() will construct the object correctly.
       * @param v The gsl_multifit_function_fdf.
       */
      explicit function_fdf( gsl_multifit_function_fdf& v ){
	// explicitly set function = 0 to indicate nothing to be deleted.
	func = 0;
	// copy
	f = v.f;
	df = v.df;
	fdf = v.fdf;
	n = v.n;
	p = v.p;
	params = v.params;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
      /**
       * Construct from an object that implements gsl::multifit::function_fdf::concept.
       * @param t An object that implements gsl::multifit::function_fdf::concept.
       */
      template<typename T>
      function_fdf( T& t ){ function_constructor<T>( *this, t ); }
      /**
       * Construct from a function with no parameters (but with n function values and arguments).
       * @param f The function to construct from.
       * @param df The derivatives to construct from.
       * @param fdf The function and derivatives to construct from.
       * @param n The number of functions and number of arguments of each.
       */
      function_fdf( int (* const f)(gsl::vector const&, gsl::vector&),
		    int (* const df)(gsl::vector const&, gsl::matrix&),
		    int (* const fdf)(gsl::vector const&, gsl::vector&, gsl::matrix&),
		    size_t n ){
	func = 0;
	count = 0;
	func = new function_fdf::Fn( f, df, fdf, n );
	this->f = Fn::fn;
	this->df = Fn::dfn;
	this->fdf = Fn::fdfn;
	this->n = n;
	this->p = p;
	params = func;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
       
      // Copy and move constructors
      // copy constructor
      /**
       * The copy constructor. This shares f. This constructor is not actually visible, but
       * gsl::multifit::function_fdf::multifit::function_fdf<T>()
       * will construct the object correctly.
       * @param v The function to copy.
       */
      function_fdf( function_fdf const& v ) : func( v.func ), count( v.count ){
	f = v.f;
	df = v.df;
	fdf = v.fdf;
	n = v.n;
	p = v.p;
	params = v.params;
	if( count != 0 ) ++*count; // function is now shared.
      }
      // assignment operator
      /**
       * The assignment operator. This makes a shared copy.
       * @param v The function to copy
       */
      function_fdf& operator=( function_fdf const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  delete func;
	  delete count;
	}
	// Then copy
	func = v.func;
	f = v.f;
	df = v.df;
	fdf = v.fdf;
	n = v.n;
	p = v.p;
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
      function_fdf( function_fdf&& v ) : func( v.func ), count( nullptr ){
	std::swap( f, v.f );
	std::swap( df, v.df );
	std::swap( fdf, v.fdf );
	std::swap( n, v.n );
	std::swap( p, v.p );
	std::swap( params, v.params );
	std::swap( count, v.count );
	v.func = nullptr;
      }
      /**
       * Move operator.
       * @param v The function to move.
       * @return A reference to f in this.
       */
      function_fdf& operator=( function_fdf&& v ){
	std::swap( func, v.func );
	std::swap( f, v.f );
	std::swap( df, v.df );
	std::swap( fdf, v.fdf );
	std::swap( n, v.n );
	std::swap( p, v.p );
	std::swap( params, v.params );
	std::swap( count, v.count );
	return *this;
      }
#endif
      /**
       * The destructor unshares any shared resource.
       */
      ~function_fdf(){ 
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
     * @param f The function_fdf.
     * @param v The function_fdf to copy to f.
     */ 
    template<>
    inline void function_constructor( function_fdf& f, function_fdf& v ){
      f.func = v.func; f.count = v.count; f.f = v.f; f.df = v.df; f.fdf = v.fdf; f.n = v.n;
      f.p = v.p; f.params = v.params; if( f.count != 0 ) ++*f.count;
      /* function is now shared. */ }
    /**
     * Specialisation for gsl_multifit_function_fdf.
     * @param f The function_fdf.
     * @param v The function_fdf to use for f.
     */ 
    template<>
    inline void function_constructor( function_fdf& f, gsl_multifit_function_fdf& v ){
      f.f = v.f; f.df = v.df; f.fdf = v.fdf; f.n = v.n; f.p = v.p; f.params = v.params; }
    /**
     * This function allows us to specialise constructors with one argument. It handles three
     * cases: constructing from an object that implements concept; constructing from a
     * gsl_multifit_function_fdf object; and copy construction.
     * @param f The function.
     * @param t A n object that implements function::concept.
     */
    template<typename T>
    inline void function_constructor( function_fdf& f, T& t ){
      f.func = 0;
      f.count = 0;
      // try constructing from T
      f.func = new function_fdf::F<T>( t );
      f.f = &function_fdf::F<T>::fn;
      f.df = &function_fdf::F<T>::dfn;
      f.fdf = &function_fdf::F<T>::fdfn;
      f.n = t.size();
      f.p = t.num_parameters();
      f.params = f.func;
      // just plausibly we could fail to allocate count: no further action needed.
      f.count = new size_t;
      *f.count = 1; // initially there is just one reference to function
    }
#endif

    /**
     * Make a gsl::multifit::function_fdf from a function object
     * that implements gsl::multifit::function_fdf::concept.
     * @param t The function object.
     */
    template<typename T>
    inline function_fdf make_function( T& t ){
      function_fdf fn( t );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
  }
}

#endif
