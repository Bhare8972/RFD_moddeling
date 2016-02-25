/*
 * $Id: multifit.hpp 309 2013-11-17 16:26:16Z jdl3 $
 * Copyright (C) 2011, 2012 John D Lamb
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

#ifndef CCGSL_MULTIFIT_HPP
#define CCGSL_MULTIFIT_HPP

#include<cmath>
#include<gsl/gsl_multifit.h>
#include"vector.hpp"
#include"matrix.hpp"

namespace gsl {
  /**
   * This namespace gives access to all the gsl_fit functions and allows us to use
   * gsl::vector and gsl::matrix shared pointers.
   */
  namespace multifit {
    /**
     * Workspace for general linear least squares with p parameters and n observations.
     */
    class linear_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      linear_workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new linear_workspace with n elements
       * @param n The number of observations in the linear_workspace
       * @param p The number of parameters in the linear_workspace
       */
      explicit linear_workspace( size_t const n, size_t const p ){
	ccgsl_pointer = gsl_multifit_linear_alloc( n, p );
	// just plausibly we could allocate linear_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multifit_linear_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multifit_linear_workspace.
       * This is not usually a good idea. In this case
       * you should not use gsl_multifit_linear_workspace_free() to deallocate the memory.
       * @param v The linear_workspace
       */
      explicit linear_workspace( gsl_multifit_linear_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The linear_workspace to copy.
       */
      linear_workspace( linear_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The linear_workspace to copy
       */
      linear_workspace& operator=( linear_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_multifit_linear_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~linear_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_multifit_linear_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The linear_workspace to move.
	 */
	linear_workspace( linear_workspace&& v )
	  : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The linear_workspace to move.
	 * @return A reference to this.
	 */
	linear_workspace& operator=( linear_workspace&& v ){
	  linear_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two linear_workspace are identically equal if their elements are identical.
       * @param v The linear_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( linear_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two linear_workspace are different equal if their elements are not identical.
       * @param v The linear_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( linear_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a linear_workspace is nonnegative.
       * @param v The linear_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( linear_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a linear_workspace is nonnegative.
       * @param v The linear_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( linear_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a linear_workspace is nonnegative.
       * @param v The linear_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( linear_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a linear_workspace is nonnegative.
       * @param v The linear_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( linear_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the linear_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two linear_workspace. This works even if the linear_workspace have different sizes
       * because it swaps pointers.
       * @param v The linear_workspace to swap with @c this.
       */
      void swap( linear_workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_multifit_linear_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multifit_linear_workspace.
       * @return the gsl_multifit_linear_workspace
       */
      gsl_multifit_linear_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multifit_linear_workspace.
       * @return @c true or @c falses according as 
       * this is the only linear_workspace object sharing the gsl_multifit_linear_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many linear_workspace objects share this pointer.
       * @return the number of linear_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_multifit_linear_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_multifit_linear().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param y The vector of observations of the dependent variable.
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int linear( matrix const& X, vector const& y, vector& c, matrix& cov,
		       double* chisq, linear_workspace& work ){
      return gsl_multifit_linear( X.get(), y.get(), c.get(), cov.get(), chisq, work.get() ); }

    /**
     * C++ version of gsl_multifit_linear_svd().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param y The vector of observations of the dependent variable.
     * @param tol The maximum allowed ratio \f$s_i/s_0\f$, where \f$s_i\f$ is the \f$i\f$th
     * singular value; components of the singular value decomposition with smaller ratio than tol
     * are discarded (return value).
     * @param rank The effective rank after discarding components (return value).
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int linear_svd( matrix const& X, vector const& y, double tol, size_t* rank, vector& c,
			   matrix& cov, double* chisq, linear_workspace& work ){
      return gsl_multifit_linear_svd( X.get(), y.get(), tol, rank, c.get(), cov.get(),
				      chisq, work.get() ); }

    /**
     * C++ version of gsl_multifit_linear_usvd().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param y The vector of observations of the dependent variable.
     * @param tol The maximum allowed ratio \f$s_i/s_0\f$, where \f$s_i\f$ is the \f$i\f$th
     * singular value; components of the singular value decomposition with smaller ratio than tol
     * are discarded (return value).
     * @param rank The effective rank after discarding components (return value).
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int linear_usvd( matrix const& X, vector const& y, double tol, size_t* rank, vector& c,
			    matrix& cov, double* chisq, linear_workspace& work ){
      return gsl_multifit_linear_usvd( X.get(), y.get(), tol, rank, c.get(), cov.get(),
				       chisq, work.get() ); }

    /**
     * C++ version of gsl_multifit_wlinear().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param w A vector of weights, which should be the reciprocals of the variances of the
     * corresponding observations.
     * @param y The vector of observations of the dependent variable.
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The weighted sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int wlinear( matrix const& X, vector const& w, vector const& y, vector& c, matrix& cov,
			double* chisq, linear_workspace& work ){
      return gsl_multifit_wlinear( X.get(), w.get(), y.get(), c.get(), cov.get(), chisq,
				   work.get() ); }

    /**
     * C++ version of gsl_multifit_wlinear_svd().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param w A vector of weights, which should be the reciprocals of the variances of the
     * corresponding observations.
     * @param y The vector of observations of the dependent variable.
     * @param tol The maximum allowed ratio \f$s_i/s_0\f$, where \f$s_i\f$ is the \f$i\f$th
     * singular value; components of the singular value decomposition with smaller ratio than tol
     * are discarded (return value).
     * @param rank The effective rank after discarding components (return value).
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The weighted sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int wlinear_svd( matrix const& X, vector const& w, vector const& y, double tol, size_t* rank,
			    vector& c, matrix& cov, double* chisq, linear_workspace& work ){
      return gsl_multifit_wlinear_svd( X.get(), w.get(), y.get(), tol, rank, c.get(), cov.get(),
				       chisq, work.get() ); }

    /**
     * C++ version of gsl_multifit_wlinear_usvd().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param w A vector of weights, which should be the reciprocals of the variances of the
     * corresponding observations.
     * @param y The vector of observations of the dependent variable.
     * @param tol The maximum allowed ratio \f$s_i/s_0\f$, where \f$s_i\f$ is the \f$i\f$th
     * singular value; components of the singular value decomposition with smaller ratio than tol
     * are discarded (return value).
     * @param rank The effective rank after discarding components (return value).
     * @param c The vector of coefficients (return value).
     * @param cov The covariance matrix (return value).
     * @param chisq The weighted sum of squares of the residuals of the best fit.
     * @param work A linear_workspace object.
     * @return Error code on failure.
     */
    inline int wlinear_usvd( matrix const& X, vector const& w, vector const& y, double tol, size_t* rank,
			     vector& c, matrix& cov, double* chisq, linear_workspace& work ){
      return gsl_multifit_wlinear_usvd( X.get(), w.get(), y.get(), tol, rank, c.get(),
					cov.get(), chisq, work.get() ); }
    
    /**
     * C++ version of gsl_multifit_linear_est().
     * @param x A vector of predictor (independent variable) values.
     * @param c A vector of coefficients (not a return value).
     * @param cov The covariance matrix (not a return value).
     * @param y The predicted dependent variable value (return value).
     * @param y_err Estimate of the standard deviation in y (return value).
     * @return Error code on failure.
     */
    inline int linear_est( vector const& x, vector const& c, matrix const& cov, double* y, double* y_err ){
      return gsl_multifit_linear_est( x.get(), c.get(), cov.get(), y, y_err ); }

    /**
     * C++ version of gsl_multifit_linear_residuals().
     * @param X The matrix of predictor variables, which has one row for each observation and one
     * column for each coefficient.
     * @param y The vector of observations of the dependent variable.
     * @param c A vector of coefficients (not a return value).
     * @param r Vector of residuals from the model \f$y=Xc+\varepsilon\f$ (return value).
     * @return Error code on failure.
     */
    inline int linear_residuals( matrix const& X, vector const& y, vector const& c, vector& r ){
      return gsl_multifit_linear_residuals( X.get(), y.get(), c.get(), r.get() ); }

    /**
     * Convenience typedef.
     */
    typedef gsl_multifit_robust_type const* robust_type;

    /**
     * Workspace for robust linear least squares with p parameters and n observations.
     */
    class robust_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      robust_workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new robust_workspace with n elements
       * @param T The type of workspace.
       * @param n The number of observations in the robust_workspace
       * @param p The number of parameters in the robust_workspace
       */
      explicit robust_workspace( robust_type& T, size_t const n, size_t const p ){
	ccgsl_pointer = gsl_multifit_robust_alloc( T, n, p );
	// just plausibly we could allocate robust_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multifit_robust_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multifit_robust_workspace.
       * This is not usually a good idea. In this case
       * you should not use gsl_multifit_robust_workspace_free() to deallocate the memory.
       * @param v The robust_workspace
       */
      explicit robust_workspace( gsl_multifit_robust_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The robust_workspace to copy.
       */
      robust_workspace( robust_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The robust_workspace to copy
       */
      robust_workspace& operator=( robust_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_multifit_robust_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~robust_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_multifit_robust_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The robust_workspace to move.
	 */
	robust_workspace( robust_workspace&& v )
	  : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The robust_workspace to move.
	 * @return A reference to this.
	 */
	robust_workspace& operator=( robust_workspace&& v ){
	  robust_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two robust_workspace are identically equal if their elements are identical.
       * @param v The robust_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( robust_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two robust_workspace are different equal if their elements are not identical.
       * @param v The robust_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( robust_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a robust_workspace is nonnegative.
       * @param v The robust_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( robust_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a robust_workspace is nonnegative.
       * @param v The robust_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( robust_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a robust_workspace is nonnegative.
       * @param v The robust_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( robust_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a robust_workspace is nonnegative.
       * @param v The robust_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( robust_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the robust_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two robust_workspace. This works even if the robust_workspace have different sizes
       * because it swaps pointers.
       * @param v The robust_workspace to swap with @c this.
       */
      void swap( robust_workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_multifit_robust_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multifit_robust_workspace.
       * @return the gsl_multifit_robust_workspace
       */
      gsl_multifit_robust_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multifit_robust_workspace.
       * @return @c true or @c falses according as 
       * this is the only robust_workspace object sharing the gsl_multifit_robust_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many robust_workspace objects share this pointer.
       * @return the number of robust_workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_multifit_robust_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * Convenience tyepdef.
     */
    typedef gsl_multifit_robust_stats robust_stats;
    
    /**
     * C++ version of gsl_multifit_robust_tune().
     * @param tune The tuning constant used to adjust the residuals at each iteration
     * @param w A robust_workspace
     * @return Error code on failure
     */
    inline int robust_tune( double const tune, robust_workspace& w ){
      return gsl_multifit_robust_tune( tune, w.get() ); }

    /**
     * C++ version of gsl_multifit_robust_name().
     * @param w A robust_workspace
     * @return The name of the robust type
     */
    inline char const* robust_name( robust_workspace const& w ){
      return gsl_multifit_robust_name( w.get() ); }

    /**
     * C++ version of gsl_multifit_robust_statistics().
     * @param w A robust_workspace
     * @return A gsl_multifit_robust_statistics object
     */
    inline robust_stats robust_statistics( robust_workspace const& w ){
      return gsl_multifit_robust_statistics( w.get() ); }

    /**
     * C++ version of gsl_multifit_robust().
     * @param X The matrix of predictor variables
     * @param y The vector of dependent variables
     * @param c The vector of coefficients [return]
     * @param cov The variance–covariance matrix [return]
     * @param w A robust_workspace
     * @return Error code on failure
     */
    inline int robust( matrix const& X, vector const& y, vector& c, matrix& cov,
		       robust_workspace& w ){
      return gsl_multifit_robust( X.get(), y.get(), c.get(), cov.get(), w.get() ); }

    /**
     * C++ version of gsl_multifit_robust_est().
     * @param x A vector of independent variables
     * @param c A vector of coefficients
     * @param cov A variance–covariance matrix
     * @param y The predicted values
     * @param y_err The standard deviation in the predicted values
     * @return Error code on failure
     */
    inline int robust_est( vector const& x, vector const& c, matrix const& cov,
			   double* y, double* y_err ){
      return gsl_multifit_robust_est( x.get(), c.get(), cov.get(), y, y_err ); }
    
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_default(){ return gsl_multifit_robust_default; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_bisquare(){ return gsl_multifit_robust_bisquare; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_cauchy(){ return gsl_multifit_robust_cauchy; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_fair(){ return gsl_multifit_robust_fair; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_huber(){ return gsl_multifit_robust_huber; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_ols(){ return gsl_multifit_robust_ols; }
    /**
     * Robust linear static type.
     * @return the named robust linear estimation type
     */
    inline static robust_type robust_welsch(){ return gsl_multifit_robust_welsch; }
  }
}
#endif
