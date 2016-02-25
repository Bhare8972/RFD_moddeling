/*
 * $Id: function_scl.hpp 289 2012-12-04 15:32:17Z jdl3 $
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

#ifndef CCGSL_FUNCTION_SCL_HPP
#define CCGSL_FUNCTION_SCL_HPP

namespace gsl {
  /**
   * Class that extends gsl_function so that it can be constructed
   * from arbitrary function objects. The arbitrary function object
   * dould be a object of a class containing a member function whose
   * argument and return values are doubles. The member function may be
   * declared const.
   *
   * To construct an object of this class from a function object t of class T, use, for example,
   * @code
   * gsl::function f( t, &T::operator() )
   * @endcode
   * where T::operator is a member function whose single argument is a double. To assign, use,
   * for example,
   * @code
   * f = make_function_scl( t, &T::operator() )
   * @endcode
   * There are multiple forms of the function_scl constructor and make_function_scl(). These should
   * allow you to construct a function_scl obejct from any reasonable function object and quite a
   * lot of unreasonable ones too. If f is an object of class gsl_function_scl,
   * it is always possible to pass &f to a gsl function that requires a gsl_function* argument.
   *
   * This class is designed for flexibility rather than efficient copying.
   * So if objects are to be copied frequently, consider using shared pointers
   * or the like. Moving is defined for C++11.
   *
   * The class extends gsl_function and is called gsl::function_scl to contrast
   * with gsl_function_vec and avoid a name conflict if gsl_function::function were used.
   * If constructed from a function object, gsl_function::params is always the
   * function object and gsl_function::function calls the member function using
   * params to obtain an object to call the member function from.
   */
  class function_scl : public gsl_function {
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
       * Virtual destructor
       */
      virtual ~base_F(){};
      /**
       * This must be overriden.
       * @return a function for gsl_function.
       */
      virtual function_t function() = 0;
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fuvuv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fuvuv( double (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fuvuv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fuvuv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fuvur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fuvur( double (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fuvur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fuvur*>( params )->f( x ); }
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
      Fuvcr( double (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fuvcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fuvcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FuvVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FuvVr( double (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FuvVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FuvVr*>( params )->f( x ); }
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
      FuvCr( double (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FuvCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FuvCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fcvuv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fcvuv( double const (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcvuv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcvuv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fcvur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fcvur( double const (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcvur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcvur*>( params )->f( x ); }
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
      Fcvcr( double const (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcvcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcvcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FcvVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FcvVr( double const (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FcvVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FcvVr*>( params )->f( x ); }
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
      FcvCr( double const (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FcvCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FcvCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVvuv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVvuv( double volatile (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVvuv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVvuv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVvur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVvur( double volatile (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVvur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVvur*>( params )->f( x ); }
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
      FVvcr( double volatile (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVvcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVvcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVvVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVvVr( double volatile (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVvVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVvVr*>( params )->f( x ); }
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
      FVvCr( double volatile (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVvCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVvCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCvuv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCvuv( double const volatile (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCvuv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCvuv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCvur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCvur( double const volatile (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCvur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCvur*>( params )->f( x ); }
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
      FCvcr( double const volatile (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCvcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCvcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCvVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCvVr( double const volatile (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCvVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCvVr*>( params )->f( x ); }
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
      FCvCr( double const volatile (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCvCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCvCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Furuv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Furuv( double& (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double& (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Furuv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Furuv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Furur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Furur( double& (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double& (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Furur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Furur*>( params )->f( x ); }
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
      Furcr( double& (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double& (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Furcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Furcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FurVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FurVr( double& (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double& (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FurVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FurVr*>( params )->f( x ); }
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
      FurCr( double& (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double& (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FurCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FurCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fcruv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fcruv( double const& (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const& (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcruv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcruv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class Fcrur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      Fcrur( double const& (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const& (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcrur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcrur*>( params )->f( x ); }
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
      Fcrcr( double const& (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const& (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type Fcrcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<Fcrcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FcrVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FcrVr( double const& (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const& (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FcrVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FcrVr*>( params )->f( x ); }
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
      FcrCr( double const& (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const& (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FcrCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FcrCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVruv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVruv( double volatile& (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile& (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVruv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVruv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVrur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVrur( double volatile& (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile& (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVrur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVrur*>( params )->f( x ); }
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
      FVrcr( double volatile& (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile& (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVrcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVrcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FVrVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FVrVr( double volatile& (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile& (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVrVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVrVr*>( params )->f( x ); }
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
      FVrCr( double volatile& (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double volatile& (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FVrCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FVrCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCruv : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCruv( double const volatile& (*const f)(double) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile& (*const f)(double);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCruv
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCruv*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCrur : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCrur( double const volatile& (*const f)(double&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile& (*const f)(double&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCrur
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCrur*>( params )->f( x ); }
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
      FCrcr( double const volatile& (*const f)(double const&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile& (*const f)(double const&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCrcr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCrcr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary functions.
     */
    class FCrVr : public base_F {
    public:
      /**
       * The constructor requires a function.
       * @param f A function.
       */
      FCrVr( double const volatile& (*const f)(double volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile& (*const f)(double volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCrVr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCrVr*>( params )->f( x ); }
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
      FCrCr( double const volatile& (*const f)(double const volatile&) ) : f( f ){}
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &fn; }
    private:
      /**
       * The original function.
       */
      double const volatile& (*const f)(double const volatile&);
      /**
       * A static function that gsl::function_scl can use.
       * @param x The function argument.
       * @param params Always a pointer to an object of type FCrCr
       * @return The function result.
       */
      static double fn( double x, void* params ){
        return reinterpret_cast<FCrCr*>( params )->f( x ); }
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvuvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvuvu( T& c, double (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvuvu<T>* const cl = reinterpret_cast<Fuvuvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvuru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvuru( T& c, double (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvuru<T>* const cl = reinterpret_cast<Fuvuru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvcru( T& c, double (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvcru<T>* const cl = reinterpret_cast<Fuvcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvVru( T& c, double (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvVru<T>* const cl = reinterpret_cast<FuvVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvCru( T& c, double (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvCru<T>* const cl = reinterpret_cast<FuvCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvuvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvuvu( T& c, double const (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvuvu<T>* const cl = reinterpret_cast<Fcvuvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvuru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvuru( T& c, double const (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvuru<T>* const cl = reinterpret_cast<Fcvuru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvcru( T& c, double const (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvcru<T>* const cl = reinterpret_cast<Fcvcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvVru( T& c, double const (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvVru<T>* const cl = reinterpret_cast<FcvVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvCru( T& c, double const (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvCru<T>* const cl = reinterpret_cast<FcvCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvuvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvuvu( T& c, double volatile (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvuvu<T>* const cl = reinterpret_cast<FVvuvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvuru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvuru( T& c, double volatile (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvuru<T>* const cl = reinterpret_cast<FVvuru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvcru( T& c, double volatile (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvcru<T>* const cl = reinterpret_cast<FVvcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvVru( T& c, double volatile (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvVru<T>* const cl = reinterpret_cast<FVvVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvCru( T& c, double volatile (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvCru<T>* const cl = reinterpret_cast<FVvCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvuvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvuvu( T& c, double const volatile (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvuvu<T>* const cl = reinterpret_cast<FCvuvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvuru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvuru( T& c, double const volatile (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvuru<T>* const cl = reinterpret_cast<FCvuru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvcru( T& c, double const volatile (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvcru<T>* const cl = reinterpret_cast<FCvcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvVru( T& c, double const volatile (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvVru<T>* const cl = reinterpret_cast<FCvVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvCru( T& c, double const volatile (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvCru<T>* const cl = reinterpret_cast<FCvCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Furuvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Furuvu( T& c, double& (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Furuvu<T>* const cl = reinterpret_cast<Furuvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fururu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fururu( T& c, double& (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fururu<T>* const cl = reinterpret_cast<Fururu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Furcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Furcru( T& c, double& (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Furcru<T>* const cl = reinterpret_cast<Furcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurVru( T& c, double& (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurVru<T>* const cl = reinterpret_cast<FurVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurCru( T& c, double& (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurCru<T>* const cl = reinterpret_cast<FurCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcruvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcruvu( T& c, double const& (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcruvu<T>* const cl = reinterpret_cast<Fcruvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcruru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcruru( T& c, double const& (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcruru<T>* const cl = reinterpret_cast<Fcruru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcrcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcrcru( T& c, double const& (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcrcru<T>* const cl = reinterpret_cast<Fcrcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrVru( T& c, double const& (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrVru<T>* const cl = reinterpret_cast<FcrVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrCru( T& c, double const& (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrCru<T>* const cl = reinterpret_cast<FcrCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVruvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVruvu( T& c, double volatile& (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVruvu<T>* const cl = reinterpret_cast<FVruvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVruru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVruru( T& c, double volatile& (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVruru<T>* const cl = reinterpret_cast<FVruru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrcru( T& c, double volatile& (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrcru<T>* const cl = reinterpret_cast<FVrcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrVru( T& c, double volatile& (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrVru<T>* const cl = reinterpret_cast<FVrVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrCru( T& c, double volatile& (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrCru<T>* const cl = reinterpret_cast<FVrCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCruvu : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCruvu( T& c, double const volatile& (T::*f)(double) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCruvu<T>* const cl = reinterpret_cast<FCruvu<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCruru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCruru( T& c, double const volatile& (T::*f)(double&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCruru<T>* const cl = reinterpret_cast<FCruru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrcru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrcru( T& c, double const volatile& (T::*f)(double const&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrcru<T>* const cl = reinterpret_cast<FCrcru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrVru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrVru( T& c, double const volatile& (T::*f)(double volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrVru<T>* const cl = reinterpret_cast<FCrVru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrCru : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrCru( T& c, double const volatile& (T::*f)(double const volatile&) )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrCru<T>* const cl = reinterpret_cast<FCrCru<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const volatile&);
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvuvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvuvc( T& c, double (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvuvc<T>* const cl = reinterpret_cast<Fuvuvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvurc( T& c, double (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvurc<T>* const cl = reinterpret_cast<Fuvurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fuvcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fuvcrc( T& c, double (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fuvcrc<T>* const cl = reinterpret_cast<Fuvcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvVrc( T& c, double (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvVrc<T>* const cl = reinterpret_cast<FuvVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvCrc( T& c, double (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvCrc<T>* const cl = reinterpret_cast<FuvCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvuvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvuvc( T& c, double const (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvuvc<T>* const cl = reinterpret_cast<Fcvuvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvurc( T& c, double const (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvurc<T>* const cl = reinterpret_cast<Fcvurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcvcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcvcrc( T& c, double const (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcvcrc<T>* const cl = reinterpret_cast<Fcvcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvVrc( T& c, double const (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvVrc<T>* const cl = reinterpret_cast<FcvVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvCrc( T& c, double const (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvCrc<T>* const cl = reinterpret_cast<FcvCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvuvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvuvc( T& c, double volatile (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvuvc<T>* const cl = reinterpret_cast<FVvuvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvurc( T& c, double volatile (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvurc<T>* const cl = reinterpret_cast<FVvurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvcrc( T& c, double volatile (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvcrc<T>* const cl = reinterpret_cast<FVvcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvVrc( T& c, double volatile (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvVrc<T>* const cl = reinterpret_cast<FVvVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvCrc( T& c, double volatile (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvCrc<T>* const cl = reinterpret_cast<FVvCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvuvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvuvc( T& c, double const volatile (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvuvc<T>* const cl = reinterpret_cast<FCvuvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvurc( T& c, double const volatile (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvurc<T>* const cl = reinterpret_cast<FCvurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvcrc( T& c, double const volatile (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvcrc<T>* const cl = reinterpret_cast<FCvcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvVrc( T& c, double const volatile (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvVrc<T>* const cl = reinterpret_cast<FCvVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvCrc( T& c, double const volatile (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvCrc<T>* const cl = reinterpret_cast<FCvCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Furuvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Furuvc( T& c, double& (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Furuvc<T>* const cl = reinterpret_cast<Furuvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fururc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fururc( T& c, double& (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fururc<T>* const cl = reinterpret_cast<Fururc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Furcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Furcrc( T& c, double& (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Furcrc<T>* const cl = reinterpret_cast<Furcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurVrc( T& c, double& (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurVrc<T>* const cl = reinterpret_cast<FurVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurCrc( T& c, double& (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurCrc<T>* const cl = reinterpret_cast<FurCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcruvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcruvc( T& c, double const& (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcruvc<T>* const cl = reinterpret_cast<Fcruvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcrurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcrurc( T& c, double const& (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcrurc<T>* const cl = reinterpret_cast<Fcrurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class Fcrcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      Fcrcrc( T& c, double const& (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        Fcrcrc<T>* const cl = reinterpret_cast<Fcrcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrVrc( T& c, double const& (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrVrc<T>* const cl = reinterpret_cast<FcrVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrCrc( T& c, double const& (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrCrc<T>* const cl = reinterpret_cast<FcrCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVruvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVruvc( T& c, double volatile& (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVruvc<T>* const cl = reinterpret_cast<FVruvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrurc( T& c, double volatile& (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrurc<T>* const cl = reinterpret_cast<FVrurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrcrc( T& c, double volatile& (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrcrc<T>* const cl = reinterpret_cast<FVrcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrVrc( T& c, double volatile& (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrVrc<T>* const cl = reinterpret_cast<FVrVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrCrc( T& c, double volatile& (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrCrc<T>* const cl = reinterpret_cast<FVrCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCruvc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCruvc( T& c, double const volatile& (T::*f)(double) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCruvc<T>* const cl = reinterpret_cast<FCruvc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrurc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrurc( T& c, double const volatile& (T::*f)(double&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrurc<T>* const cl = reinterpret_cast<FCrurc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrcrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrcrc( T& c, double const volatile& (T::*f)(double const&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrcrc<T>* const cl = reinterpret_cast<FCrcrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrVrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrVrc( T& c, double const volatile& (T::*f)(double volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrVrc<T>* const cl = reinterpret_cast<FCrVrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrCrc : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrCrc( T& c, double const volatile& (T::*f)(double const volatile&) const )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrCrc<T>* const cl = reinterpret_cast<FCrCrc<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const volatile&) const;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvuvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvuvV( T& c, double (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvuvV<T>* const cl = reinterpret_cast<FuvuvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvurV( T& c, double (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvurV<T>* const cl = reinterpret_cast<FuvurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvcrV( T& c, double (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvcrV<T>* const cl = reinterpret_cast<FuvcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvVrV( T& c, double (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvVrV<T>* const cl = reinterpret_cast<FuvVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvCrV( T& c, double (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvCrV<T>* const cl = reinterpret_cast<FuvCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvuvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvuvV( T& c, double const (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvuvV<T>* const cl = reinterpret_cast<FcvuvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvurV( T& c, double const (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvurV<T>* const cl = reinterpret_cast<FcvurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvcrV( T& c, double const (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvcrV<T>* const cl = reinterpret_cast<FcvcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvVrV( T& c, double const (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvVrV<T>* const cl = reinterpret_cast<FcvVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvCrV( T& c, double const (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvCrV<T>* const cl = reinterpret_cast<FcvCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvuvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvuvV( T& c, double volatile (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvuvV<T>* const cl = reinterpret_cast<FVvuvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvurV( T& c, double volatile (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvurV<T>* const cl = reinterpret_cast<FVvurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvcrV( T& c, double volatile (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvcrV<T>* const cl = reinterpret_cast<FVvcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvVrV( T& c, double volatile (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvVrV<T>* const cl = reinterpret_cast<FVvVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvCrV( T& c, double volatile (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvCrV<T>* const cl = reinterpret_cast<FVvCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvuvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvuvV( T& c, double const volatile (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvuvV<T>* const cl = reinterpret_cast<FCvuvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvurV( T& c, double const volatile (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvurV<T>* const cl = reinterpret_cast<FCvurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvcrV( T& c, double const volatile (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvcrV<T>* const cl = reinterpret_cast<FCvcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvVrV( T& c, double const volatile (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvVrV<T>* const cl = reinterpret_cast<FCvVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvCrV( T& c, double const volatile (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvCrV<T>* const cl = reinterpret_cast<FCvCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuruvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuruvV( T& c, double& (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuruvV<T>* const cl = reinterpret_cast<FuruvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FururV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FururV( T& c, double& (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FururV<T>* const cl = reinterpret_cast<FururV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurcrV( T& c, double& (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurcrV<T>* const cl = reinterpret_cast<FurcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurVrV( T& c, double& (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurVrV<T>* const cl = reinterpret_cast<FurVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurCrV( T& c, double& (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurCrV<T>* const cl = reinterpret_cast<FurCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcruvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcruvV( T& c, double const& (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcruvV<T>* const cl = reinterpret_cast<FcruvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrurV( T& c, double const& (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrurV<T>* const cl = reinterpret_cast<FcrurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrcrV( T& c, double const& (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrcrV<T>* const cl = reinterpret_cast<FcrcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrVrV( T& c, double const& (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrVrV<T>* const cl = reinterpret_cast<FcrVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrCrV( T& c, double const& (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrCrV<T>* const cl = reinterpret_cast<FcrCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVruvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVruvV( T& c, double volatile& (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVruvV<T>* const cl = reinterpret_cast<FVruvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrurV( T& c, double volatile& (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrurV<T>* const cl = reinterpret_cast<FVrurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrcrV( T& c, double volatile& (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrcrV<T>* const cl = reinterpret_cast<FVrcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrVrV( T& c, double volatile& (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrVrV<T>* const cl = reinterpret_cast<FVrVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrCrV( T& c, double volatile& (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrCrV<T>* const cl = reinterpret_cast<FVrCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCruvV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCruvV( T& c, double const volatile& (T::*f)(double) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCruvV<T>* const cl = reinterpret_cast<FCruvV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrurV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrurV( T& c, double const volatile& (T::*f)(double&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrurV<T>* const cl = reinterpret_cast<FCrurV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrcrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrcrV( T& c, double const volatile& (T::*f)(double const&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrcrV<T>* const cl = reinterpret_cast<FCrcrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrVrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrVrV( T& c, double const volatile& (T::*f)(double volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrVrV<T>* const cl = reinterpret_cast<FCrVrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrCrV : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrCrV( T& c, double const volatile& (T::*f)(double const volatile&) volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrCrV<T>* const cl = reinterpret_cast<FCrCrV<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const volatile&) volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvuvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvuvC( T& c, double (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvuvC<T>* const cl = reinterpret_cast<FuvuvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvurC( T& c, double (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvurC<T>* const cl = reinterpret_cast<FuvurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvcrC( T& c, double (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvcrC<T>* const cl = reinterpret_cast<FuvcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvVrC( T& c, double (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvVrC<T>* const cl = reinterpret_cast<FuvVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuvCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuvCrC( T& c, double (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuvCrC<T>* const cl = reinterpret_cast<FuvCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvuvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvuvC( T& c, double const (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvuvC<T>* const cl = reinterpret_cast<FcvuvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvurC( T& c, double const (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvurC<T>* const cl = reinterpret_cast<FcvurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvcrC( T& c, double const (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvcrC<T>* const cl = reinterpret_cast<FcvcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvVrC( T& c, double const (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvVrC<T>* const cl = reinterpret_cast<FcvVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcvCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcvCrC( T& c, double const (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcvCrC<T>* const cl = reinterpret_cast<FcvCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvuvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvuvC( T& c, double volatile (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvuvC<T>* const cl = reinterpret_cast<FVvuvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvurC( T& c, double volatile (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvurC<T>* const cl = reinterpret_cast<FVvurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvcrC( T& c, double volatile (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvcrC<T>* const cl = reinterpret_cast<FVvcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvVrC( T& c, double volatile (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvVrC<T>* const cl = reinterpret_cast<FVvVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVvCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVvCrC( T& c, double volatile (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVvCrC<T>* const cl = reinterpret_cast<FVvCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvuvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvuvC( T& c, double const volatile (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvuvC<T>* const cl = reinterpret_cast<FCvuvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvurC( T& c, double const volatile (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvurC<T>* const cl = reinterpret_cast<FCvurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvcrC( T& c, double const volatile (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvcrC<T>* const cl = reinterpret_cast<FCvcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvVrC( T& c, double const volatile (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvVrC<T>* const cl = reinterpret_cast<FCvVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCvCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCvCrC( T& c, double const volatile (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCvCrC<T>* const cl = reinterpret_cast<FCvCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FuruvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FuruvC( T& c, double& (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FuruvC<T>* const cl = reinterpret_cast<FuruvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FururC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FururC( T& c, double& (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FururC<T>* const cl = reinterpret_cast<FururC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurcrC( T& c, double& (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurcrC<T>* const cl = reinterpret_cast<FurcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurVrC( T& c, double& (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurVrC<T>* const cl = reinterpret_cast<FurVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FurCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FurCrC( T& c, double& (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FurCrC<T>* const cl = reinterpret_cast<FurCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double& (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcruvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcruvC( T& c, double const& (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcruvC<T>* const cl = reinterpret_cast<FcruvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrurC( T& c, double const& (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrurC<T>* const cl = reinterpret_cast<FcrurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrcrC( T& c, double const& (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrcrC<T>* const cl = reinterpret_cast<FcrcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrVrC( T& c, double const& (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrVrC<T>* const cl = reinterpret_cast<FcrVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FcrCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FcrCrC( T& c, double const& (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FcrCrC<T>* const cl = reinterpret_cast<FcrCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const& (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVruvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVruvC( T& c, double volatile& (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVruvC<T>* const cl = reinterpret_cast<FVruvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrurC( T& c, double volatile& (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrurC<T>* const cl = reinterpret_cast<FVrurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrcrC( T& c, double volatile& (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrcrC<T>* const cl = reinterpret_cast<FVrcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrVrC( T& c, double volatile& (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrVrC<T>* const cl = reinterpret_cast<FVrVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FVrCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FVrCrC( T& c, double volatile& (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FVrCrC<T>* const cl = reinterpret_cast<FVrCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double volatile& (T::*f)(double const volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCruvC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCruvC( T& c, double const volatile& (T::*f)(double) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCruvC<T>* const cl = reinterpret_cast<FCruvC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrurC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrurC( T& c, double const volatile& (T::*f)(double&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrurC<T>* const cl = reinterpret_cast<FCrurC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrcrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrcrC( T& c, double const volatile& (T::*f)(double const&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrcrC<T>* const cl = reinterpret_cast<FCrcrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrVrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrVrC( T& c, double const volatile& (T::*f)(double volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrVrC<T>* const cl = reinterpret_cast<FCrVrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double volatile&) const volatile;
    };
    /**
     * Subclass that allows us to use arbitrary non-const
     * function objects.
     */
    template<typename T>
    class FCrCrC : public base_F {
    public:
      /**
       * The constructor requires the function object and
       * a suitable non-const function. Typically this will
       * be &T::operator().
       * @param c The object.
       * @param f A non-const member function.
       */
      FCrCrC( T& c, double const volatile& (T::*f)(double const volatile&) const volatile )
        : c( c ), f(f) {}
      /**
       * Give access to the function from T in F.
       * @param x A double.
       * @return The value of c.*f( x ).
       */
      double operator()( double x ){ return (c.*f)( x ); }
      /**
       * @return A function for gsl_function.
       */
      function_t function(){ return &function_s; }
    private:
      /**
       * The function. This is static so that gsl_function can
       * hold a pointer to it without a function object.
       * @param x A double.
       * @param params The parameters, which are always set to be a pointer
       * to the function object.
       * @return the function object function evaluated at x.
       */
      static double function_s( double x, void* params ){
        FCrCrC<T>* const cl = reinterpret_cast<FCrCrC<T>*>( params );
        return (cl->c.*cl->f)(x); }
      /**
       * Reference to the function object.
       */
      T& c;
      /**
       * Pointer to the function object function.
       */
      double const volatile& (T::*f)(double const volatile&) const volatile;
    };
#endif
    base_F* f;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    function_scl(){
      f = 0;
      function = 0;
      params = 0;
      count = 0; // initially nullptr will do
    }
    /**
     * Could construct from a gsl_function. 
     * @param v The gsl_function
     */
    explicit function_scl( gsl_function& v ){
      // explicitly set f = 0 to indicate nothing to be deleted.
      f = 0;
      // copy
      function = v.function;
      params = v.params;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function. There are multiple different versions of this constructor
     * that allow the argument and return value to be cv-qualified and the argument to be
     * supplied by reference.
     * @param f The function.
     */
    explicit function_scl( double (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvuv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
#ifndef DOXYGEN_SKIP
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FuvVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FuvCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvuv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcvVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcvCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FVvuv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FCvuv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double& (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Furuv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double& (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Furur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double& (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Furcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double& (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FurVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double& (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FurCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const& (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fcruv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const& (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcrur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const& (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcrcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const& (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcrVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const& (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcrCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile& (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FVruv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile& (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile& (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile& (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double volatile& (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile& (*const f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FCruv( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile& (*const f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrur( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile& (*const f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrcr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile& (*const f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrVr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function.
     * @param f The function.
     */
    explicit function_scl( double const volatile& (*const f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrCr( f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
#endif
    /**
     * Construct from a function object and a suitable non-const member function.
     * There are multiple different versions of this constructor
     * that allow the argument, return value and function to be cv-qualified and the argument to be
     * supplied by reference.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvuvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
#ifndef DOXYGEN_SKIP
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvuru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fuvcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FuvVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FuvCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvuvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvuru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcvcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcvVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcvCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FVvuvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvuru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVvCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FCvuvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvuru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCvCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Furuvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fururu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Furcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FurVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FurCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new Fcruvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcruru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new Fcrcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcrVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FcrCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FVruvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FVruru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FVrCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double) ){
      this->f = 0;
      params = 0;
      this->f = new FCruvu<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double&) ){
      this->f = 0;
      params = 0;
      this->f = new FCruru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrcru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrVru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const volatile&) ){
      this->f = 0;
      params = 0;
      this->f = new FCrCru<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new Fuvuvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fuvurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fuvcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FuvVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FuvCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcvuvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcvurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcvcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FcvVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FcvCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new FVvuvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVvurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVvcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVvVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVvCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new FCvuvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCvurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCvcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCvVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCvCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new Furuvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fururc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new Furcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FurVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FurCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcruvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcrurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new Fcrcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FcrVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FcrCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new FVruvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVrurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVrcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVrVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FVrCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double) const ){
      this->f = 0;
      params = 0;
      this->f = new FCruvc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCrurc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCrcrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCrVrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const volatile&) const ){
      this->f = 0;
      params = 0;
      this->f = new FCrCrc<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvuvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvuvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvuvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvuvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuruvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FururV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcruvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVruvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCruvV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrurV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrcrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrVrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const volatile&) volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrCrV<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvuvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuvCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvuvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcvCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvuvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVvCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvuvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCvCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FuruvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FururC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double& (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FurCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcruvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const& (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FcrCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVruvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double volatile& (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FVrCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCruvC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrurC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrcrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrVrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
    /**
     * Construct from a function object and a suitable non-const member function.
     * @param c The function object.
     * @param f The member function.
     */
    template<typename T> function_scl( T& c, double const volatile& (T::*f)(double const volatile&) const volatile ){
      this->f = 0;
      params = 0;
      this->f = new FCrCrC<T>( c, f );
      params = this->f;
      function = &fn;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to f
    }
#endif
    // Copy and move constructors
    // copy constructor
    /**
     * The copy constructor. This shares f. 
     * @param v The function_scl to copy.
     */
    function_scl( function_scl const& v ) : f( v.f ), count( v.count ){
      function = v.function;
      params = v.params;
      if( count != 0 ) ++*count; // function_scl is now shared.
    }
    // assignment operator
    /**
     * The assignment operator. This makes a shared copy.
     * @param v The function_scl to copy
     */
    function_scl& operator=( function_scl const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	delete f;
	delete count;
      }
      // Then copy
      f = v.f;
      function = v.function;
      params = v.params;
      count = v.count;
      if( count != 0 ) ++*count; // function_scl is now shared.
      return *this;
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The function_scl to move.
     */
    function_scl( function_scl&& v ) : f( v.f ), count( nullptr ){
      std::swap( function, v.function );
      std::swap( params, v.params );
      std::swap( count, v.count );
      v.f = nullptr;
    }
    /**
     * Move operator.
     * @param v The function_scl to move.
     * @return A reference to f in this.
     */
    function_scl& operator=( function_scl&& v ){
      std::swap( f, v.f );
      std::swap( function, v.function );
      std::swap( params, v.params );
      std::swap( count, v.count );
      return *this;
    }
#endif
    /**
     * The destructor unshares any shared resource.
     */
    ~function_scl(){ 
      // first, possibly delete anything pointed to by f
      if( count == 0 or --*count == 0 ){
	delete f;
	delete count;
      }
    }
  private:
    /**
     * This is the function that gsl_function points to if
     * function_scl is constructed from a function object.
     * @param x A double.
     * @param params The parameters, which are always set to be a pointer
     * to the function object.
     * @return the function object function evaluated at x.
     */
    static double fn( double x, void* params ){
      base_F* const b = reinterpret_cast<base_F*>( params );
      return b->function()( x, params );
    }
  };    

  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * There are multiple different versions of this function
   * that allow the argument, return value and function of the second argument to be cv-qualified
   * and allow its argument to be supplied by reference.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
#ifndef DOXYGEN_SKIP
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const volatile&) ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const volatile&) const ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const volatile&) volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double& (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const& (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double volatile& (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
  /**
   * Make a gsl::function_scl from a function object and a suitable non-const member function.
   * @param c The function object.
   * @param f The member function.
   */
  template<typename T>
  inline function_scl make_function_scl( T& c, double const volatile& (T::*f)(double const volatile&) const volatile ){
    function_scl fn( c, f );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    return std::move( fn );
#else
    return fn;
#endif
  }
#endif

  /**
   * Evaluate a gsl::function (or gsl_function).
   * @param F The function.
   * @param x The argument.
   * @return The result.
   */
  inline double fn_eval( gsl_function const* const F, double const x ){
    return (F->function)( x, F->params );
  }
  
  /**
   * Evaluate a gsl::function (or gsl_function).
   * @param F The function.
   * @param x The argument.
   * @return The result.
   */
  inline double fn_eval( gsl_function const& F, double const x ){
    return (F.function)( x, F.params );
  }

}

#endif
