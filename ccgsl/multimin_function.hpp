/*
 * $Id: multimin_function.hpp 289 2012-12-04 15:32:17Z jdl3 $
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

#ifndef CCGSL_MULTIMIN_FUNCTION_HPP
#define CCGSL_MULTIMIN_FUNCTION_HPP

#include<gsl/gsl_multimin.h>
#include"vector.hpp"

namespace gsl {
  namespace multimin {
    /**
     * Class that extends gsl_multimin_function so that it can be constructed
     * from arbitrary function objects. The arbitrary function object
     * could be a object of a class containing a member function whose
     * argument and return values are doubles. The member function may be
     * declared const.
     *
     * To construct an object of this class from a function object t of class T, use, for example,
     * @code
     * gsl::multimin::function f( t, &T::operator() )
     * @endcode
     * where T::operator is a member function whose single argument is a double. To assign, use,
     * for example,
     * @code
     * f = multimin::make_function( t, &T::operator() )
     * @endcode
     * There are multiple forms of the function constructor and make_function(). These should
     * allow you to construct a function object from any reasonable function object and quite a
     * lot of unreasonable ones too. If f is an object of class gsl_multimin_function,
     * it is always possible to pass &f to a gsl function that requires a gsl_multimin_function* argument.
     *
     * This class is designed for flexibility rather than efficient copying.
     * So if objects are to be copied frequently, consider using shared pointers
     * or the like. Moving is defined for C++11.
     *
     * The class extends gsl_multimin_function.
     * If constructed from a function object, gsl_multimin_function::params is always the
     * function object and gsl_multimin_function::function calls the member function using
     * params to obtain an object to call the member function from.
     */
    class function : public gsl_multimin_function {
#ifndef DOXYGEN_SKIP
    private:
      /**
       * Used internally to point to function objects.
       */
      struct base_F {
        /**
         * Typedef for function pointer.
         */
        typedef double (*function_t)(gsl_vector const*,void*);
	/**
	 * Virtual destructor
	 */
	virtual ~base_F(){};
        /**
         * This must be overriden.
         * @return a function for gsl_multimin_function.
         */
        virtual function_t function() = 0;
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fuvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        Fuvcr( double (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type Fuvcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          Fuvcr* ft = reinterpret_cast<Fuvcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FuvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FuvCr( double (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FuvCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FuvCr* ft = reinterpret_cast<FuvCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fcvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        Fcvcr( double const (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type Fcvcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          Fcvcr* ft = reinterpret_cast<Fcvcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FcvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FcvCr( double const (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FcvCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FcvCr* ft = reinterpret_cast<FcvCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FVvcr( double volatile (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double volatile (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FVvcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FVvcr* ft = reinterpret_cast<FVvcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FVvCr( double volatile (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double volatile (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FVvCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FVvCr* ft = reinterpret_cast<FVvCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FCvcr( double const volatile (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const volatile (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FCvcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FCvcr* ft = reinterpret_cast<FCvcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FCvCr( double const volatile (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const volatile (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FCvCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FCvCr* ft = reinterpret_cast<FCvCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Furcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        Furcr( double& (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double& (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type Furcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          Furcr* ft = reinterpret_cast<Furcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FurCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FurCr( double& (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double& (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FurCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FurCr* ft = reinterpret_cast<FurCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fcrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        Fcrcr( double const& (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const& (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type Fcrcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          Fcrcr* ft = reinterpret_cast<Fcrcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FcrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FcrCr( double const& (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const& (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FcrCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FcrCr* ft = reinterpret_cast<FcrCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FVrcr( double volatile& (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double volatile& (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FVrcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FVrcr* ft = reinterpret_cast<FVrcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FVrCr( double volatile& (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double volatile& (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FVrCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FVrCr* ft = reinterpret_cast<FVrCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FCrcr( double const volatile& (*const f)(gsl::vector const&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const volatile& (*const f)(gsl::vector const&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FCrcr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FCrcr* ft = reinterpret_cast<FCrcr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         */
        FCrCr( double const volatile& (*const f)(gsl::vector const volatile&) ) : f( f ){}
        /**
         * @return A function for gsl_multimin_function.
         */
        function_t function(){ return &fn; }
      private:
        /**
         * The original function.
         */
        double const volatile& (*const f)(gsl::vector const volatile&);
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * A static function that gsl::multimin::function can use.
         * @param x The function argument.
         * @param params Always a pointer to an object of type FCrCr
         * @return The function result.
         */
        static double fn( gsl_vector const* x, void* params ){
          FCrCr* ft = reinterpret_cast<FCrCr*>( params );
          ft->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return ft->f( ft->xv ); }
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fuvcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fuvcru( T& c, double (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fuvcru<T>* const cl = reinterpret_cast<Fuvcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvCru( T& c, double (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvCru<T>* const cl = reinterpret_cast<FuvCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fcvcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fcvcru( T& c, double const (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fcvcru<T>* const cl = reinterpret_cast<Fcvcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvCru( T& c, double const (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvCru<T>* const cl = reinterpret_cast<FcvCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvcru( T& c, double volatile (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvcru<T>* const cl = reinterpret_cast<FVvcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvCru( T& c, double volatile (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvCru<T>* const cl = reinterpret_cast<FVvCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvcru( T& c, double const volatile (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvcru<T>* const cl = reinterpret_cast<FCvcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvCru( T& c, double const volatile (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvCru<T>* const cl = reinterpret_cast<FCvCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Furcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Furcru( T& c, double& (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Furcru<T>* const cl = reinterpret_cast<Furcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurCru( T& c, double& (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurCru<T>* const cl = reinterpret_cast<FurCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fcrcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fcrcru( T& c, double const& (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fcrcru<T>* const cl = reinterpret_cast<Fcrcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrCru( T& c, double const& (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrCru<T>* const cl = reinterpret_cast<FcrCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrcru( T& c, double volatile& (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrcru<T>* const cl = reinterpret_cast<FVrcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrCru( T& c, double volatile& (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrCru<T>* const cl = reinterpret_cast<FVrCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrcru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrcru( T& c, double const volatile& (T::*f)(gsl::vector const&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrcru<T>* const cl = reinterpret_cast<FCrcru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrCru : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrCru( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrCru<T>* const cl = reinterpret_cast<FCrCru<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const volatile&);
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fuvcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fuvcrc( T& c, double (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fuvcrc<T>* const cl = reinterpret_cast<Fuvcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvCrc( T& c, double (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvCrc<T>* const cl = reinterpret_cast<FuvCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fcvcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fcvcrc( T& c, double const (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fcvcrc<T>* const cl = reinterpret_cast<Fcvcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvCrc( T& c, double const (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvCrc<T>* const cl = reinterpret_cast<FcvCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvcrc( T& c, double volatile (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvcrc<T>* const cl = reinterpret_cast<FVvcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvCrc( T& c, double volatile (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvCrc<T>* const cl = reinterpret_cast<FVvCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvcrc( T& c, double const volatile (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvcrc<T>* const cl = reinterpret_cast<FCvcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvCrc( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvCrc<T>* const cl = reinterpret_cast<FCvCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Furcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Furcrc( T& c, double& (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Furcrc<T>* const cl = reinterpret_cast<Furcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurCrc( T& c, double& (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurCrc<T>* const cl = reinterpret_cast<FurCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class Fcrcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        Fcrcrc( T& c, double const& (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          Fcrcrc<T>* const cl = reinterpret_cast<Fcrcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrCrc( T& c, double const& (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrCrc<T>* const cl = reinterpret_cast<FcrCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrcrc( T& c, double volatile& (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrcrc<T>* const cl = reinterpret_cast<FVrcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrCrc( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrCrc<T>* const cl = reinterpret_cast<FVrCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrcrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrcrc( T& c, double const volatile& (T::*f)(gsl::vector const&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrcrc<T>* const cl = reinterpret_cast<FCrcrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrCrc : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrCrc( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrCrc<T>* const cl = reinterpret_cast<FCrCrc<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const volatile&) const;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvcrV( T& c, double (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvcrV<T>* const cl = reinterpret_cast<FuvcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvCrV( T& c, double (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvCrV<T>* const cl = reinterpret_cast<FuvCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvcrV( T& c, double const (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvcrV<T>* const cl = reinterpret_cast<FcvcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvCrV( T& c, double const (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvCrV<T>* const cl = reinterpret_cast<FcvCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvcrV( T& c, double volatile (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvcrV<T>* const cl = reinterpret_cast<FVvcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvCrV( T& c, double volatile (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvCrV<T>* const cl = reinterpret_cast<FVvCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvcrV( T& c, double const volatile (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvcrV<T>* const cl = reinterpret_cast<FCvcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvCrV( T& c, double const volatile (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvCrV<T>* const cl = reinterpret_cast<FCvCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurcrV( T& c, double& (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurcrV<T>* const cl = reinterpret_cast<FurcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurCrV( T& c, double& (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurCrV<T>* const cl = reinterpret_cast<FurCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrcrV( T& c, double const& (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrcrV<T>* const cl = reinterpret_cast<FcrcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrCrV( T& c, double const& (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrCrV<T>* const cl = reinterpret_cast<FcrCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrcrV( T& c, double volatile& (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrcrV<T>* const cl = reinterpret_cast<FVrcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrCrV( T& c, double volatile& (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrCrV<T>* const cl = reinterpret_cast<FVrCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrcrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrcrV( T& c, double const volatile& (T::*f)(gsl::vector const&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrcrV<T>* const cl = reinterpret_cast<FCrcrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrCrV : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrCrV( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrCrV<T>* const cl = reinterpret_cast<FCrCrV<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const volatile&) volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvcrC( T& c, double (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvcrC<T>* const cl = reinterpret_cast<FuvcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FuvCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FuvCrC( T& c, double (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FuvCrC<T>* const cl = reinterpret_cast<FuvCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvcrC( T& c, double const (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvcrC<T>* const cl = reinterpret_cast<FcvcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcvCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcvCrC( T& c, double const (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcvCrC<T>* const cl = reinterpret_cast<FcvCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvcrC( T& c, double volatile (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvcrC<T>* const cl = reinterpret_cast<FVvcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVvCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVvCrC( T& c, double volatile (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVvCrC<T>* const cl = reinterpret_cast<FVvCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvcrC( T& c, double const volatile (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvcrC<T>* const cl = reinterpret_cast<FCvcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCvCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCvCrC( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCvCrC<T>* const cl = reinterpret_cast<FCvCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurcrC( T& c, double& (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurcrC<T>* const cl = reinterpret_cast<FurcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FurCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FurCrC( T& c, double& (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FurCrC<T>* const cl = reinterpret_cast<FurCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double& (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrcrC( T& c, double const& (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrcrC<T>* const cl = reinterpret_cast<FcrcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FcrCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FcrCrC( T& c, double const& (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FcrCrC<T>* const cl = reinterpret_cast<FcrCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const& (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrcrC( T& c, double volatile& (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrcrC<T>* const cl = reinterpret_cast<FVrcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FVrCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FVrCrC( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FVrCrC<T>* const cl = reinterpret_cast<FVrCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double volatile& (T::*f)(gsl::vector const volatile&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrcrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrcrC( T& c, double const volatile& (T::*f)(gsl::vector const&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrcrC<T>* const cl = reinterpret_cast<FCrcrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const&) const volatile;
      };
      /**
       * Subclass that allows us to use arbitrary
       * function objects.
       */
      template<typename T>
      class FCrCrC : public base_F {
      public:
        /**
         * The constructor requires the function object and
         * a suitable function. Typically this will
         * be &T::operator().
         * @param c The object.
         * @param f A member function.
         */
        FCrCrC( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const volatile )
          : c( c ), f(f) {}
        /**
         * Give access to the function from T in F.
         * @param x A gsl::vector.
         * @return The value of c.*f( x ).
         */
        double operator()( gsl::vector const& x ){ return (c.*f)( x ); }
        /**
         * @return A function for gsl_function.
         */
        function_t function(){ return &function_s; }
      private:
        /**
         * A gsl::vector object for x.
         */
        gsl::vector xv;
        /**
         * The function. This is static so that gsl_function can
         * hold a pointer to it without a function object.
         * @param x A gsl::vector.
         * @param params The parameters, which are always set to be a pointer
         * to the function object.
         * @return the function object function evaluated at x.
         */
        static double function_s( gsl_vector const* x, void* params ){
          FCrCrC<T>* const cl = reinterpret_cast<FCrCrC<T>*>( params );
          cl->xv.wrap_gsl_vector_without_ownership( const_cast<gsl_vector*>( x ) );
          return (cl->c.*cl->f)( cl->xv ); }
        /**
         * Reference to the function object.
         */
        T& c;
        /**
         * Pointer to the function object function.
         */
        double const volatile& (T::*f)(gsl::vector const volatile&) const volatile;
      };
#endif
    base_F* base_f;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    function(){
      base_f = 0;
      f = 0;
      n = 0;
      params = 0;
      count = 0; // initially nullptr will do
    }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double volatile (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double volatile (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const volatile (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const volatile (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double& (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Furcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double& (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const& (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const& (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double volatile& (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double volatile& (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const volatile& (*const f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrcr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param n The number of parameters of the function.
       */
      explicit function( double const volatile& (*const f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrCr( f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Furcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrcru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&), size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrCru<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Furcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrc<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrV<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FurCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param n The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const n ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrC<T>( c, f );
        params = base_f;
        this->f = &fn;
        this->n = n;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
    // Copy and move constructors
    // copy constructor
    /**
     * The copy constructor. This shares base_f. 
     * @param v The function to copy.
     */
    function( function const& v ) : base_f( v.base_f ), count( v.count ){
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
	delete base_f;
	delete count;
      }
      // Then copy
      base_f = v.base_f;
      f = v.f;
      n = v.n;
      params = v.params;
      count = v.count;
      if( count != 0 ) ++*count; // function is now shared.
      return *this;
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The function to move.
     */
    function( function&& v ) : base_f( v.base_f ), count( nullptr ){
      std::swap( f, v.f );
      std::swap( n, v.n );
      std::swap( params, v.params );
      std::swap( count, v.count );
      v.base_f = nullptr;
    }
    /**
     * Move operator.
     * @param v The function to move.
     * @return A reference to base_f in this.
     */
    function& operator=( function&& v ){
      std::swap( base_f, v.base_f );
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
      // first, possibly delete anything pointed to by base_f
      if( count == 0 or --*count == 0 ){
	delete base_f;
	delete count;
      }
    }
  private:
    /**
     * This is the function that gsl_multimin_function points to if
     * function is constructed from a function object.
     * @param x A double.
     * @param params The parameters, which are always set to be a pointer
     * to the function object.
     * @return the function object function evaluated at x.
     */
    static double fn( gsl_vector const* x, void* params ){
      base_F* const b = reinterpret_cast<base_F*>( params );
      return b->function()( x, params );
    }
  };
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) , size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  const, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::multimin::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param n The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const n ){
      function fn( c, f, n );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif

  }
}

#endif
