/*
 * $Id: multifit_nlin.hpp 309 2013-11-17 16:26:16Z jdl3 $
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

#ifndef CCGSL_MULTIFIT_NLIN_HPP
#define CCGSL_MULTIFIT_NLIN_HPP
#include<gsl/gsl_multifit_nlin.h>
#include"multifit_function.hpp"
#include"multifit_function_fdf.hpp"

namespace gsl {
  namespace multifit {
    /**
     * C++ version of gsl_multifit_gradient().
     * @param J the Jacobian matrix
     * @param f the function values
     * @param g the gradient (return value)
     * @return Error code on failure
     */
    inline int gradient( matrix const& J, vector const& f, vector& g ){
      return gsl_multifit_gradient( J.get(), f.get(), g.get() ); }

    /**
     * C++ version of gsl_multifit_covar().
     * @param J The Jacobian matrix
     * @param epsrel relative error (for removing linearly dependent columns)
     * @param covar The covariance matrix (return value)
     * @return Error code on failure
     */
    inline int covar( matrix const& J, double epsrel, matrix& covar ){
      return gsl_multifit_covar( J.get(), epsrel, covar.get() ); }
    
    /**
     * Workspace for solving multidimensional fit problems. Also functions as a namespace
     * for fsolver functions. This also allows for functions to be called on fsolver as an object.
     */
    class fsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_multifit_fsolver_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      fsolver(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new fsolver with n elements.
       * @param T The fsolver type.
       * @param n The number of elements in the fsolver.
       * @param p The number of predictor variables
       */
      explicit fsolver( type const* T, size_t const n, size_t const p ){
	ccgsl_pointer = gsl_multifit_fsolver_alloc( T, n, p );
	// just plausibly we could allocate fsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multifit_fsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multifit_fsolver.
       * This is not usually a good idea. In this case
       * we should not use gsl_multifit_fsolver_free() to deallocate the memory.
       * @param v The fsolver
       */
      explicit fsolver( gsl_multifit_fsolver* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fsolver to copy.
       */
      fsolver( fsolver const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fsolver to copy
       */
      fsolver& operator=( fsolver const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_multifit_fsolver_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fsolver(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_multifit_fsolver_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The fsolver to move.
       */
      fsolver( fsolver&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The fsolver to move.
       * @return A reference to this.
       */
      fsolver& operator=( fsolver&& v ){
	fsolver( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fsolver are identically equal if their elements are identical.
       * @param v The fsolver to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fsolver const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fsolver are different if their elements are not identical.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fsolver const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fsolver is nonnegative.
       * @param v The fsolver to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fsolver const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fsolver is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fsolver objects. This works even if the fsolver have different sizes
       * because it swaps pointers.
       * @param v The fsolver to swap with @c this.
       */
      void swap( fsolver& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_multifit_fsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multifit_fsolver.
       * @return the gsl_multifit_fsolver
       */
      gsl_multifit_fsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multifit_fsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multifit_fsolver.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fsolver objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_multifit_fsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multifit_fsolver_set().
       * @param s The fsolver.
       * @param f The function.
       * @param x The initial guess of the fit.
       * @return Error code on failure.
       */
      inline static int set( fsolver& s, function* f, vector const& x ){
	int const result = gsl_multifit_fsolver_set( s.get(), f, x.get() );
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->f );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_iterate().
       * @param s The fsolver.
       * @return Error code on failure.
       */
      inline static int iterate( fsolver& s ){
	int const result = gsl_multifit_fsolver_iterate( s.get() );
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->f );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_driver().
       * @param s The fsolver
       * @param maxiter Maximum iterations
       * @param epsabs Absolute error bounds
       * @param epsrel Relative error bounds
       * @return Error code on failure.
       */
      inline static int driver( fsolver& s, size_t const maxiter,
				double const epsabs, double const epsrel ){
	int const result = gsl_multifit_fsolver_driver( s.get(), maxiter, epsabs, epsrel ); 
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_name().
       * @param s The fsolver.
       * @return The name of the solver.
       */
      inline static std::string name( fsolver const& s ){
	return std::string( gsl_multifit_fsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_multifit_fsolver_position().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fsolver.
       * @return The current estimate of the position.
       */
      inline static vector position( fsolver const& s ){
	vector result( gsl_multifit_fsolver_position( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }

      // Member functions

      /**
       * C++ version of gsl_multifit_fsolver_set().
       * @param f The function.
       * @param x The initial guess of the fit.
       * @return Error code on failure.
       */
      int set( function* f, vector const& x ){
	int const result = gsl_multifit_fsolver_set( get(), f, x.get() );
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->f );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){
	int const& result = gsl_multifit_fsolver_iterate( get() );
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->f );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_driver().
       * @param maxiter Maximum iterations
       * @param epsabs Absolute error bounds
       * @param epsrel Relative error bounds
       * @return Error code on failure.
       */
      int driver( const size_t maxiter,
		  const double epsabs, const double epsrel ){
	int const& result = gsl_multifit_fsolver_driver( get(), maxiter, epsabs, epsrel );
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->f );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fsolver_name().
       * @return The name of the solver.
       */
      char const* name(){ return gsl_multifit_fsolver_name( get() ); }

      /**
       * C++ version of gsl_multifit_fsolver_position().
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& position(){
	p_v.wrap_gsl_vector_without_ownership( gsl_multifit_fsolver_position( get() ) );
	return p_v;
      }
      
      /**
       * The current position.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_x(){ return x_v; }
      /**
       * The function value at the current position.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_f(){ return f_v; }
      /**
       * The last step as a vector.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_dx(){ return dx_v; }
    private:
      /**
       * Vector for use by position.
       */
      vector p_v;
      /**
       * Vector for use by x.
       */
      vector x_v;
      /**
       * Vector for use by f.
       */
      vector f_v;
      /**
       * Vector for use by dx.
       */
      vector dx_v;
    public:
      // Solver types
      // None yet implemented in GSL
    };

    /**
     * Workspace for solving multidimensional fit problems. Also functions as a namespace
     * for fdfsolver functions. This also allows for functions to be called on fdfsolver
     * as an object.
     */
    class fdfsolver {
    public:
      /**
       * Typedef.
       */
      typedef gsl_multifit_fdfsolver_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      fdfsolver(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new fdfsolver with n elements.
       * @param T The fdfsolver type.
       * @param n The number of elements in the fdfsolver.
       * @param p The number of predictor variables
       */
      explicit fdfsolver( type const* T, size_t const n, size_t const p ){
	ccgsl_pointer = gsl_multifit_fdfsolver_alloc( T, n, p );
	// just plausibly we could allocate fdfsolver but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_multifit_fdfsolver_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_multifit_fdfsolver.
       * This is not usually a good idea. In this case
       * we should not use gsl_multifit_fdfsolver_free() to deallocate the memory.
       * @param v The fdfsolver
       */
      explicit fdfsolver( gsl_multifit_fdfsolver* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The fdfsolver to copy.
       */
      fdfsolver( fdfsolver const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The fdfsolver to copy
       */
      fdfsolver& operator=( fdfsolver const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_multifit_fdfsolver_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~fdfsolver(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_multifit_fdfsolver_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The fdfsolver to move.
       */
      fdfsolver( fdfsolver&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The fdfsolver to move.
       * @return A reference to this.
       */
      fdfsolver& operator=( fdfsolver&& v ){
	fdfsolver( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two fdfsolver are identically equal if their elements are identical.
       * @param v The fdfsolver to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( fdfsolver const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two fdfsolver are different if their elements are not identical.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( fdfsolver const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( fdfsolver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( fdfsolver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( fdfsolver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a fdfsolver is nonnegative.
       * @param v The fdfsolver to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( fdfsolver const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the fdfsolver is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two fdfsolver objects. This works even if the fdfsolver have different sizes
       * because it swaps pointers.
       * @param v The fdfsolver to swap with @c this.
       */
      void swap( fdfsolver& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_multifit_fdfsolver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_multifit_fdfsolver.
       * @return the gsl_multifit_fdfsolver
       */
      gsl_multifit_fdfsolver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_multifit_fdfsolver.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_multifit_fdfsolver.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many fdfsolver objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_multifit_fdfsolver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_multifit_fdfsolver_set().
       * @param s The fdfsolver.
       * @param f The function.
       * @param x The initial guess of the fit.
       * @return Error code on failure.
       */
      inline static int set( fdfsolver& s, function_fdf* f, vector const& x ){
	int const result = gsl_multifit_fdfsolver_set( s.get(), f, x.get() );
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	s.J_m.wrap_gsl_matrix_without_ownership( s.get()->J );
	return result;
      }
      
      /**
       * C++ version of gsl_multifit_fdfsolver_iterate().
       * @param s The fdfsolver.
       * @return Error code on failure.
       */
      inline static int iterate( fdfsolver& s ){
	int const result = gsl_multifit_fdfsolver_iterate( s.get() ); 
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	s.J_m.wrap_gsl_matrix_without_ownership( s.get()->J );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fdfsolver_driver().
       * @param s The fdfsolver
       * @param maxiter Maximum iterations
       * @param epsabs Absolute error bounds
       * @param epsrel Relative error bounds
       * @return Error code on failure.
       */
      inline static int driver( fdfsolver& s, size_t const maxiter,
				double const epsabs, double const epsrel ){
	int const result = gsl_multifit_fdfsolver_driver( s.get(), maxiter, epsabs, epsrel ); 
	s.x_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.f_v.wrap_gsl_vector_without_ownership( s.get()->x );
	s.dx_v.wrap_gsl_vector_without_ownership( s.get()->dx );
	s.J_m.wrap_gsl_matrix_without_ownership( s.get()->J );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fdfsolver_name().
       * @param s The fdfsolver.
       * @return The name of the solver.
       */
      inline static std::string name( fdfsolver const& s ){
	return std::string( gsl_multifit_fdfsolver_name( s.get() ) ); }

      /**
       * C++ version of gsl_multifit_fdfsolver_position().
       * Note that it is s and not the return value
       * that owns the data in the return value.
       * @param s The fdfsolver.
       * @return The current estimate of the position.
       */
      inline static vector position( fdfsolver const& s ){
	vector result( gsl_multifit_fdfsolver_position( s.get() ) );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	return std::move( result );
#else
	return result;
#endif
      }
      /**
       * C++ version of gsl_multifit_fdfsolver_dif_df().
       * @param x The current position
       * @param fdf The fdfsolver_fdf object
       * @param f The function values at the current position
       * @param J The apprximate Jacobian [return]
       * @return Error code on failure.
       */
      inline static int dif_df( vector const& x, function_fdf* fdf, vector const& f,
				matrix& J ){
	return gsl_multifit_fdfsolver_dif_df( x.get(), fdf, f.get(), J.get() ); }
      /**
       * C++ version of gsl_multifit_fdfsolver_dif_fdf().
       * @param x The current position
       * @param fdf The fdfsolver_fdf object
       * @param f The function values at the current position [return]
       * @param J The apprximate Jacobian [return]
       * @return Error code on failure.
       */
      inline static int dif_fdf( vector const& x, function_fdf *fdf, vector& f, matrix& J ){
      	return gsl_multifit_fdfsolver_dif_fdf( x.get(), fdf, f.get(), J.get() ); }

      // Member functions

      /**
       * C++ version of gsl_multifit_fdfsolver_set().
       * @param f The function.
       * @param x The initial guess of the fit.
       * @return Error code on failure.
       */
      int set( function_fdf* f, vector const& x ){
	int const result = gsl_multifit_fdfsolver_set( get(), f, x.get() );
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->f );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	J_m.wrap_gsl_matrix_without_ownership( get()->J );
	return result;
      }
      
      /**
       * C++ version of gsl_multifit_fdfsolver_iterate().
       * @return Error code on failure.
       */
      int iterate(){
	int const result = gsl_multifit_fdfsolver_iterate( get() );
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->f );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	J_m.wrap_gsl_matrix_without_ownership( get()->J );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fdfsolver_driver().
       * @param maxiter Maximum iterations
       * @param epsabs Absolute error bounds
       * @param epsrel Relative error bounds
       * @return Error code on failure.
       */
      int driver( size_t const maxiter,
		  double const epsabs, double const epsrel ){
	int const result = gsl_multifit_fdfsolver_driver( get(), maxiter, epsabs, epsrel ); 
	x_v.wrap_gsl_vector_without_ownership( get()->x );
	f_v.wrap_gsl_vector_without_ownership( get()->x );
	dx_v.wrap_gsl_vector_without_ownership( get()->dx );
	J_m.wrap_gsl_matrix_without_ownership( get()->J );
	return result;
      }

      /**
       * C++ version of gsl_multifit_fdfsolver_name().
       * @return The name of the solver.
       */
      char const* name(){ return gsl_multifit_fdfsolver_name( get() ); }

      /**
       * C++ version of gsl_multifit_fdfsolver_position().
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& position(){
	p_v.wrap_gsl_vector_without_ownership( gsl_multifit_fdfsolver_position( get() ) );
	return p_v;
      }
      
      /**
       * The current position.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_x(){ return x_v; }
      /**
       * The function value at the current position.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_f(){ return f_v; }
      /**
       * The last step as a vector.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      vector const& get_dx(){ return dx_v; }
      /**
       * The Jacobian matrix at the current position.
       * Note that it is @c this fsolver and not the return value
       * that owns the data in the return value.
       * @return The current estimate of the position.
       */
      matrix const& get_J(){ return J_m; }
    private:
      /**
       * Vector for use by position.
       */
      vector p_v;
      /**
       * Vector for use by x.
       */
      vector x_v;
      /**
       * Vector for use by f.
       */
      vector f_v;
      /**
       * Vector for use by dx.
       */
      vector dx_v;
      /**
       * Matrix for use by J.
       */
      matrix J_m; 
    public:
      // Solver types
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* lmder(){ return gsl_multifit_fdfsolver_lmder; }
      /**
       * Static type.
       * @return the named fdfsolver type
       */
      inline static type const* lmsder(){ return gsl_multifit_fdfsolver_lmsder; }
    };

    namespace test {
      /**
       * C++ version of gsl_multifit_test_delta().
       * @param dx Last step
       * @param x Current estimate of x
       * @param epsabs Absolute error bounds
       * @param epsrel Relative error bounds
       * @return GSL_SUCCESS if test condition reached
       */
      inline int delta( vector const& dx, vector const& x, double epsabs, double epsrel ){
	return gsl_multifit_test_delta( dx.get(), x.get(), epsabs, epsrel ); }

      /**
       * C++ version of gsl_multifit_test_gradient().
       * @param g Residual gradient
       * @param epsabs Absolute error bounds
       * @return GSL_SUCCESS if test condition reached
       */
      inline int gradient( vector const& g, double epsabs ){
	return gsl_multifit_test_gradient( g.get(), epsabs ); }
    }


    
  }
}
#endif
