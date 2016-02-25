/*
 * $Id: poly.hpp 309 2013-11-17 16:26:16Z jdl3 $
 * Copyright (C) 2012 John D Lamb
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

#ifndef CCGSL_POLY_HPP
#define CCGSL_POLY_HPP

#include<gsl/gsl_poly.h>
#include<gsl/gsl_errno.h>
#include"complex.hpp"

namespace gsl {
  /**
   * Various functions for handling polynomials, represented as sequences of
   * coefficients \f$a_0,\ldots,a_n\f$ for \f$a_0+\cdots+a_nx^n\f$. The sequences can be 
   * stored in arrays (as GSL does) or in any container that has a size() functiion and data()
   * function returning an array of doubles of length container.size(). In particular, there are
   * versions of the GSL functions that contain no size paramater work with a mixture of 
   * std::array, std::vector and gsl::vector objects, which have these properties. It is only
   * possible to mix C-style arrays with containers by using the functions that require an
   * array size and passing the remaining arrays in as container.data().
   */
  namespace poly {
    /**
     * Workspace for solving polynomials. This has to represent complex roots.
     */
    class complex_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      complex_workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new complex_workspace with n elements. This workspace
       * is suitable for solving a polynomial with n components.
       * @param n The number of elements in the complex_workspace
       */
      explicit complex_workspace( size_t const n ){
	ccgsl_pointer = gsl_poly_complex_workspace_alloc( n );
	// just plausibly we could allocate complex_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_poly_complex_workspace_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_poly_complex_workspace.
       * This is not usually a good idea. In this case
       * you should not use gsl_poly_complex_workspace_free() to deallocate the memory.
       * @param v The complex_workspace
       */
      explicit complex_workspace( gsl_poly_complex_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The complex_workspace to copy.
       */
      complex_workspace( complex_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The complex_workspace to copy
       */
      complex_workspace& operator=( complex_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_poly_complex_workspace_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~complex_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_poly_complex_workspace_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The complex_workspace to move.
	 */
	complex_workspace( complex_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The complex_workspace to move.
	 * @return A reference to this.
	 */
	complex_workspace& operator=( complex_workspace&& v ){
	  complex_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two complex_workspace are identically equal if their elements are identical.
       * @param v The complex_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( complex_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two complex_workspace are different equal if their elements are not identical.
       * @param v The complex_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( complex_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a complex_workspace is nonnegative.
       * @param v The complex_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( complex_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a complex_workspace is nonnegative.
       * @param v The complex_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( complex_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a complex_workspace is nonnegative.
       * @param v The complex_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( complex_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a complex_workspace is nonnegative.
       * @param v The complex_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( complex_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the complex_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two complex_workspace. This works even if the complex_workspace have different sizes
       * because it swaps pointers.
       * @param v The complex_workspace to swap with @c this.
       */
      void swap( complex_workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_poly_complex_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_poly_complex_workspace.
       * @return the gsl_poly_complex_workspace
       */
      gsl_poly_complex_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_poly_complex_workspace.
       * @return @c true or @c falses according as 
       * this is the only gen_workspace object sharing the gsl_poly_complex_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many gen_workspace objects share this pointer.
       * @return the number of gen_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_poly_complex_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_poly_eval().
     * @param c Array of polynomial coefficients.
     * @param len Length of array.
     * @param x Value at which to evaluate polynomial.
     * @return Value of polynomial at x.
     */
    inline double eval( double const c[], int const len, double const x ){
      return gsl_poly_eval( c, len, x ); }

    /**
     * C++ version of gsl_poly_eval().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param c Array of polynomial coefficients.
     * @param x Value at which to evaluate polynomial.
     * @return Value of polynomial at x.
     */
    template<typename T>
    inline double eval( T const& c, double const x ){
      return gsl_poly_eval( c.data(), c.size(), x ); }

    /**
     * C++ version of gsl_poly_complex_eval().
     * @param c Array of polynomial coefficients.
     * @param len Length of array.
     * @param z Value at which to evaluate polynomial.
     * @return Value of polynomial at z.
     */
    inline complex complex_eval( double const c[], int const len, gsl::complex const z ){
      return gsl_poly_complex_eval( c, len, z.get() ); }
    
    /**
     * C++ version of gsl_poly_complex_eval().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param c Array of polynomial coefficients.
     * @param z Value at which to evaluate polynomial.
     * @return Value of polynomial at z.
     */
    template<typename T>
    inline complex complex_eval( T const& c, gsl::complex const z ){
      return gsl_poly_complex_eval( c.data(), c.size() , z.get() ); }

    /**
     * C++ version of gsl_poly_eval_derivs().
     * @param c Array of polynomial coefficients.
     * @param lenc Length of array.
     * @param x Value at which to evaluate polynomial.
     * @param res An array containing 0th, 1st, 2nd, ... derivatives of polynomial
     * evaluated at x.
     * @param lenres Largest value of \f$k\f$ at which 
     * to evaluate \f$k\f$th derivative of polynomial.
     * @return Error code on failure.
     */
    inline int eval_derivs( double const c[], size_t const lenc, double const x,
			  double res[], size_t const lenres ){
      return gsl_poly_eval_derivs( c, lenc, x, res, lenres ); }
    
    /**
     * C++ version of gsl_poly_eval_derivs().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param c Array of polynomial coefficients.
     * @param x Value at which to evaluate polynomial.
     * @param res An array containing 0th, 1st, 2nd, ... derivatives of polynomial
     * evaluated at x: res.size() is the largest value of \f$k\f$ at which 
     * to evaluate \f$k\f$th derivative of polynomial.
     * @return Error code on failure.
     */
    template<typename T, typename U>
    inline int eval_derivs( T const& c, double const x, U& res ){
      return gsl_poly_eval_derivs( c.data(), c.size(), x, res.data(), res.size() ); }

    /**
     * C++ version of gsl_poly_dd_init().
     * @param dd An array of length size representing the divided differences (return value).
     * @param x An array of x values of length size (input).
     * @param y An array of y values (input).
     * @param size Number of pairs of points through which interpolating ploynomial passes.
     * @return Error code on failure.
     */
    inline int dd_init( double dd[], double const x[], double const y[], size_t size ){
      return gsl_poly_dd_init( dd, x, y, size ); }
    
    /**
     * C++ version of gsl_poly_dd_init().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param dd An array of length size representing the divided differences (return value).
     * @param x An array of x values of length size (input).
     * @param y An array of y values (input).
     * @return Error code on failure.
     */
    template<typename T, typename U, typename V>
    inline int dd_init( T& dd, U const& x, V const& y ){
      size_t const size = dd.size() < x.size() ?
	( dd.size() < y.size() ? dd.size() : y.size() ) : x.size();
      return gsl_poly_dd_init( dd.data(), x.data(), y.data(), size ); }
    
    /**
     * This function computes a divided-difference representation of the interpolating
     * Hermite polynomial for the points (x, y) stored in the arrays xa and ya of length
     * size. Hermite interpolation constructs polynomials which also match first derivatives
     * dy/dx which are provided in the array dya also of length size. The first derivatives
     * can be incorported into the usual divided-difference algorithm by forming a new dataset
     * \f$z = \{x_0,x_0,x_1,x_1,...\}\f$, which is stored in the array za of length
     * 2*size on output. On output the divided-differences of the Hermite representation
     * are stored in the array dd, also of length 2*size. Using the notation above,
     * \f$dd[k] = [z_0,z_1,...,z_k]\f$. The resulting Hermite polynomial can be evaluated
     * by calling gsl_poly_dd_eval and using za for the input argument xa. 
     * @param dd An array of length size representing the divided differences (return value).
     * @param z The first derivatives
     * @param xa The x-coördinates of the interpolating Hermite polynomial
     * @param ya The y-coördinates of the interpolating Hermite polynomial
     * @param dya First derivatives
     * @param size The size of the array
     * @return Error code on failure.
     */
    inline int
    dd_hermite_init( double dd[], double z[], double const xa[], double const ya[],
		     double const dya[], size_t const size ){
      return gsl_poly_dd_hermite_init( dd, z, xa, ya, dya, size ); }

    /**
     * This function computes a divided-difference representation of the interpolating
     * Hermite polynomial for the points (x, y) stored in the arrays xa and ya of length
     * size. Hermite interpolation constructs polynomials which also match first derivatives
     * dy/dx which are provided in the array dya also of length size. The first derivatives
     * can be incorported into the usual divided-difference algorithm by forming a new dataset
     * \f$z = \{x_0,x_0,x_1,x_1,...\}\f$, which is stored in the array za of length
     * 2*size on output. On output the divided-differences of the Hermite representation
     * are stored in the array dd, also of length 2*size. Using the notation above,
     * \f$dd[k] = [z_0,z_1,...,z_k]\f$. The resulting Hermite polynomial can be evaluated
     * by calling gsl_poly_dd_eval and using za for the input argument xa. 
     * @param dd An array of length size representing the divided differences (return value).
     * @param z The first derivatives
     * @param xa The x-coördinates of the interpolating Hermite polynomial
     * @param ya The y-coördinates of the interpolating Hermite polynomial
     * @param dya First derivatives
     * @param size The size of the array
     * @return Error code on failure.
     */
    template<typename DD, typename Z, typename XA, typename YA, typename DYA>
    inline int
    dd_hermite_init( double dd[], double z[], double const xa[], double const ya[],
		     double const dya[], size_t const size ){
      return gsl_poly_dd_hermite_init( dd, z, xa, ya, dya, size );
    }


    /**
     * C++ version of gsl_poly_dd_eval().
     * @param dd An array of length size representing the divided differences (input).
     * @param xa An array of x values of length size representing the divided differences (input).
     * @param size Number of pairs of points through which interpolating ploynomial passes.
     * @param x Value at which to evaluate polynomial.
     * @return Value of inrterpolating polynomial at x.
     */
    inline double dd_eval( double const dd[], double const xa[], size_t const size,
				double const x ){
      return gsl_poly_dd_eval( dd, xa, size, x ); }

    /**
     * C++ version of gsl_poly_dd_eval().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param dd An array of length size representing the divided differences (input).
     * @param xa An array of x values of length size representing the divided differences (input).
     * @param x Value at which to evaluate polynomial.
     * @return Value of inrterpolating polynomial at x.
     */
    template<typename T, typename U>
    inline double dd_eval( T const& dd, U const& xa, double const x ){
      size_t const size = dd.size() < xa.size() ? dd.size() : xa.size();
      return gsl_poly_dd_eval( dd.data(), xa.data(), size, x ); }

    /**
     * C++ version of gsl_poly_dd_taylor().
     * @param c Array of Taylor coefficients about xp.
     * @param xp Point at which Taylor expansion is computed.
     * @param dd An array of length size representing the divided differences.
     * @param x An array of x values of length size.
     * @param size Number of pairs of points through which interpolating ploynomial passes.
     * @param w A workspace in the form of an array of length size.
     * @return Error code on failure.
     */
    inline int dd_taylor( double c[], double xp, double const dd[], double const x[],
			size_t size, double w[] ){
      return gsl_poly_dd_taylor( c, xp, dd, x, size, w ); }

    /**
     * C++ version of gsl_poly_dd_taylor().
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param c Array of Taylor coefficients about xp.
     * @param xp Point at which Taylor expansion is computed.
     * @param dd An array of length size of c representing the divided differences.
     * @param x An array of x values of length size of c.
     * @param w A workspace in the form of an array of length size of c.
     * @return Error code on failure.
     */
    template<typename C, typename DD, typename X, typename W>
    inline int dd_taylor( C& c, double xp, DD const& dd, X const& x, W& w ){
      size_t const size = c.size() < dd.size() ?
	( c.size() < x.size() ? c.size() : x.size() ) : dd.size();
      if( w.size() < size )
	gsl_error( "workspace too small", __FILE__, __LINE__, GSL_EBADLEN );
      return gsl_poly_dd_taylor( c.data(), xp, dd.data(), x.data(), size, w.data() ); }

    /**
     * C++ version of gsl_poly_solve_quadratic().
     * @param a Coefficient of \f$x^2\f$.
     * @param b Coefficient of \f$x\f$.
     * @param c Constant in quadratic.
     * @param x0 Real root (return value).
     * @param x1 Real root (return value).
     * @return The number of real roots.
     */
    inline int solve_quadratic( double a, double b, double c, double* x0, double* x1 ){
      return gsl_poly_solve_quadratic( a, b, c, x0, x1 ); }

    /**
     * C++ version of gsl_poly_complex_solve_quadratic().
     * @param a Coefficient of \f$x^2\f$.
     * @param b Coefficient of \f$x\f$.
     * @param c Constant in quadratic.
     * @param z0 Root (return value).
     * @param z1 Root (return value).
     * @return Error code on failure.
     */
    inline int complex_solve_quadratic( double a, double b, double c,
					complex& z0, complex& z1 ){
      return gsl_poly_complex_solve_quadratic( a, b, c, &z0.get(), &z1.get() ); }


    /**
     * C++ version of gsl_poly_solve_cubic(). Cubic with coefficient of \f$x^3=1\f$.
     * @param a Coefficient of \f$x^2\f$.
     * @param b Coefficient of \f$x\f$.
     * @param c Constant in cubic.
     * @param x0 Real root (return value).
     * @param x1 Real root (return value).
     * @param x2 Real root (return value).
     * @return The number of real roots.
     */
    inline int solve_cubic( double a, double b, double c, double* x0,
				 double* x1, double* x2 ){
      return gsl_poly_solve_cubic( a, b, c, x0, x1, x2 ); }

    /**
     * C++ version of gsl_poly_complex_solve_cubic().
     * @param a Coefficient of \f$x^2\f$.
     * @param b Coefficient of \f$x\f$.
     * @param c Constant in cubic.
     * @param z0 Root (return value).
     * @param z1 Root (return value).
     * @param z2 Root (return value).
     * @return Error code on failure.
     */
    inline int complex_solve_cubic( double a, double b, double c, complex& z0,
				    complex& z1, complex& z2 ){
      return gsl_poly_complex_solve_cubic( a, b, c, &z0.get(), &z1.get(), &z2.get() ); }

    /**
     * C++ version of gsl_poly_complex_solve().
     * @param a Array of coefficients.
     * @param n Size of a (and workspace).
     * @param w Workspace.
     * @param z Roots (\f$n-1\f$, return value).
     * @return Error code on failure.
     */
    inline int complex_solve( double const* a, size_t n, complex_workspace& w,
			      complex_packed_ptr z ){
      return gsl_poly_complex_solve( a, n, w.get(), z ); }

    /**
     * C++ version of gsl_poly_complex_solve(). Here the actual sizes of a
     * and z should be \f$n\f$ and \f$2(n-1)\f$ because the complex values are
     * stored as pairs of doubles in arrays or vectors of doubles.
     * Works with std::array<double>, std::vector<double> and gsl::vector.
     * @param a Array of coefficients.
     * @param w Workspace (with same size \f$n\f$ as a).
     * @param z Roots (\f$n-1\f$, return value).
     * @return Error code on failure.
     */
    template<typename A, typename Z>
    inline int complex_solve( A const& a, complex_workspace& w, Z& z ){
      size_t const n = a.size();
      size_t const n_2 = z.size();
      if( 2 * n != n_2 + 2 )
	gsl_error( "mismatch in sizes of coefficients and roots",
		   __FILE__, __LINE__, GSL_EBADLEN );
      return gsl_poly_complex_solve( a.data(), n, w.get(), z.data() ); }
  }
  /**
   * Polynomial with complex coefficients.
   */
  namespace complex_poly {
    /**
     * C++ version of gsl_complex_poly_complex_eval().
     * @param c Array of polynomial coefficients.
     * @param len Length of array.
     * @param z Value at which to evaluate polynomial.
     * @return Value of polynomial at z.
     */
    inline complex complex_eval( gsl_complex c[], size_t const len, gsl::complex const z ){
      return gsl_complex_poly_complex_eval( c, len, z.get() ); }
  }
}
#endif
