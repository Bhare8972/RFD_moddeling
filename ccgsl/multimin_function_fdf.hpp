/*
 * $Id: multimin_function_fdf.hpp 130 2012-03-27 09:45:00Z jdl3 $
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

#ifndef CCGSL_MULTIMIN_FUNCTION_FDF_HPP
#define CCGSL_MULTIMIN_FUNCTION_FDF_HPP

// For std::pair
#include<utility>
#include"multimin_function.hpp"

namespace gsl {
  namespace multimin {
    /**
     * Class that extends gsl_multimin_function_fdf so that it can be constructed
     * from arbitrary function objects. Since gsl::multimin::function_fdf is a subclass of gsl_multimin_function_fdf,
     * a pointer to an object of this class can be used whenever a pointer to a gsl_multimin_function_fdf
     * object is required. Thus, pointers to objects of this class can be used directly with the GNU
     * Scientific Library.
     *
     * You can construct a gsl::multimin::function_fdf object from arbitrary function objects.
     * If the function object is not already implemented, implementing it as a subclass T of
     * gsl::multimin::function_fdf::concept will make it easy to construct using gsl::multimin::function_fdf<T>(). If
     * the function object uses a single class T that cannot readily be modified, try something like
     * @code
     * class FDF : public gsl::multimin::function_fdf::concept {
     * public:
     *   FDF( T& t ) : t( t );
     *   double f( gsl::vector const& x ){ ... }
     *   double df( gsl::vector const& x ){ ... }
     *   std::pair<double,double> fdf( gsl::vector const& x ){ ... }
     * private:
     *   T& t;
     * };
     *
     * ...
     *
     *   FDF fdf( t );
     *   gsl::multimin_function_fdf ffdf( fdf );
     * @endcode
     * You can use similar constructions if f, df and fdf are defined in different classes.
     *
     * This class is designed for flexibility rather than efficient copying.
     * So if objects are to be copied frequently, consider using shared pointers
     * or the like. Moving is defined for C++11.
     */
    class function_fdf : public gsl_multimin_function_fdf {
    public:
      /**
       * This is an empty abstract base class. Its main purpose is to define the concept
       * of a class whose objects can be used to construct a function_fdf object. Any class that
       * implements all three functions can be used in the constructor of function_fdf. If
       * class is derived from gsl::function_fdf::concept, the compiler is likely to give better
       * warnings if one of the three required functions is not implemented correctly.
       */
      struct concept {
	/**
	 * The function.
	 * @param x The argument.
	 * @return The function evaluated at x.
	 */
	virtual double f( const vector& x ) = 0;
	/**
	 * The derivative.
	 * @param x The argument.
	 * @param gradient The derivative evaluated at x.
	 */
	virtual void df( const vector& x, vector& gradient ) = 0;
	/**
	 * The function and derivative.
	 * @param x The argument.
	 * @param result The function value at x.
	 * @param gradient The derivative evaluated at x.
	 */
	virtual void fdf( const vector& x, double* result, vector& gradient ) = 0;
	/**
	 * The dimension of the function argument
	 * @return The dimension (number of elements) of the function argument
	 */
	size_t size() const;
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
	typedef double (*function_t)(const vector&,void*);
	/**
	 * Typedef for function pointer.
	 */
	typedef void (*function_df_t)(const vector&,void*,vector&);
	/**
	 * Typedef for function pointer.
	 */
	typedef void (*function_fdf_t)(const vector&,void*,double*,vector&);
      };
      /**
       * Used internally to point to function objects.
       */
      template<typename T>
      struct FDF : public base_F {
      public:
	/**
	 * The constructor requires an object that implements concept.
	 * @param A function object that implements concept.
	 */
	FDF( T& t ) : t( t ){}
      public:
	/**
	 * A static function that gsl::multimin::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type F
	 * @return The function result.
	 */
	static double fn( gsl_vector const* x, void* params ){
	  FDF<T>* fdft = reinterpret_cast<FDF<T>*>( params );
	  fdft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  return fdft->t.f( fdft->xv ); }
	/**
	 * A static function that gsl::multimin::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type F
	 * @param df The gradient
	 */
	static void dfn( gsl_vector const* x, void* params, gsl_vector* df ){
	  FDF<T>* fdft = reinterpret_cast<FDF<T>*>( params );
	  fdft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  fdft->dfv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( df ) );
	  return fdft->t.df( fdft->xv, fdft->dfv ); }
	/**
	 * A static function that gsl::multimin::function_fdf can use.
	 * @param x The function argument.
	 * @param params Always a pointer to an object of type F
	 * @param f The function value
	 * @param df The gradient
	 */
	static void fdfn( gsl_vector const* x, void* params, double* f, gsl_vector* df ){
	  FDF<T>* fdft = reinterpret_cast<FDF<T>*>( params );
	  fdft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
	  fdft->dfv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( df ) );
	  return fdft->t.fdf( fdft->xv, f, fdft->dfv ); }
      private:
	/**
	 * A gsl::vector object for x.
	 */
	gsl::vector xv;
	/**
	 * A gsl::vector object for df.
	 */
	gsl::vector dfv;
	/**
	 * Reference to function object from which this was constructed.
	 */
	T& t;
      };
