/*
 * $Id: monte.hpp 289 2012-12-04 15:32:17Z jdl3 $
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

#ifndef CCGSL_MONTE_HPP
#define CCGSL_MONTE_HPP

#include<gsl/gsl_monte.h>
#include"vector.hpp"
#include"rng.hpp"

namespace gsl {
  namespace monte {
    /**
     * Class that extends gsl_monte_function so that it can be constructed
     * from arbitrary function objects. The arbitrary function object
     * dould be a object of a class containing a member function whose
     * argument and return values are doubles. The member function may be
     * declared const.
     *
     * To construct an object of this class from a function object t of class T, use, for example,
     * @code
     * gsl::monte::function f( t, dim, &T::operator() )
     * @endcode
     * where T::operator is a member function whose single argument is a double. To assign, use,
     * for example,
     * @code
     * f = monte::make_function( t, dim, &T::operator() )
     * @endcode
     * There are multiple forms of the function constructor and make_function(). These should
     * allow you to construct a function object from any reasonable function object and quite a
     * lot of unreasonable ones too. If f is an object of class gsl_monte_function,
     * it is always possible to pass &f to a gsl function that requires a gsl_monte_function* argument.
     *
     * This class is designed for flexibility rather than efficient copying.
     * So if objects are to be copied frequently, consider using shared pointers
     * or the like. Moving is defined for C++11.
     *
     * The class extends gsl_monte_function.
     * If constructed from a function object, gsl_monte_function::params is always the
     * function object and gsl_monte_function::function calls the member function using
     * params to obtain an object to call the member function from.
     */
    class function : public gsl_monte_function {
#ifndef DOXYGEN_SKIP
    private:
      /**
       * Used internally to point to function objects.
       */
      struct base_F {
        /**
         * Typedef for function pointer.
         */
        typedef double (*function_t)(double*,size_t,void*);
	/**
	 * Virtual destructor
	 */
	virtual ~base_F(){};
        /**
         * This must be overriden.
         * @return a function for gsl_monte_function.
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
         * @param dim The number of arguments the function takes.
         */
        Fuvcr( double (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type Fuvcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          Fuvcr* ft = reinterpret_cast<Fuvcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FuvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FuvCr( double (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FuvCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FuvCr* ft = reinterpret_cast<FuvCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fcvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        Fcvcr( double const (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type Fcvcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          Fcvcr* ft = reinterpret_cast<Fcvcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FcvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FcvCr( double const (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FcvCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FcvCr* ft = reinterpret_cast<FcvCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FVvcr( double volatile (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FVvcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FVvcr* ft = reinterpret_cast<FVvcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FVvCr( double volatile (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FVvCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FVvCr* ft = reinterpret_cast<FVvCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCvcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FCvcr( double const volatile (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FCvcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FCvcr* ft = reinterpret_cast<FCvcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCvCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FCvCr( double const volatile (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FCvCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FCvCr* ft = reinterpret_cast<FCvCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Furcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        Furcr( double& (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type Furcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          Furcr* ft = reinterpret_cast<Furcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FurCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FurCr( double& (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FurCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FurCr* ft = reinterpret_cast<FurCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class Fcrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        Fcrcr( double const& (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type Fcrcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          Fcrcr* ft = reinterpret_cast<Fcrcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FcrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FcrCr( double const& (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FcrCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FcrCr* ft = reinterpret_cast<FcrCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FVrcr( double volatile& (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FVrcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FVrcr* ft = reinterpret_cast<FVrcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FVrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FVrCr( double volatile& (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FVrCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FVrCr* ft = reinterpret_cast<FVrCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCrcr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FCrcr( double const volatile& (*const f)(gsl::vector const&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FCrcr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FCrcr* ft = reinterpret_cast<FCrcr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
      };
      /**
       * Subclass that allows us to use arbitrary functions.
       */
      class FCrCr : public base_F {
      public:
        /**
         * The constructor requires a function.
         * @param f A function.
         * @param dim The number of arguments the function takes.
         */
        FCrCr( double const volatile& (*const f)(gsl::vector const volatile&), size_t const dim ) : f( f ), xv( dim ){}
        /**
         * @return A function for gsl_monte_function.
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
         * A static function that gsl::monte::function can use.
         * @param x The function argument.
         * @param dim The length of x.
         * @param params Always a pointer to an object of type FCrCr
         * @return The function result.
         */
        static double fn( double* x, size_t dim, void* params ){
          FCrCr* ft = reinterpret_cast<FCrCr*>( params );
          if( dim != ft->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          ft->xv.get()->data = x;
          return ft->f( ft->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fuvcru( T& c, double (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fuvcru<T>* const cl = reinterpret_cast<Fuvcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvCru( T& c, double (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvCru<T>* const cl = reinterpret_cast<FuvCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fcvcru( T& c, double const (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fcvcru<T>* const cl = reinterpret_cast<Fcvcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvCru( T& c, double const (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvCru<T>* const cl = reinterpret_cast<FcvCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvcru( T& c, double volatile (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvcru<T>* const cl = reinterpret_cast<FVvcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvCru( T& c, double volatile (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvCru<T>* const cl = reinterpret_cast<FVvCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvcru( T& c, double const volatile (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvcru<T>* const cl = reinterpret_cast<FCvcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvCru( T& c, double const volatile (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvCru<T>* const cl = reinterpret_cast<FCvCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Furcru( T& c, double& (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Furcru<T>* const cl = reinterpret_cast<Furcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurCru( T& c, double& (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurCru<T>* const cl = reinterpret_cast<FurCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fcrcru( T& c, double const& (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fcrcru<T>* const cl = reinterpret_cast<Fcrcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrCru( T& c, double const& (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrCru<T>* const cl = reinterpret_cast<FcrCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrcru( T& c, double volatile& (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrcru<T>* const cl = reinterpret_cast<FVrcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrCru( T& c, double volatile& (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrCru<T>* const cl = reinterpret_cast<FVrCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrcru( T& c, double const volatile& (T::*f)(gsl::vector const&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrcru<T>* const cl = reinterpret_cast<FCrcru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrCru( T& c, double const volatile& (T::*f)(gsl::vector const volatile&), size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrCru<T>* const cl = reinterpret_cast<FCrCru<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fuvcrc( T& c, double (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fuvcrc<T>* const cl = reinterpret_cast<Fuvcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvCrc( T& c, double (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvCrc<T>* const cl = reinterpret_cast<FuvCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fcvcrc( T& c, double const (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fcvcrc<T>* const cl = reinterpret_cast<Fcvcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvCrc( T& c, double const (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvCrc<T>* const cl = reinterpret_cast<FcvCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvcrc( T& c, double volatile (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvcrc<T>* const cl = reinterpret_cast<FVvcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvCrc( T& c, double volatile (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvCrc<T>* const cl = reinterpret_cast<FVvCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvcrc( T& c, double const volatile (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvcrc<T>* const cl = reinterpret_cast<FCvcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvCrc( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvCrc<T>* const cl = reinterpret_cast<FCvCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Furcrc( T& c, double& (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Furcrc<T>* const cl = reinterpret_cast<Furcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurCrc( T& c, double& (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurCrc<T>* const cl = reinterpret_cast<FurCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        Fcrcrc( T& c, double const& (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          Fcrcrc<T>* const cl = reinterpret_cast<Fcrcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrCrc( T& c, double const& (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrCrc<T>* const cl = reinterpret_cast<FcrCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrcrc( T& c, double volatile& (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrcrc<T>* const cl = reinterpret_cast<FVrcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrCrc( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrCrc<T>* const cl = reinterpret_cast<FVrCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrcrc( T& c, double const volatile& (T::*f)(gsl::vector const&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrcrc<T>* const cl = reinterpret_cast<FCrcrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrCrc( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrCrc<T>* const cl = reinterpret_cast<FCrCrc<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvcrV( T& c, double (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvcrV<T>* const cl = reinterpret_cast<FuvcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvCrV( T& c, double (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvCrV<T>* const cl = reinterpret_cast<FuvCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvcrV( T& c, double const (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvcrV<T>* const cl = reinterpret_cast<FcvcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvCrV( T& c, double const (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvCrV<T>* const cl = reinterpret_cast<FcvCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvcrV( T& c, double volatile (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvcrV<T>* const cl = reinterpret_cast<FVvcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvCrV( T& c, double volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvCrV<T>* const cl = reinterpret_cast<FVvCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvcrV( T& c, double const volatile (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvcrV<T>* const cl = reinterpret_cast<FCvcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvCrV( T& c, double const volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvCrV<T>* const cl = reinterpret_cast<FCvCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurcrV( T& c, double& (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurcrV<T>* const cl = reinterpret_cast<FurcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurCrV( T& c, double& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurCrV<T>* const cl = reinterpret_cast<FurCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrcrV( T& c, double const& (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrcrV<T>* const cl = reinterpret_cast<FcrcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrCrV( T& c, double const& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrCrV<T>* const cl = reinterpret_cast<FcrCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrcrV( T& c, double volatile& (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrcrV<T>* const cl = reinterpret_cast<FVrcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrCrV( T& c, double volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrCrV<T>* const cl = reinterpret_cast<FVrCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrcrV( T& c, double const volatile& (T::*f)(gsl::vector const&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrcrV<T>* const cl = reinterpret_cast<FCrcrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrCrV( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrCrV<T>* const cl = reinterpret_cast<FCrCrV<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvcrC( T& c, double (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvcrC<T>* const cl = reinterpret_cast<FuvcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FuvCrC( T& c, double (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FuvCrC<T>* const cl = reinterpret_cast<FuvCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvcrC( T& c, double const (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvcrC<T>* const cl = reinterpret_cast<FcvcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcvCrC( T& c, double const (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcvCrC<T>* const cl = reinterpret_cast<FcvCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvcrC( T& c, double volatile (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvcrC<T>* const cl = reinterpret_cast<FVvcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVvCrC( T& c, double volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVvCrC<T>* const cl = reinterpret_cast<FVvCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvcrC( T& c, double const volatile (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvcrC<T>* const cl = reinterpret_cast<FCvcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCvCrC( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCvCrC<T>* const cl = reinterpret_cast<FCvCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurcrC( T& c, double& (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurcrC<T>* const cl = reinterpret_cast<FurcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FurCrC( T& c, double& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FurCrC<T>* const cl = reinterpret_cast<FurCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrcrC( T& c, double const& (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrcrC<T>* const cl = reinterpret_cast<FcrcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FcrCrC( T& c, double const& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FcrCrC<T>* const cl = reinterpret_cast<FcrCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrcrC( T& c, double volatile& (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrcrC<T>* const cl = reinterpret_cast<FVrcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FVrCrC( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FVrCrC<T>* const cl = reinterpret_cast<FVrCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrcrC( T& c, double const volatile& (T::*f)(gsl::vector const&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrcrC<T>* const cl = reinterpret_cast<FCrcrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
         * @param dim The number of arguments the function takes.
         */
        FCrCrC( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim )
          : xv( dim ), c( c ), f( f ){}
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
        static double function_s( double* x, size_t dim, void* params ){
          FCrCrC<T>* const cl = reinterpret_cast<FCrCrC<T>*>( params );
          if( dim != cl->xv.size() )
            GSL_ERROR( "Number of arguments and dim do not match.", GSL_EBADLEN );
          cl->xv.get()->data = x;
          return (cl->c.*cl->f)( cl->xv );
        }
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
	dim = 0;
	params = 0;
	count = 0; // initially nullptr will do
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double volatile (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double volatile (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const volatile (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const volatile (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double& (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Furcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double& (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const& (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const& (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double volatile& (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double volatile& (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const volatile& (*const f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrcr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function.
       * @param f The function.
       * @param dim The number of parameters of the function.
       */
      explicit function( double const volatile& (*const f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrCr( f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#endif
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
#ifndef DOXYGEN_SKIP
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Furcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrcru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&), size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrCru<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fuvcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcvcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Furcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new Fcrcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrc<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrV<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FuvCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcvCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVvCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCvCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FurCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FcrCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FVrCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrcrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
        // just plausibly we could fail to allocate count: no further action needed.
        count = new size_t;
        *count = 1; // initially there is just one reference to f
      }
      /**
       * Construct from a function object and a suitable member function.
       * @param c The function object.
       * @param f The member function.
       * @param dim The number of parameters of the function.
       */
      template<typename T> function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) const volatile, size_t const dim ){
        base_f = 0;
        params = 0;
        base_f = new FCrCrC<T>( c, f, dim );
        params = base_f;
        this->f = base_f->function();
        this->dim = dim;
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
	dim = v.dim;
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
	dim = v.dim;
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
	std::swap( dim, v.dim );
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
	std::swap( dim, v.dim );
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
       * This is the function that gsl_monte_function points to if
       * function is constructed from a function object.
       * @param x A double.
       * @param dim The number of function arguments.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double fn( double* x, size_t dim, void* params ){
	base_F* const b = reinterpret_cast<base_F*>( params );
	return b->function()( x, dim, params );
      }
    };
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&) , size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  const, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double volatile& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
#endif
#ifndef DOXYGEN_SKIP
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
    /**
     * Make a gsl::monte::function from a function object and a suitable member function.
     * @param c The function object.
     * @param f The member function.
     * @param dim The number of parameters of the function.
     */
    template<typename T>
    inline function make_function( T& c, double const volatile& (T::*f)(gsl::vector const volatile&)  const volatile, size_t const dim ){
      function fn( c, f, dim );
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
