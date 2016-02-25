/*
 * $Id: randist.hpp 293 2012-12-17 20:27:36Z jdl3 $
 * Copyright (C) 2010 John D Lamb
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

#ifndef CCGSL_RANDIST_HPP
#define CCGSL_RANDIST_HPP

#include<new>
#include<gsl/gsl_randist.h>
#include"rng.hpp"

namespace gsl {
  /**
   * Namespace for random number distributions. See also cdf.
   */
  namespace ran {
    /**
     * Class for walker algorithm.
     */
    class discrete_t {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      discrete_t(){
	ccgsl_pointer = 0;
	// just plausibly we could fail to allocate count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // Refines random access container
      // Refines assignable
      /**
       * The standard constructor creates a new discrete_t.
       * @param K An integer
       * @param P An array of real values
       */
      explicit discrete_t( size_t K, double const* P  ){
	ccgsl_pointer = gsl_ran_discrete_preproc( K, P );
	// just plausibly we could allocate gsl_ran_discrete_t but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_ran_discrete_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_ran_discrete_t. This is not usually a good idea. In this case
       * you should not use gsl_ran_discrete_free() to deallocate the memory.
       * @param v The workspace
       */
      explicit discrete_t( gsl_ran_discrete_t* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the discrete_t.
       * @param v The discrete_t to copy.
       */
      discrete_t( discrete_t const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The discrete_t to copy
       */
      discrete_t& operator=( discrete_t const& v ){
	// first, possibly delete anything pointed to by this
	if( --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_ran_discrete_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~discrete_t(){
	if( --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_ran_discrete_free( ccgsl_pointer );
	  delete count;
	}
      }
      // Refines equality comparable
      // == operator
      /**
       * Two discrete_t are identically equal if their elements are identical.
       * @param v The discrete_t to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( discrete_t const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two discrete_t are different equal if their elements are not identical.
       * @param v The discrete_t to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( discrete_t const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a discrete_t is nonnegative.
       * @param v The discrete_t to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( discrete_t const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a discrete_t is nonnegative.
       * @param v The discrete_t to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( discrete_t const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a discrete_t is nonnegative.
       * @param v The discrete_t to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( discrete_t const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a discrete_t is nonnegative.
       * @param v The discrete_t to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( discrete_t const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the discrete_t is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two discrete_t. This works even if the discrete_t have different sizes
       * because it swaps pointers.
       * @param v The discrete_t to swap with @c this.
       */
      void swap( discrete_t& v ){
	gsl_ran_discrete_t* tmp = ccgsl_pointer; ccgsl_pointer = v.ccgsl_pointer;
	v.ccgsl_pointer = tmp; size_t* tmp2 = count; count = v.count; v.count = tmp2; }
    private:
      /**
       * The shared pointer
       */
      gsl_ran_discrete_t* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_ran_discrete_t.
       * @return the gsl_ran_discrete_t
       */
      gsl_ran_discrete_t* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_ran_discrete_t.
       * @return @c true or @c falses according as 
       * this is the only discrete_t object sharing the gsl_ran_discrete_t
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many discrete_t objects share this pointer.
       * @return the number of discrete_t objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_ran_discrete_t
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    // Functions
    /**
     * C++ version of gsl_ran_bernoulli().
     * @param r A random number generator
     * @param p Probability of success (1)
     * @return 0 or 1 (success)
     */
    inline unsigned int bernoulli( rng const& r, double p ){
      return gsl_ran_bernoulli( r.get(), p ); } 
    /**
     * C++ version of gsl_ran_bernoulli_pdf().
     * @param k An integer (0 or 1)
     * @param p The probability of success
     * @return The probability of obtaining @c k
     */
    inline double bernoulli_pdf( unsigned int const k, double p ){
      return gsl_ran_bernoulli_pdf( k, p ); } 
    /**
     * C++ version of gsl_ran_beta().
     * @param r A random number generator
     * @param a First parameter
     * @param b Second parameter
     * @return A beta variate
     */
    inline double beta( rng const& r, double const a, double const b ){
      return gsl_ran_beta( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_beta_pdf().
     * @param x A real value
     * @param a First parameter
     * @param b Second parameter
     * @return The density function evaluated at @c x
     */
    inline double beta_pdf( double const x, double const a, double const b ){
      return gsl_ran_beta_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_binomial().
     * @param r A random number generator
     * @param p The probability of getting success in a trial
     * @param n The number of trials
     * @return A random variate giving the number of successes
     */
    inline unsigned int binomial( rng const& r, double p, unsigned int n ){
      return gsl_ran_binomial( r.get(), p, n ); } 
    /**
     * C++ version of gsl_ran_binomial_knuth().
     * @param r A random number generator
     * @param p The probability of getting success in a trial
     * @param n The number of trials
     * @return A random variate giving the number of successes
     */
    inline unsigned int binomial_knuth( rng const& r, double p, unsigned int n ){
      return gsl_ran_binomial_knuth( r.get(), p, n ); } 
    /**
     * C++ version of gsl_ran_binomial_tpe().
     * @param r A random number generator
     * @param p The probability of getting success in a trial
     * @param n The number of trials
     * @return A random variate giving the number of successes
     */
    inline unsigned int binomial_tpe( rng const& r, double p, unsigned int n ){
      return gsl_ran_binomial_tpe( r.get(), p, n ); } 
    /**
     * C++ version of gsl_ran_binomial_pdf().
     * @param k The number of successes
     * @param p The probability of success in one trial
     * @param n The number of trials
     * @return The probability of @c k successes in @c n trials
     */
    inline double binomial_pdf( unsigned int const k, double const p, unsigned int const n ){
      return gsl_ran_binomial_pdf( k, p, n ); } 
    /**
     * C++ version of gsl_ran_exponential().
     * @param r A random number generator
     * @param mu The mean value (parameter)
     * @return A value sampled from the exponential distribution
     */
    inline double exponential( rng const& r, double const mu ){
      return gsl_ran_exponential( r.get(), mu ); } 
    /**
     * C++ version of gsl_ran_exponential_pdf().
     * @param x A real value
     * @param mu The mean value (parameter)
     * @return The exponential density evaluated at @c x
     */
    inline double exponential_pdf( double const x, double const mu ){
      return gsl_ran_exponential_pdf( x, mu ); } 
    /**
     * C++ version of gsl_ran_exppow().
     * @param r A random number generator
     * @param a The first parameter
     * @param b The second parameter
     * @return A value sampled from the exponential power distribution
     */
    inline double exppow( rng const& r, double const a, double const b ){
      return gsl_ran_exppow( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_exppow_pdf().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The exponential power density evaluated at @c x
     */
    inline double exppow_pdf( double const x, double const a, double const b ){
      return gsl_ran_exppow_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_cauchy().
     * @param r A random number generator
     * @param a The scale parameter
     * @return A value sampled from the Cauchy distribution
     */
    inline double cauchy( rng const& r, double const a ){
      return gsl_ran_cauchy( r.get(), a ); } 
    /**
     * C++ version of gsl_ran_cauchy_pdf().
     * @param x A real value
     * @param a The scale parameter
     * @return The Cauchy density evaluated at @c x
     */
    inline double cauchy_pdf( double const x, double const a ){
      return gsl_ran_cauchy_pdf( x, a ); } 
    /**
     * C++ version of gsl_ran_chisq().
     * @param r A random number generator
     * @param nu The degrees of freedom
     * @return A value sampled from the \f$\chi^2\f$ distribution with @c nu degrees of freedom
     */
    inline double chisq( rng const& r, double const nu ){
      return gsl_ran_chisq( r.get(), nu ); } 
    /**
     * C++ version of gsl_ran_chisq_pdf().
     * @param x A real value
     * @param nu The degrees of freedom
     * @return The \f$\chi^2\f$ denstity evaluated at @c x
     */
    inline double chisq_pdf( double const x, double const nu ){
      return gsl_ran_chisq_pdf( x, nu ); } 
    /**
     * C++ version of gsl_ran_dirichlet().
     * @param r A random number generator
     * @param K The number of parameters
     * @param alpha[] Parameter array of size @c K
     * @param theta[] @c K samples (return values)
     */
    inline void dirichlet( rng const& r, size_t const K, double const alpha[], double theta[] ){
      gsl_ran_dirichlet( r.get(), K, alpha, theta ); } 
    /**
     * C++ version of gsl_ran_dirichlet_pdf().
     * @param K The number of parameters
     * @param alpha[] Parameter array of size @c K
     * @param theta[] @c K real values to evaluate denisty function at
     * @return The density function evaluated at @c theta
     */
    inline double dirichlet_pdf( size_t const K, double const alpha[], double const theta[] ){
      return gsl_ran_dirichlet_pdf( K, alpha, theta ); } 
    /**
     * C++ version of gsl_ran_dirichlet_lnpdf().
     * @param K The number of parameters
     * @param alpha[] Parameter array of size @c K
     * @param theta[] @c K real values to evaluate denisty function a
     * @return The logarithm of the density function evaluated at @c theta
     */
    inline double dirichlet_lnpdf( size_t const K, double const alpha[], double const theta[] ){
      return gsl_ran_dirichlet_lnpdf( K, alpha, theta ); } 
    /**
     * C++ version of gsl_ran_erlang().
     * @param r A random number generator
     * @param a First parameter
     * @param n second parameter
     * @return A value sampled from the Erlang distribution
     */
    inline double erlang( rng const& r, double const a, double const n ){
      return gsl_ran_erlang( r.get(), a, n ); } 
    /**
     * C++ version of gsl_ran_erlang_pdf().
     * @param x A real value
     * @param a First parameter
     * @param n second parameter
     * @return The Erlang density evaluated at @c x
     */
    inline double erlang_pdf( double const x, double const a, double const n ){
      return gsl_ran_erlang_pdf( x, a, n ); } 
    /**
     * C++ version of gsl_ran_fdist().
     * @param r A random number generator
     * @param nu1 First degrees of freedom
     * @param nu2 Second degrees of freedom
     * @return Sampled value from the F distribution
     */
    inline double fdist( rng const& r, double const nu1, double const nu2 ){
      return gsl_ran_fdist( r.get(), nu1, nu2 ); } 
    /**
     * C++ version of gsl_ran_fdist_pdf().
     * @param x A real value
     * @param nu1 First degrees of freedom
     * @param nu2 Second degrees of freedom
     * @return F distribution density evaluated at @c x
     */
    inline double fdist_pdf( double const x, double const nu1, double const nu2 ){
      return gsl_ran_fdist_pdf( x, nu1, nu2 ); } 
    /**
     * C++ version of gsl_ran_flat().
     * @param r A random number generator
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return Sample uniformly distributed in [a,b]
     */
    inline double flat( rng const& r, double const a, double const b ){
      return gsl_ran_flat( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_flat_pdf().
     * @param x A real value
     * @param a Lower parameter value
     * @param b Upper parameter value
     * @return Uniform density evaluated at @c x
     */
    inline double flat_pdf( double x, double const a, double const b ){
      return gsl_ran_flat_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_gamma().
     * @param r A random number generator
     * @param a First parameter
     * @param b Second parameter
     * @return Sample from the gamma distribution
     */
    inline double gamma( rng const& r, double const a, double const b ){
      return gsl_ran_gamma( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_gamma_int().
     * @param r A random number generator
     * @param a Integer value
     * @return Sample from the gamma distribution (integer value)
     */
    inline double gamma_int( rng const& r, unsigned int const a ){
      return gsl_ran_gamma_int( r.get(), a ); } 
    /**
     * C++ version of gsl_ran_gamma_pdf().
     * @param x A real value
     * @param a First parameter
     * @param b Second parameter
     * @return Gamma density evaluated at @c x
     */
    inline double gamma_pdf( double const x, double const a, double const b ){
      return gsl_ran_gamma_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_gamma_mt().
     * @param r A random number generator
     * @param a First parameter
     * @param b Second parameter
     * @return Sample from the gamma distribution
     */
    inline double gamma_mt( rng const& r, double const a, double const b ){
      return gsl_ran_gamma_mt( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_gamma_knuth().
     * @param r A random number generator
     * @param a First parameter
     * @param b Second parameter
     * @return Sample from the gamma distribution
     */
    inline double gamma_knuth( rng const& r, double const a, double const b ){
      return gsl_ran_gamma_knuth( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_gaussian().
     * @param r A random number generator
     * @param sigma The standard deviation (parameter)
     * @return Sample from the Gaussian (normal) distribution
     */
    inline double gaussian( rng const& r, double const sigma ){
      return gsl_ran_gaussian( r.get(), sigma ); } 
    /**
     * C++ version of gsl_ran_gaussian_ratio_method().
     * @param r A random number generator
     * @param sigma The standard deviation (parameter)
     * @return Sample from the Gaussian (normal) distribution
     */
    inline double gaussian_ratio_method( rng const& r, double const sigma ){
      return gsl_ran_gaussian_ratio_method( r.get(), sigma ); } 
    /**
     * C++ version of gsl_ran_gaussian_ziggurat().
     * @param r A random number generator
     * @param sigma The standard deviation (parameter)
     * @return Sample from the Gaussian (normal) distribution
     */
    inline double gaussian_ziggurat( rng const& r, double const sigma ){
      return gsl_ran_gaussian_ziggurat( r.get(), sigma ); } 
    /**
     * C++ version of gsl_ran_gaussian_pdf().
     * @param x A real value
     * @param sigma The standard deviation (parameter)
     * @return Gaussian density evaluated at @c x
     */
    inline double gaussian_pdf( double const x, double const sigma ){
      return gsl_ran_gaussian_pdf( x, sigma ); } 
    /**
     * C++ version of gsl_ran_ugaussian().
     * @param r A random number generator
     * @return Sample value from a unit Gaussian (standard normal) distribution
     */
    inline double ugaussian( rng const& r ){
      return gsl_ran_ugaussian( r.get() ); } 
    /**
     * C++ version of gsl_ran_ugaussian_ratio_method().
     * @param r A random number generator
     * @return Sample value from a unit Gaussian (standard normal) distribution
     */
    inline double ugaussian_ratio_method( rng const& r ){
      return gsl_ran_ugaussian_ratio_method( r.get() ); } 
    /**
     * C++ version of gsl_ran_ugaussian_pdf().
     * @param x A real value
     * @return Unit Gaussian (standard normal) density evaluated at @c x
     */
    inline double ugaussian_pdf( double const x ){
      return gsl_ran_ugaussian_pdf( x ); } 
    /**
     * C++ version of gsl_ran_gaussian_tail().
     * @param r A random number generator
     * @param a Lower limit parameter
     * @param sigma Standard deviation (parameter)
     * @return Sample from the Gaussian tail distribution
     */
    inline double gaussian_tail( rng const& r, double const a, double const sigma ){
      return gsl_ran_gaussian_tail( r.get(), a, sigma ); } 
    /**
     * C++ version of gsl_ran_gaussian_tail_pdf().
     * @param x A real value
     * @param a Lower limit parameter
     * @param sigma Standard deviation (parameter)
     * @return Gaussian tail density evaluated at @c x
     */
    inline double gaussian_tail_pdf( double const x, double const a, double const sigma ){
      return gsl_ran_gaussian_tail_pdf( x, a, sigma ); } 
    /**
     * C++ version of gsl_ran_ugaussian_tail().
     * @param r A random number generator
     * @param a Lower limit parameter
     * @return Sample from the unit Gaussian tail distribution
     */
    inline double ugaussian_tail( rng const& r, double const a ){
      return gsl_ran_ugaussian_tail( r.get(), a ); } 
    /**
     * C++ version of gsl_ran_ugaussian_tail_pdf().
     * @param x A real value
     * @param a lower limit parameter
     * @return Unit Gaussian tail density evaluated at @c x
     */
    inline double ugaussian_tail_pdf( double const x, double const a ){
      return gsl_ran_ugaussian_tail_pdf( x, a ); } 
    /**
     * C++ version of gsl_ran_bivariate_gaussian().
     * @param r A random number generator
     * @param sigma_x The standard deviation (@c x parameter)
     * @param sigma_y The standard deviation (@c y parameter)
     * @param rho The correlation coefficient (parameter)
     * @param x Sample (return value) from distribution
     * @param y Sample (return value) from distribution
     */
    inline void bivariate_gaussian( rng const& r, double sigma_x, double sigma_y,
				    double rho, double* x, double* y ){
      gsl_ran_bivariate_gaussian( r.get(), sigma_x, sigma_y, rho, x, y ); } 
    /**
     * C++ version of gsl_ran_bivariate_gaussian_pdf().
     * @param x A real value
     * @param y Another real value
     * @param sigma_x The standard deviation (@c x parameter)
     * @param sigma_y The standard deviation (@c y parameter)
     * @param rho The correlation coefficient (parameter)
     * @return The bivariate Gaussian density evaluated at @c x
     */
    inline double bivariate_gaussian_pdf( double const x, double const y, double const sigma_x,
					  double const sigma_y, double const rho ){
      return gsl_ran_bivariate_gaussian_pdf( x, y, sigma_x, sigma_y, rho ); } 
    /**
     * C++ version of gsl_ran_landau().
     * @param r A random number generator
     * @return Sample from the Landau distribution
     */
    inline double landau( rng const& r ){
      return gsl_ran_landau( r.get() ); } 
    /**
     * C++ version of gsl_ran_landau_pdf().
     * @param x A real value
     * @return The Landua density evaluated at @c x
     */
    inline double landau_pdf( double const x ){ return gsl_ran_landau_pdf( x ); } 
    /**
     * C++ version of gsl_ran_geometric().
     * @param r A random number generator
     * @param p The probability of success
     * @return Sample value from the geometric distribution
     */
    inline unsigned int geometric( rng const& r, double const p ){
      return gsl_ran_geometric( r.get(), p ); } 
    /**
     * C++ version of gsl_ran_geometric_pdf().
     * @param k The number of trials before first succcess
     * @param p The probability of success
     * @return The probability of obtaining @c k from the distribution
     */
    inline double geometric_pdf( unsigned int const k, double const p ){
      return gsl_ran_geometric_pdf( k, p ); } 
    /**
     * C++ version of gsl_ran_hypergeometric().
     * @param r A random number generator
     * @param n1 The first parameter
     * @param n2 The second parameter
     * @param t The number of samples
     * @return Sample value for number of elements of type 1 from hypergeometric distribution.
     */
    inline unsigned int hypergeometric( rng const& r, unsigned int n1, unsigned int n2, unsigned int t ){
      return gsl_ran_hypergeometric( r.get(), n1, n2, t ); } 
    /**
     * C++ version of gsl_ran_hypergeometric_pdf().
     * @param k An integer
     * @param n1 The first parameter
     * @param n2 The second parameter
     * @param t The number of samples
     * @return Probability of obtaining k elements of type 1
     */
    inline double hypergeometric_pdf( unsigned int const k, unsigned int const n1,
				      unsigned int const n2, unsigned int t ){
      return gsl_ran_hypergeometric_pdf( k, n1, n2, t ); } 
    /**
     * C++ version of gsl_ran_gumbel1().
     * @param r A random number generator
     * @param a The first parameter
     * @param b The second parameter
     * @return Sample from the gumbel1 distribution
     */
    inline double gumbel1( rng const& r, double const a, double const b ){
      return gsl_ran_gumbel1( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_gumbel1_pdf().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return Gumbel1 density evaluated at @c x
     */
    inline double gumbel1_pdf( double const x, double const a, double const b ){
      return gsl_ran_gumbel1_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_gumbel2().
     * @param r A random number generator
     * @param a The first parameter
     * @param b The second parameter
     * @return Sample from the gumbel2 distribution
     */
    inline double gumbel2( rng const& r, double const a, double const b ){
      return gsl_ran_gumbel2( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_gumbel2_pdf().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return Gumbel2 density evaluated at @c x
     */
    inline double gumbel2_pdf( double const x, double const a, double const b ){
      return gsl_ran_gumbel2_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_logistic().
     * @param r A random number generator
     * @param a The parameter
     * @return Sample from the logistic distribution
     */
    inline double logistic( rng const& r, double const a ){
      return gsl_ran_logistic( r.get(), a ); } 
    /**
     * C++ version of gsl_ran_logistic_pdf().
     * @param x A real value
     * @param a The parameter
     * @return Logistic density evaluated at @c x
     */
    inline double logistic_pdf( double const x, double const a ){
      return gsl_ran_logistic_pdf( x, a ); } 
    /**
     * C++ version of gsl_ran_lognormal().
     * @param r A random number generator
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Paramater (standard deviation of corresponding normal distribution)
     * @return Sample from lognormal distribution
     */
    inline double lognormal( rng const& r, double const zeta, double const sigma ){
      return gsl_ran_lognormal( r.get(), zeta, sigma ); } 
    /**
     * C++ version of gsl_ran_lognormal_pdf().
     * @param x A real value
     * @param zeta Parameter (mean of corresponding normal distribution)
     * @param sigma Paramater (standard deviation of corresponding normal distribution)
     * @return Lognormal density evaluated at @c x
     */
    inline double lognormal_pdf( double const x, double const zeta, double const sigma ){
      return gsl_ran_lognormal_pdf( x, zeta, sigma ); } 
    /**
     * C++ version of gsl_ran_logarithmic().
     * @param r A random number generator
     * @param p A parameter
     * @return Sample (integer) from the logarithmic distribution
     */
    inline unsigned int logarithmic( rng const& r, double const p ){
      return gsl_ran_logarithmic( r.get(), p ); } 
    /**
     * C++ version of gsl_ran_logarithmic_pdf().
     * @param k An integer value (distribution outcome)
     * @param p A parameter
     * @return Probability of obtaining @c k
     */
    inline double logarithmic_pdf( unsigned int const k, double const p ){
      return gsl_ran_logarithmic_pdf( k, p ); } 
    /**
     * C++ version of gsl_ran_multinomial().
     * @param r A random number generator
     * @param K Number of outcomes
     * @param N Number of trials
     * @param p[] Array of size @c K of probabilities of outcomes
     * @param n[] Array of sampled outcome values
     */
    inline void multinomial( rng const& r, size_t const K, unsigned int const N,
			     double const p[], unsigned int n[] ){
      gsl_ran_multinomial( r.get(), K, N, p, n ); } 
    /**
     * C++ version of gsl_ran_multinomial_pdf().
     * @param K Number of outcomes
     * @param p[] Array of size @c K of probabilities of outcomes
     * @param n[] Array of numbers of outcomes
     * @return Probability of observing @c n
     */
    inline double multinomial_pdf( size_t const K, double const p[], unsigned int const n[] ){
      return gsl_ran_multinomial_pdf( K, p, n ); } 
    /**
     * C++ version of gsl_ran_multinomial_lnpdf().
     * @param K Number of outcomes
     * @param p[] Array of size @c K of probabilities of outcomes
     * @param n[] Array of numbers of outcomes
     * @return Logarithm of probability of observing @c n
     */
    inline double multinomial_lnpdf( size_t const K, double const p[], unsigned int const n[] ){
      return gsl_ran_multinomial_lnpdf( K, p, n ); } 
    /**
     * C++ version of gsl_ran_negative_binomial().
     * @param r A random number generator
     * @param p Probability of success in a trial
     * @param n Number of trials
     * @return Sample of number of failures before @c n successes
     */
    inline unsigned int negative_binomial( rng const& r, double p, double n ){
      return gsl_ran_negative_binomial( r.get(), p, n ); } 
    /**
     * C++ version of gsl_ran_negative_binomial_pdf().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n Number of successes
     * @return The probability of @c k failures before @c n successes
     */
    inline double negative_binomial_pdf( unsigned int const k, double const p, double n ){
      return gsl_ran_negative_binomial_pdf( k, p, n ); } 
    /**
     * C++ version of gsl_ran_pascal().
     * @param r A random number generator
     * @param p Probability of success in a trial
     * @param n Number of successes
     * @return Sample number of failures before @c n successes
     */
    inline unsigned int pascal( rng const& r, double p, unsigned int n ){
      return gsl_ran_pascal( r.get(), p, n ); } 
    /**
     * C++ version of gsl_ran_pascal_pdf().
     * @param k Number of failures before @c n successes
     * @param p Probability of success in a trial
     * @param n Number of successes
     * @return The probability of @c k failures before @c n successes 
     */
    inline double pascal_pdf( unsigned int const k, double const p, unsigned int n ){
      return gsl_ran_pascal_pdf( k, p, n ); } 
    /**
     * C++ version of gsl_ran_pareto().
     * @param r A random number generator
     * @param a The first parameter
     * @param b The second parameter
     * @return Sample value from the Pareto distribution
     */
    inline double pareto( rng const& r, double a, double const b ){
      return gsl_ran_pareto( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_pareto_pdf().
     * @param x A real value
     * @param a The first parameter
     * @param b The second parameter
     * @return The Pareto density evaluated at @c x
     */
    inline double pareto_pdf( double const x, double const a, double const b ){
      return gsl_ran_pareto_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_poisson().
     * @param r A random number generator
     * @param mu Mean value (parameter)
     * @return Sample from Poisson distribution
     */
    inline unsigned int poisson( rng const& r, double mu ){
      return gsl_ran_poisson( r.get(), mu ); } 
    /**
     * C++ version of gsl_ran_poisson_array().
     * @param r A random number generator
     * @param n An integer
     * @param array[] An array of values (return)
     * @param mu Mean value (parameter)
     */
    inline void poisson_array( rng const& r, size_t n, unsigned int array[], double mu ){
      gsl_ran_poisson_array( r.get(), n, array, mu ); } 
    /**
     * C++ version of gsl_ran_poisson_pdf().
     * @param k Number observed
     * @param mu Mean value (parameter)
     * @return Probability of obtaining @c k from the Poisson distribution
     */
    inline double poisson_pdf( unsigned int const k, double const mu ){
      return gsl_ran_poisson_pdf( k, mu ); } 
    /**
     * C++ version of gsl_ran_rayleigh().
     * @param r A random number generator
     * @param sigma The parameter
     * @return Sample from the Rayleigh distribution
     */
    inline double rayleigh( rng const& r, double const sigma ){
      return gsl_ran_rayleigh( r.get(), sigma ); } 
    /**
     * C++ version of gsl_ran_rayleigh_pdf().
     * @param x A real value
     * @param sigma The parameter
     * @return Rayleigh density sampled at @c x
     */
    inline double rayleigh_pdf( double const x, double const sigma ){
      return gsl_ran_rayleigh_pdf( x, sigma ); } 
    /**
     * C++ version of gsl_ran_rayleigh_tail().
     * @param r A random number generator
     * @param a Lower limit parameter
     * @param sigma Parameter
     * @return Sample from Rayleigh tail distribution
     */
    inline double rayleigh_tail( rng const& r, double const a, double const sigma ){
      return gsl_ran_rayleigh_tail( r.get(), a, sigma ); } 
    /**
     * C++ version of gsl_ran_rayleigh_tail_pdf().
     * @param x A real value
     * @param a Lower limit parameter
     * @param sigma Parameter
     * @return Rayleigh tail density evaluated at @c x
     */
    inline double rayleigh_tail_pdf( double const x, double const a, double const sigma ){
      return gsl_ran_rayleigh_tail_pdf( x, a, sigma ); } 
    /**
     * C++ version of gsl_ran_tdist().
     * @param r A random number generator
     * @param nu Degrees of freedom
     * @return Sample from Student \f$t\f$ distribution
     */
    inline double tdist( rng const& r, double const nu ){
      return gsl_ran_tdist( r.get(), nu ); } 
    /**
     * C++ version of gsl_ran_tdist_pdf().
     * @param x A real value
     * @param nu Degrees of freedom
     * @return Student \f$t\f$ density sampled at @c x
     */
    inline double tdist_pdf( double const x, double const nu ){
      return gsl_ran_tdist_pdf( x, nu ); } 
    /**
     * C++ version of gsl_ran_laplace().
     * @param r A random number generator
     * @param a Width parameter
     * @return Sample from the Laplace distribution
     */
    inline double laplace( rng const& r, double const a ){
      return gsl_ran_laplace( r.get(), a ); } 
    /**
     * C++ version of gsl_ran_laplace_pdf().
     * @param x A real value
     * @param a Width parameter
     * @return Laplace density evaluated at @c x
     */
    inline double laplace_pdf( double const x, double const a ){
      return gsl_ran_laplace_pdf( x, a ); } 
    /**
     * C++ version of gsl_ran_levy().
     * @param r A random number generator
     * @param c Scale parameter
     * @param alpha Exponent (parameter)
     * @return Sample from the Levy distribution
     */
    inline double levy( rng const& r, double const c, double const alpha ){
      return gsl_ran_levy( r.get(), c, alpha ); } 
    /**
     * C++ version of gsl_ran_levy_skew().
     * @param r A random number generator
     * @param c Scale parameter
     * @param alpha Exponent (parameter)
     * @param beta Skewness parameter
     * @return Sample from the Levy skew distribution
     */
    inline double levy_skew( rng const& r, double const c, double const alpha, double const beta ){
      return gsl_ran_levy_skew( r.get(), c, alpha, beta ); } 
    /**
     * C++ version of gsl_ran_weibull().
     * @param r A random number generator
     * @param a First parameter
     * @param b Second parameter
     * @return Sample from the Weibull distribution
     */
    inline double weibull( rng const& r, double const a, double const b ){
      return gsl_ran_weibull( r.get(), a, b ); } 
    /**
     * C++ version of gsl_ran_weibull_pdf().
     * @param x A real value
     * @param a First parameter
     * @param b Second parameter
     * @return Weibull density evaluated at @c x
     */
    inline double weibull_pdf( double const x, double const a, double const b ){
      return gsl_ran_weibull_pdf( x, a, b ); } 
    /**
     * C++ version of gsl_ran_dir_2d().
     * @param r A random number generator
     * @param x Sampled component of direction (return value)
     * @param y Sampled component of direction (retun value)
     */
    inline void dir_2d( rng const& r, double* x, double* y ){
      gsl_ran_dir_2d( r.get(), x, y ); } 
    /**
     * C++ version of gsl_ran_dir_2d_trig_method().
     * @param r A random number generator
     * @param x Sampled component of direction (return value)
     * @param y Sampled component of direction (return value)
     */
    inline void dir_2d_trig_method( rng const& r, double* x, double* y ){
      gsl_ran_dir_2d_trig_method( r.get(), x, y ); } 
    /**
     * C++ version of gsl_ran_dir_3d().
     * @param r A random number generator
     * @param x Sampled component of direction (return value)
     * @param y Sampled component of direction (return value)
     * @param z Sampled component of direction (return value)
     */
    inline void dir_3d( rng const& r, double* x, double* y, double* z ){
      gsl_ran_dir_3d( r.get(), x, y, z ); } 
    /**
     * C++ version of gsl_ran_dir_nd().
     * @param r A random number generator
     * @param n The number of dimensions
     * @param x Array to store @c n sampled direction values
     */
    inline void dir_nd( rng const& r, size_t n, double* x ){
      gsl_ran_dir_nd( r.get(), n, x ); } 
    /**
     * C++ version of gsl_ran_shuffle().
     * @param r A random number generator
     * @param base Array of objects of size @c size
     * @param nmembm Number of elements in @c base
     * @param size Size of objects
     */
    inline void shuffle( rng const& r, void* base, size_t nmembm, size_t size ){
      gsl_ran_shuffle( r.get(), base, nmembm, size ); } 
    /**
     * C++ version of gsl_ran_choose().
     * @param r A random number generator
     * @param dest Destination array
     * @param k Number of elements in destination array
     * @param src Source array
     * @param n Number of elements (at least @c k) in source array
     * @param size Size of elements in @c src or @c dest
     * @return Error code on failure
     */
    inline int choose( rng const& r, void* dest, size_t k, void* src, size_t n, size_t size ){
      return gsl_ran_choose( r.get(), dest, k, src, n, size ); } 
    /**
     * C++ version of gsl_ran_sample().
     * @param r A random number generator
     * @param dest Destination array
     * @param k Number of elements in destination array
     * @param src Spource array
     * @param n Number of elements in source array
     * @param size Size of elements in @c src or @c dest
     */
    inline void sample( rng const& r, void* dest, size_t k, void* src, size_t n, size_t size ){
      gsl_ran_sample( r.get(), dest, k, src, n, size ); } 
    /**
     * C++ version of gsl_ran_discrete().
     * @param r A random number generator
     * @param g A @c gsl::ran::discrete_t object
     * @return Sample value from distribution given by @c g
     */
    inline size_t discrete( rng const& r, discrete_t const& g ){
      return gsl_ran_discrete( r.get(), g.get() ); } 
    /**
     * C++ version of gsl_ran_discrete_pdf().
     * @param k A discrete value k
     * @param g A @c gsl::ran::discrete_t object
     * @return Probability of outcome @c k  value from distribution given by @c g
     */
    inline double discrete_pdf( size_t k, discrete_t const& g ){
      return gsl_ran_discrete_pdf( k, g.get() ); } 
  }
}

#endif
