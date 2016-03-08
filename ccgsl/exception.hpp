/*
 * $Id: exception.hpp 247 2012-08-12 11:22:22Z jdl3 $
 * Copyright (C) 2010 John D Lamb
 * Enum copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

#ifndef CCGSL_EXCEPTION_HPP
#define CCGSL_EXCEPTION_HPP
#include<iostream>
#include<iso646.h>
#include<gsl/gsl_errno.h>

/**
 * @mainpage
 * The @c ccgsl package is designed to provide simple C++ wrappers for the GNU Scientific Library.
 * It grew out of some practical classes designed to make GSL a little easier to use without
 * changing its basic features. It is not meant to be an object oriented version of GSL. Rather it tries
 * to keep most of the efficiency of GSL while letting a C++ compiler simplify the creation and
 * destruction of GSL structs. It also allows better interaction with the C++ standard template library
 * and optionally changes errors into C++ exceptions.
 *
 * Much of the package is created using Ruby scripts to rewrite the GSL header files.
 *
 * Here are some of the features of @c ccgsl.
 * - The only installed files are header files. This means that no extra libraries need to be installed
 *     to get ccgsl to work. Just link the gsl libraries as usual.
 * - Structs that have @c _alloc() and @c _free() functions are replaced with classes that behave like
 *    Java classes of boost shared pointers. They are lightweight shared handles that should be
 *   created as automatic variables. When one of these classes goes out of scope, it deletes the
 *   struct it points to automatically unless it is shared by another class.
 * - The @c ccgsl classes can be used safely and efficiently inside STL containers. So, for example,
 *   @c std::list<gsl::vector> and @c std::vector<gsl::rng> work properly.
 * - The @c vector and @c block classes are STL containers. So you can use the STL algorithms with
 *   them.
 * - Most GSL functions are reimplemented in @c ccgsl namespaces and have nearly the same form
 *  as the corresponding GSL function. So, for example, @c gsl_sf_gamma() becomes
 *  @c gsl::sf::gamma() with the same parameters. Replace pointers to GSL structs with references to
 *  @c ccgsl classes.
 * - You can mix GSL functions freely with @c ccgsl. The @c get() function of a @c ccgsl class gives
 *   a GSL pointer. So, for example @c gsl::vector::get() returns a @c gsl_vector* object.
 * - @c ccgsl can turn on exceptions instead of GSL errors. Thus you can use the simpler form of many
 *   functions within a @c try block instead of testing the result of a function call with an extra
 *   parameter. See the gsl::exception class for details.
 */

/**
 * The gsl package creates an interface to the GNU Scientific Library for C++.
 * It adds little to the library. Instead it allows C++ users to use GSL objects through
 * handle classes rather than through pointers to C structs.  The handle objects construct
 * and destroy the GSL objects and keep a count of the number of references to their GSL
 * objects so that the user need not use pointers. It adds iterators for the vector
 * and matrix classes so that they can be used with the standard template library. It adds limited
 * object orientation. Typically, GSL funtions are typically called through member functions
 * where the member would be the first parameter of the corresponding GSL function.
 * @code gsl_vector_set( v, i, x) @endcode
 * becomes
 * @code w.set( i, x ) @endcode
 * where
 * @c w is an object of the handle class @c gsl::vector.
 *
 * The package allows es only header files and allows C++ exception handling instead of
 * GSL error handling.
 */
