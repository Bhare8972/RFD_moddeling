/*
 * $Id: function_fdf.hpp 51 2012-01-07 13:55:48Z jdl3 $
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

#ifndef CCGSL_FUNCTION_FDF_HPP
#define CCGSL_FUNCTION_FDF_HPP

// For std::pair
#include<utility>

namespace gsl {
  /**
   * Class that extends gsl_function_fdf so that it can be constructed
   * from arbitrary function objects. Since gsl::function_fdf is a subclass of gsl_function_fdf,
   * a pointer to an object of this class can be used whenever a pointer to a gsl_function_fdf
   * object is required. Thus, pointers to objects of this class can be used directly with the GNU
   * Scientific Library.
   *
   * There are several ways to construct a gsl::function_fdf object from arbitrary function objects.
   * If the function object is not already implemented, implementing it as a subclass T of
   * gsl::function_fdf::concept will make it easy to construct using gsl::function_fdf<T>(). If
   * the function object uses a single class T that cannot readily be modified, try something like
   * @code
   * class FDF : public gsl::function_fdf::concept {
   * public:
   *   FDF( T& t ) : t( t );
   *   double f( double x ){ ... }
   *   double df( double x ){ ... }
   *   std::pair<double,double> fdf( double x ){ ... }
   * private:
   *   T& t;
   * };
   *
   * ...
   *
   *   FDF fdf( t );
   *   gsl::function_fdf ffdf( fdf );
   * @endcode
   * You can use similar constructions if f, df and fdf are defined in different classes.
   *
   * Another way to construct a gsl::function_fdf object is to start with gsl::function_scl 
   * objects f and df for the function and derivative. Then
   * @code
   *   gsl::function_fdf fdf( f, fdf );
   * @endcode
   * will construct a suitable object. In general, this is less efficient than constructing from a
   * class that implements concept. However, the overhead of using this method is only high if
   * there is significant benefit in calculating the values of f and df at the same time and the 
   * function objects do not take advantage of this benefit.
   *
   * This class is designed for flexibility rather than efficient copying.
   * So if objects are to be copied frequently, consider using shared pointers
   * or the like. Moving is defined for C++11.
   */
  class function_fdf : public gsl_function_fdf {
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
      virtual double f( double x ) = 0;
      /**
       * The derivative.
       * @param x The argument.
       * @return The derivative evaluated at x.
       */
      virtual double df( double x ) = 0;
      /**
       * The function and derivative.
       * @param x The argument.
       * @return The function and derivative (in that order) evaluated at x.
       */
      virtual std::pair<double,double> fdf( double x ) = 0;
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
      typedef double (*function_t)(double,void*);
      /**
       * Typedef for function pointer.
       */
      typedef void (*function_fdf_t)(double,void*,double*,double*);
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
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FDF<T>*>( params )->t.f( x ); }
      /**
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static double dfn( double x, void* params ){
        return reinterpret_cast<FDF<T>*>( params )->t.df( x ); }
      /**
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static void fdfn( double x, void* params, double* d, double* df ){
	std::pair<double,double> pair = reinterpret_cast<FDF<T>*>( params )->t.fdf( x );
	*d = pair.first; *df = pair.second; }
    private:
      /**
       * Reference to function object from which this was constructed.
       */
      T& t;
    };
    /**
     * Class that allows us to convert a pair of function_scl objects into
     * a function_fdf object.
     */
    struct function_scl_adapter : public base_F, public concept {
    public:
      function_scl_adapter( function_scl& function, function_scl&  dfunction )
	: scl_function( function ), scl_dfunction( dfunction ){}
      /**
       * The function.
       * @param x The argument.
       * @return The function evaluated at x.
       */
      double f( double x ){ return fn_eval( scl_function, x ); }
      /**
       * The derivative.
       * @param x The argument.
       * @return The derivative evaluated at x.
       */
      double df( double x ){ return fn_eval( scl_dfunction, x ); }
      /**
       * The function and derivative.
       * @param x The argument.
       * @return The function and derivative (in that order) evaluated at x.
       */
      std::pair<double,double> fdf( double x ){
	return std::make_pair( fn_eval( scl_function, x ), fn_eval( scl_dfunction, x ) ); }
    public:
      /**
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<function_scl_adapter*>( params )->f( x ); }
      /**
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static double dfn( double x, void* params ){
        return reinterpret_cast<function_scl_adapter*>( params )->df( x ); }
      /**
       * A static function that gsl::function_fdf can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type F
       * @return The function result.
       */
      static void fdfn( double x, void* params, double* d, double* df ){
	std::pair<double,double> pair = reinterpret_cast<function_scl_adapter*>( params )->fdf( x );
	*d = pair.first; *df = pair.second; }
    private:
      /**
       * The function function object.
       */
      function_scl& scl_function;
      /**
       * The derivative function object.
       */
      function_scl& scl_dfunction;
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
     * Could construct from a gsl_function_fdf. This constuctor is not actually visible, but
     * gsl::function_fdf::function_fdf<T>() will construct the object correctly.
     * @param v The gsl_function_fdf.
     */
    explicit function_fdf( gsl_function_fdf& v ){
      // explicitly set function = 0 to indicate nothing to be deleted.
      function = 0;
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
     * Construct from an object that implements gsl::function_fdf::concept.
     * @param t An object that implements gsl::function_fdf::concept.
     */
    template<typename T>
    function_fdf( T& t ){
      function_fdf_constructor<T>( *this, t );
    }
    /**
     * Construct from function_scl objects for function and derivative. This method is not
     * generally as efficient as constructing from a single function object.
     * @param function A function object for the function.
     * @param dfunction A function object for the derivative.
     */
    function_fdf( function_scl& function, function_scl&  dfunction ){
      this->function = 0;
      count = 0;
      // try constructing from function_scl_adapter
      this->function = new function_scl_adapter( function, dfunction );
      f = &function_scl_adapter::fn;
      df = &function_scl_adapter::dfn;
      fdf = &function_scl_adapter::fdfn;
      params = this->function;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to function
    }
       
    // Copy and move constructors
    // copy constructor
    /**
     * The copy constructor. This shares f. This constuctor is not actually visible, but
     * gsl::function_fdf::function_fdf<T>() will construct the object correctly.
     * @param v The function_fdf to copy.
     */
    function_fdf( function_fdf const& v ) : function( v.function ), count( v.count ){
      f = v.f;
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
    fdf.function = v.function; fdf.count = v.count; fdf.f = v.f; fdf.df = v.df;
    fdf.fdf = v.fdf; fdf.params = v.params; if( fdf.count != 0 ) ++*fdf.count;
    /* function_fdf is now shared. */ }
  /**
   * Specialisation for copy constructor.
   */ 
  template<>
  inline void function_fdf_constructor( function_fdf& fdf, gsl_function_fdf& v ){
    fdf.f = v.f; fdf.df = v.df; fdf.fdf = v.fdf; fdf.params = v.params; }
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
   * Make a gsl::function_fdf from a function object that implements gsl::function_fdf::concept.
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

  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @return The function value.
   */
  inline double fn_fdf_eval_f( gsl_function_fdf const* const F, double const x ){
    return (F->f)( x, F->params );
  }
  
  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @return The function value.
   */
  inline double fn_fdf_eval_f( gsl_function_fdf const& F, double const x ){
    return (F.f)( x, F.params );
  }

  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @return The gradient value.
   */
  inline double fn_fdf_eval_df( gsl_function_fdf const* const F, double const x ){
    return (F->df)( x, F->params );
  }
  
  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @return The gradient value.
   */
  inline double fn_fdf_eval_df( gsl_function_fdf const& F, double const x ){
    return (F.df)( x, F.params );
  }

  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @param f The function value (returned).
   * @param df The gradient value (returned).
   */
  inline void fn_fdf_eval_df( gsl_function_fdf const* const F, double const x,
			      double* f, double* df ){
    return (F->fdf)( x, F->params, f, df );
  }
  
  /**
   * Evaluate a gsl::function_fdf (or gsl_function_fdf).
   * @param F The function.
   * @param x The argument.
   * @param f The function value (returned).
   * @param df The gradient value (returned).
   */
  inline void fn_fdf_eval_df( gsl_function_fdf const& F, double const x,
			      double& f, double& df ){
    return (F.fdf)( x, F.params, &f, &df );
  }
}

#endif