#endif
      /**
       * This gives something for params to point to.
       */
      base_F* function;
      /**
       * The shared reference count: used for copying this.
       */
      size_t* count;
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      function_fdf(){
	function = 0;
	f = 0;
	df = 0;
	fdf = 0;
	params = 0;
	count = 0; // initially nullptr will do
      }
      template<typename T>
      friend void function_fdf_constructor( function_fdf& fdf, T& t );
      /**
       * Could construct from a gsl_multimin_function_fdf. This constuctor is not actually visible, but
       * gsl::multimin::function_fdf::function_fdf<T>() will construct the object correctly.
       * @param v The gsl_multimin_function_fdf.
       */
      explicit function_fdf( gsl_multimin_function_fdf& v ){
	// explicitly set function = 0 to indicate nothing to be deleted.
	function = 0;
	n = 0;
	// copy
	f = v.f;
	df = v.df;
	fdf = v.fdf;
	params = v.params;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
      /**
       * Construct from an object that implements gsl::multimin::function_fdf::concept.
       * @param t An object that implements gsl::multimin::function_fdf::concept.
       */
      template<typename T>
      function_fdf( T& t ){
	function_fdf_constructor<T>( *this, t );
      }
       
      // Copy and move constructors
      // copy constructor
      /**
       * The copy constructor. This shares f. This constuctor is not actually visible, but
       * gsl::multimin::function_fdf::function_fdf<T>()
       * will construct the object correctly.
       * @param v The function_fdf to copy.
       */
      function_fdf( function_fdf const& v ) : function( v.function ), count( v.count ){
	f = v.f;
	n = v.n;
	df = v.df;
	fdf = v.fdf;
	params = v.params;
	if( count != 0 ) ++*count; // function_fdf is now shared.
      }
      // assignment operator
      /**
       * The assignment operator. This makes a shared copy.
       * @param v The function_fdf to copy
       */
      function_fdf& operator=( function_fdf const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  delete function;
	  delete count;
	}
	// Then copy
	function = v.function;
	f = v.f;
	n = v.n;
	df = v.df;
	fdf = v.fdf;
	params = v.params;
	// count = v.count;
	if( count != 0 ) ++*count; // function_fdf is now shared.
	return *this;
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The function_fdf to move.
       */
      function_fdf( function_fdf&& v ) : function( v.function ), count( nullptr ){
	std::swap( f, v.f );
	std::swap( n, v.n );
	std::swap( df, v.df );
	std::swap( fdf, v.fdf );
	std::swap( params, v.params );
	std::swap( count, v.count );
	v.function = nullptr;
      }
      /**
       * Move operator.
       * @param v The function_fdf to move.
       * @return A reference to f in this.
       */
      function_fdf& operator=( function_fdf&& v ){
	std::swap( function, v.function );
	std::swap( f, v.f );
	std::swap( n, v.n );
	std::swap( df, v.df );
	std::swap( fdf, v.fdf );
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
	  delete function;
	  delete count;
	}
      }
    };
#ifndef DOXYGEN_SKIP
    /**
     * Specialisation for gsl_function_fdf.
     */ 
    template<>
    inline void function_fdf_constructor( function_fdf& fdf, function_fdf& v ){
      fdf.function = v.function; fdf.count = v.count; fdf.f = v.f; fdf.n = v.n; fdf.df = v.df;
      fdf.fdf = v.fdf; fdf.params = v.params; if( fdf.count != 0 ) ++*fdf.count;
      /* function_fdf is now shared. */ }
    /**
     * Specialisation for copy constructor.
     */ 
    template<>
    inline void function_fdf_constructor( function_fdf& fdf, gsl_multimin_function_fdf& v ){
      fdf.f = v.f; fdf.n = v.n; fdf.df = v.df; fdf.fdf = v.fdf; fdf.params = v.params; }
    /**
     * This function allows us to specialise constructors with one argument. It handles three
     * cases: constructing from an object that implements concept; constructing from a
     * gsl_function_fdf object; and copy construction.
     */
    template<typename T>
    inline void function_fdf_constructor( function_fdf& fdf, T& t ){
      fdf.function = 0;
      fdf.count = 0;
      // try constructing from T
      fdf.function = new function_fdf::FDF<T>( t );
      fdf.n = t.size();
      fdf.f = &function_fdf::FDF<T>::fn;
      fdf.df = &function_fdf::FDF<T>::dfn;
      fdf.fdf = &function_fdf::FDF<T>::fdfn;
      fdf.params = fdf.function;
      // just plausibly we could fail to allocate count: no further action needed.
      fdf.count = new size_t;
      *fdf.count = 1; // initially there is just one reference to function
    }
#endif
  
    /**
     * Make a gsl::multimin::function_fdf from a function object that implements 
     * gsl::multimin::function_fdf::concept.
     * @param t The function object.
     */
    template<typename T>
    inline function_fdf make_function_fdf( T& t ){
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