namespace gsl {
  /**
   * This class is used to handle gsl exceptions so that gsl can use these rather than
   * the GSL error handling. It defines its own error handler that throws exceptions. But the
   * default behaviour is to rely on the error handling of GSL. Typically C++ users should
   * use @code gsl::exception::enable() @endcode early in their programmes to enable
   * exception handling. Then if a GSL function calls the error handler, you should put this function
   * in a try block and catch gsl::exception objects.
   *
   * To enable and catch the exceptions, use code like the following.
   * @code
   * #include<ccgsl/gsl_exception>
   * ...
   * gsl::exception::enable();
   * ...
   * try {
   *   ...
   * } catch( gsl::exception& e ){
   *   ...
   * }
   * @endcode
   *
   * The gsl default handler prints a
   * short message to @c std::clog. You can change this by deriving your own subclass of exception,
   * say @c my_exception and create a function, say
   * @code
   * void
   * exception::my_handler( char const* reason, char const* file,
   * 				     int line, int gsl_errno ){
   *   throw my_exception( reason, file, line, gsl_errno );
   * }
   * @endcode
   * Then use
   * @code
   * gsl::exception::set_handler( &my_handler );
   * @endcode
   * instead of
   * @code
   * gsl::exception::enable();
   * @endcode
   * so that GSL uses your handler to create exceptions of class @c my_exception.
   */
  class exception {
  public:
    /**
     * The constructor takes the same paramters as a GSL error handler: a description
     * of the reason for the error, a file name, a line number, and an error number.
     * @param reason A message explaining the reason for the exception
     * @param file The name of the file that called the error handler and so created the
     * exception
     * @param line The line number within file
     * @param gsl_errno An error number corresponding to the enum below
     */
    exception( char const* reason, char const* file, int line, int gsl_errno )
      : reason( reason ), file( file ), line( line ), gsl_errno( gsl_errno ){
      if( file != 0 ) std::clog << file << ":";
      if( line != 0 ) std::clog << line << ": ";
      std::clog << "gsl::exception: ";
      char const* message = strerror();
      if( message != 0 )
	std::clog << message; // << ": ";
      else
	std::clog << "undefined error: ";
      if( reason != 0 )
	std::clog << ": " << reason;
      std::clog << std::endl;
    }
    /**
     * Get the message explaining the reason for the error/exception.
     * @return the message explaining the reason for the error/exception
     */
    char const* get_reason() const { return reason; }
    /**
     * Get the name of the file that caused an exception.
     * @return the name of the file that caused an exception
     */
    char const* get_file() const { return file; }
    /**
     * Get the line number at which a GSL error handler caused the exception.
     * @return the line number at which a GSL error handler caused the exception
     */
    int get_line() const { return line; }
    /**
     * Get an error number.
     * @return an error number
     * @see strerror().
     */
    int get_gsl_errno() const { return gsl_errno; }
    /**
     * Get a standard GSL message corresponding to the error number.
     * @return a standard GSL message corresponding to the error number
     */
    char const* strerror() const { return gsl_strerror( gsl_errno ); }
  public:
    /**
     * Set the handler function. This corresponds to gsl_set_error_handler and allows
     * you to set your own error handler. Note that ccgsl provides a default error handler
     * that throws exceptions of class exception.
     * @param handler The new handler
     * @return A pointer to the old handler
     */
    static void (*set_handler( void (*handler)( char const*, char const*, int, int )))
    ( char const*, char const*, int, int ){ return gsl_set_error_handler( handler ); }
    /**
     * Set the handler function to handle exceptions. Use set_handler_gsl_exceptions
     * if you want a pointer to the old handler.
     */
    static void enable(){ set_handler_gsl_exceptions(); }
    /**
     * Set the handler function to handle exceptions.
     * @return A pointer to the old handler
     */
    static void (*set_handler_gsl_exceptions())( char const*, char const*, int, int ){
      return gsl_set_error_handler( handler_gsl_exceptions ); }
    /**
     * Set the handler function to a null handler. This disables any error handling.
     * @return A pointer to the old handler
     */
    static void (*set_handler_off())( char const*, char const*, int, int ){
      return gsl_set_error_handler( handler_off ); }
    /**
     * Enumerated type
     */
    enum {
      GSL_SUCCESS  = 0,
      GSL_FAILURE  = -1,
      GSL_CONTINUE = -2,  /**< iteration has not converged */
      GSL_EDOM     = 1,   /**< input domain error, e.g sqrt(-1) */
      GSL_ERANGE   = 2,   /**< output range error, e.g. exp(1e100) */
      GSL_EFAULT   = 3,   /**< invalid pointer */
      GSL_EINVAL   = 4,   /**< invalid argument supplied by user */
      GSL_EFAILED  = 5,   /**< generic failure */
      GSL_EFACTOR  = 6,   /**< factorization failed */
      GSL_ESANITY  = 7,   /**< sanity check failed - shouldn't happen */
      GSL_ENOMEM   = 8,   /**< malloc failed */
      GSL_EBADFUNC = 9,   /**< problem with user-supplied function */
      GSL_ERUNAWAY = 10,  /**< iterative process is out of control */
      GSL_EMAXITER = 11,  /**< exceeded max number of iterations */
      GSL_EZERODIV = 12,  /**< tried to divide by zero */
      GSL_EBADTOL  = 13,  /**< user specified an invalid tolerance */
      GSL_ETOL     = 14,  /**< failed to reach the specified tolerance */
      GSL_EUNDRFLW = 15,  /**< underflow */
      GSL_EOVRFLW  = 16,  /**< overflow  */
      GSL_ELOSS    = 17,  /**< loss of accuracy */
      GSL_EROUND   = 18,  /**< failed because of roundoff error */
      GSL_EBADLEN  = 19,  /**< matrix, vector lengths are not conformant */
      GSL_ENOTSQR  = 20,  /**< matrix not square */
      GSL_ESING    = 21,  /**< apparent singularity detected */
      GSL_EDIVERGE = 22,  /**< integral or series is divergent */
      GSL_EUNSUP   = 23,  /**< requested feature is not supported by the hardware */
      GSL_EUNIMPL  = 24,  /**< requested feature not (yet) implemented */
      GSL_ECACHE   = 25,  /**< cache limit exceeded */
      GSL_ETABLE   = 26,  /**< table limit exceeded */
      GSL_ENOPROG  = 27,  /**< iteration is not making progress towards solution */
      GSL_ENOPROGJ = 28,  /**< jacobian evaluations are not improving the solution */
      GSL_ETOLF    = 29,  /**< cannot reach the specified tolerance in F */
      GSL_ETOLX    = 30,  /**< cannot reach the specified tolerance in X */
      GSL_ETOLG    = 31,  /**< cannot reach the specified tolerance in gradient */
      GSL_EOF      = 32   /**< end of file */
    };
  private:
    /**
     * The default exception handler for gsl.
     * @param reason A message explaining the reason for the exception
     * @param file The name of the file that called the error handler and so created the
     * exception
     * @param line The line number within file
     * @param gsl_errno An error number corresponding to the enum below
     */
    static void handler_gsl_exceptions( char const* reason, char const* file, int line, int gsl_errno ){
      throw exception( reason, file, line, gsl_errno ); }
    /**
     * The empty exception handler for gsl.
     */
    static void handler_off( char const*, char const*, int, int ){ /* Do nothing */ }
  private:
    /**
     * A message giving a reason for this exception.
     */
    char const* reason;
    /**
     * The file that cause this exception.
     */
    char const* file;
    /**
     * The line number in file at which exception handler was called.
     */
    int const line;
    /**
     * A number indicating error type.
     * @see strerror()
     */
    int const gsl_errno;
  };

}


//my extra exceptions, these are clumsy
class vector_exception: public std::exception //this is wrong. needs to be merged into exception .hpp and generalized
{
  virtual const char* what() const throw()
  {
    return "vector exception";
  }
};

#endif
