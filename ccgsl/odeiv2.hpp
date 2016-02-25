/*
 * $Id: odeiv2.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_ODEIV2_HPP
#define CCGSL_ODEIV2_HPP

#include<new>
#include<string>
#include"odeiv2_system.hpp"

namespace gsl {
  /**
   * Functions for numerical approximation to solutions of ordinary differential equation
   * initial value problems.
   */
  namespace odeiv2 {
    class driver;
    /**
     * Stepping functions. These advance a solution from time \f$t\f$ to time \f$t+h\f$ for
     * a fixed step size \f$h\f$. They also estimate the local error.
     */
    class step {
    public:
      /**
       * Convenience typedef
       */
      typedef gsl_odeiv2_step_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      step(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The standard constructor creates a new step of type T and dimensions dim
       * @param T The type
       * @param dim The number of dimensions
       */
      step( step::type const* T, size_t dim ){
	ccgsl_pointer = gsl_odeiv2_step_alloc( T, dim );
	// just plausibly we could allocate step but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_odeiv2_step_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_odeiv2_step.
       * This is not usually a good idea. In this case
       * we should not use gsl_odeiv2_step_free() to deallocate the memory.
       * @param v The step
       */
      explicit step( gsl_odeiv2_step* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The step to copy.
       */
      step( step const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The step to copy
       */
      step& operator=( step const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_step_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~step(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_step_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The step to move.
	 */
	step( step&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The step to move.
	 * @return A reference to this.
	 */
	step& operator=( step&& v ){
	  step( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two step are identically equal if their elements are identical.
       * @param v The step to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( step const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two step are different if their elements are not identical.
       * @param v The step to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( step const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a step is nonnegative.
       * @param v The step to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( step const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a step is nonnegative.
       * @param v The step to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( step const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a step is nonnegative.
       * @param v The step to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( step const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a step is nonnegative.
       * @param v The step to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( step const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the step is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two step objects. This works even if the step have different sizes
       * because it swaps pointers.
       * @param v The step to swap with @c this.
       */
      void swap( step& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_odeiv2_step* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_odeiv2_step.
       * @return the gsl_odeiv2_step
       */
      gsl_odeiv2_step* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_odeiv2_step.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_odeiv2_step.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many step objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_odeiv2_step.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      // Step-specific functions
      /**
       * C++ version of gsl_odeiv2_step_reset().
       * @return Error code on failure
       */
      int reset(){ return gsl_odeiv2_step_reset( get() ); }
      /**
       * C++ version of gsl_odeiv2_step_name().
       * @return The name of the step type
       */
      char const* name() const { return gsl_odeiv2_step_name( get() ); }
      /**
       * C++ version of gsl_odeiv2_step_order().
       * @return The order of the stepping function
       */
      unsigned int order(){ return gsl_odeiv2_step_order( get() ); }
      /**
       * C++ version of gsl_odeiv2_step_apply().
       * @param t Current time
       * @param h Step size
       * @param y[] Current state
       * @param yerr[] Absolute error value (return value)
       * @param dydt_in[] Array of derivatives at time t (may be null)
       * @param dydt_out[] Array of derivatives at time t+h (may be null)
       * @param sys The odeiv::system
       * @return Error code on failure
       */
      int apply( double t, double h, double y[], double yerr[], double const dydt_in[],
		 double dydt_out[], system const* sys ){
	return gsl_odeiv2_step_apply( get(), t, h, y, yerr, dydt_in, dydt_out, sys ); }
      /**
       * C++ version of gsl_odeiv2_step_apply().
       * This version works with gsl::vector (or std::vector<double>) objects.
       * @param t Current time
       * @param h Step size
       * @param y Current state
       * @param yerr Absolute error value (return value)
       * @param dydt_in[] Array of derivatives at time t (usually null)
       * @param dydt_out[] Array of derivatives at time t+h (usually null)
       * @param sys The odeiv::system
       * @return Error code on failure
       */
      template<typename Y,typename YERR>
      int apply( double t, double h, Y& y, YERR& yerr, double const dydt_in[],
		 double dydt_out[], system const* sys ){
	return gsl_odeiv2_step_apply( get(), t, h, y.data(), yerr.data(),
				      dydt_in, dydt_out, sys ); }
      /**
       * C++ version of gsl_odeiv2_step_apply().
       * This version works with gsl::vector (or std::vector<double>) objects.
       * @param t Current time
       * @param h Step size
       * @param y Current state
       * @param yerr Absolute error value (return value)
       * @param dydt_in Array of derivatives at time t (not null)
       * @param dydt_out[] Array of derivatives at time t+h (usually null)
       * @param sys The odeiv::system
       * @return Error code on failure
       */
      template<typename Y,typename YERR,typename DYDT_IN>
      int apply( double t, double h, Y& y, YERR& yerr, DYDT_IN const& dydt_in,
		 double dydt_out[], system const* sys ){
	return gsl_odeiv2_step_apply( get(), t, h, y.data(), yerr.data(),
				      dydt_in.data(), dydt_out, sys ); }
      /**
       * C++ version of gsl_odeiv2_step_apply().
       * This version works with gsl::vector (or std::vector<double>) objects.
       * @param t Current time
       * @param h Step size
       * @param y Current state
       * @param yerr Absolute error value (return value)
       * @param dydt_in[] Array of derivatives at time t (usually null)
       * @param dydt_out Array of derivatives at time t+h (not null)
       * @param sys The odeiv::system
       * @return Error code on failure
       */
      template<typename Y,typename YERR,typename DYDT_OUT>
      int apply( double t, double h, Y& y, YERR& yerr, double const dydt_in[],
		 DYDT_OUT& dydt_out, system const* sys ){
	return gsl_odeiv2_step_apply( get(), t, h, y.data, yerr.data,
				      dydt_in, dydt_out.data(), sys ); }
      /**
       * C++ version of gsl_odeiv2_step_apply().
       * This version works with gsl::vector (or std::vector<double>) objects.
       * @param t Current time
       * @param h Step size
       * @param y Current state
       * @param yerr Absolute error value (return value)
       * @param dydt_in Array of derivatives at time t (not null)
       * @param dydt_out Array of derivatives at time t+h (not null)
       * @param sys The odeiv::system
       * @return Error code on failure
       */
      template<typename Y,typename YERR,typename DYDT_IN,typename DYDT_OUT>
      int apply( double t, double h, Y& y, YERR& yerr, DYDT_IN const& dydt_in,
		 DYDT_OUT dydt_out, system const* sys ){
	return gsl_odeiv2_step_apply( get(), t, h, y.data(), yerr.data(),
				      dydt_in.data(), dydt_out.data(), sys ); }
      /**
       * C++ version of gsl_odeiv2_step_set_driver().
       * @param d The driver
       * @return Error code on failure
       */
      int set_driver( driver const& d );//{ return gsl_odeiv2_step_set_driver( get(), d.get() ); }

      // Step types
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk2(){ return gsl_odeiv2_step_rk2; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk4(){ return gsl_odeiv2_step_rk4; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rkf45(){ return gsl_odeiv2_step_rkf45; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rkck(){ return gsl_odeiv2_step_rkck; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk8pd(){ return gsl_odeiv2_step_rk8pd; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk2imp(){ return gsl_odeiv2_step_rk2imp; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk4imp(){ return gsl_odeiv2_step_rk4imp; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* bsimp(){ return gsl_odeiv2_step_bsimp; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* rk1imp(){ return gsl_odeiv2_step_rk1imp; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* msadams(){ return gsl_odeiv2_step_msadams; }
      /**
       * Static type.
       * @return the named step type
       */
      inline static type const* msbdf(){ return gsl_odeiv2_step_msbdf; }
    };

    /**
     * Adaptive step size control
     */
    class control {
    public:
      /**
       * Convenience typedef.
       */
      typedef gsl_odeiv2_control_type type;
      /**
       * The default constructor is only really useful for assigning to.
       */
      control(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The constructor creates a new control of type T. The four static constructors are
       * more useful if you have not defined a type.
       * @param T The type
       */
      explicit control( type const* T ){
	ccgsl_pointer = gsl_odeiv2_control_alloc( T );
	// just plausibly we could allocate control but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_odeiv2_control_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_odeiv2_control.
       * This is not usually a good idea. In this case
       * we should not use gsl_odeiv2_control_free() to deallocate the memory.
       * @param v The control
       */
      explicit control( gsl_odeiv2_control* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The control to copy.
       */
      control( control const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The control to copy
       */
      control& operator=( control const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_control_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~control(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_control_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The control to move.
	 */
	control( control&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The control to move.
	 * @return A reference to this.
	 */
	control& operator=( control&& v ){
	  control( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two control are identically equal if their elements are identical.
       * @param v The control to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( control const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two control are different if their elements are not identical.
       * @param v The control to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( control const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a control is nonnegative.
       * @param v The control to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( control const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a control is nonnegative.
       * @param v The control to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( control const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a control is nonnegative.
       * @param v The control to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( control const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a control is nonnegative.
       * @param v The control to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( control const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the control is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two control objects. This works even if the control have different sizes
       * because it swaps pointers.
       * @param v The control to swap with @c this.
       */
      void swap( control& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_odeiv2_control* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_odeiv2_control.
       * @return the gsl_odeiv2_control
       */
      gsl_odeiv2_control* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_odeiv2_control.
       * @return @c true or @c falses according as 
       * this is the only control object sharing the gsl_odeiv2_control.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many control objects share this pointer.
       * @return the number of control objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_odeiv2_control.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      // Extra functions
      /**
       * C++ version of gsl_odeiv2_control_init().
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @return Error code on failure
       */
      int init( double eps_abs, double eps_rel, double a_y, double a_dydt ){
	return gsl_odeiv2_control_init( get(), eps_abs, eps_rel, a_y, a_dydt ); }

      /**
       * C++ version of gsl_odeiv2_control_hadjust().
       * @param s The step
       * @param y[] Current value of y
       * @param yerr[] Current value of yerr
       * @param dydt[] Current value of dydt
       * @param h Step size (return value)
       * @return GSL_ODEIV_HADJ_DEC, GSL_ODEIV_HADJ_INC or GSL_ODEIV_HADJ_NIL according as
       * h is decreased, may be increased or is unchanged
       */
      int hadjust( step& s, double const y[], double const yerr[],
		   double const dydt[], double* h ){
	return gsl_odeiv2_control_hadjust( get(), s.get(), y, yerr, dydt, h ); }
      /**
       * C++ version of gsl_odeiv2_control_hadjust().
       * This version allows arrays as gsl::vector objects (or std::vector<double>).
       * @param s The step
       * @param y Current value of y
       * @param yerr Current value of yerr
       * @param dydt Current value of dydt
       * @param h Step size (return value)
       * @return GSL_ODEIV_HADJ_DEC, GSL_ODEIV_HADJ_INC or GSL_ODEIV_HADJ_NIL according as
       * h is decreased, may be increased or is unchanged
       */
      template<typename Y,typename YERR,typename DYDT>
      int hadjust( step& s, Y const& y, YERR const& yerr, DYDT const& dydt, double* h ){
	return gsl_odeiv2_control_hadjust( get(), s.get(), y.data(), yerr.data(), dydt.data(), h ); }

      /**
       * C++ version of gsl_odeiv2_control_name().
       * @return The name of the control type.
       */
      char const* name() const { return gsl_odeiv2_control_name( get() ); }

      /**
       * C++ version of gsl_odeiv2_control_errlevel().
       * @param y The current value of y
       * @param dydt The current value of dydt
       * @param h The current step size
       * @param ind The index
       * @param errlev The desired error level (return value)
       * @return Error code on failure
       */
      int errlevel( double const y, double const dydt, double const h,
		    size_t const ind, double* errlev ){
	return gsl_odeiv2_control_errlevel( get(), y, dydt, h, ind, errlev ); }

      /**
       * C++ version of gsl_odeiv2_control_set_driver().
       * @param d The driver
       * @return Error code on failure
       */
      int set_driver( driver const& d );
      //{ return gsl_odeiv2_control_set_driver( get(), d.get() ); } 

      /**
       * C++ version of gsl_odeiv2_control_standard_new().
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @return The control object (shared pointer)
       */
      inline static control standard_new( double eps_abs, double eps_rel,
						  double a_y, double a_dydt ){
	return control( gsl_odeiv2_control_standard_new( eps_abs, eps_rel, a_y, a_dydt ) ); }

      /**
       * C++ version of gsl_odeiv2_control_y_new().
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @return The control object (shared pointer)
       */
      inline static control y_new( double eps_abs, double eps_rel ){
	return control( gsl_odeiv2_control_y_new( eps_abs, eps_rel ) ); }

      /**
       * C++ version of gsl_odeiv2_control_yp_new().
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @return The control object (shared pointer)
       */
      inline static control yp_new( double eps_abs, double eps_rel ){
	return control( gsl_odeiv2_control_yp_new( eps_abs, eps_rel ) ); }

      /**
       * C++ version of gsl_odeiv2_control_scaled_new().
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @param scale_abs[] An array of scales for absolute error
       * @param dim dimension
       * @return The control object (shared pointer)
       */
      inline static control scaled_new( double eps_abs, double eps_rel, double a_y,
					double a_dydt, double const scale_abs[], size_t dim ){
	return control( gsl_odeiv2_control_scaled_new( eps_abs, eps_rel, a_y, a_dydt,
						       scale_abs, dim ) ); }
      /**
       * C++ version of gsl_odeiv2_control_scaled_new().
       * This version allows scale_abs to be gsl::vector or anything for which
       * scale_abs.data() returns an array of doubles.
       * @param eps_abs Absolute error
       * @param eps_rel Relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @param scale_abs An array of scales for absolute error
       * @param dim dimension
       * @return The control object (shared pointer)
       */
      template<typename SCALE_ABS>
      inline static control scaled_new( double eps_abs, double eps_rel, double a_y,
					double a_dydt, SCALE_ABS const& scale_abs, size_t dim ){
	return control( gsl_odeiv2_control_scaled_new( eps_abs, eps_rel, a_y, a_dydt,
						       scale_abs.data(), dim ) ); }
    };

    /**
     * ODE inintial value evolve.
     */
    class evolve {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      evolve(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new evolve with n elements.
       * @param dim The dimension
       */
      explicit evolve( size_t const dim ){
	ccgsl_pointer = gsl_odeiv2_evolve_alloc( dim );
	// just plausibly we could allocate evolve but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_odeiv2_evolve_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_odeiv2_evolve.
       * This is not usually a good idea. In this case
       * we should not use gsl_odeiv2_evolve_free() to deallocate the memory.
       * @param v The evolve
       */
      explicit evolve( gsl_odeiv2_evolve* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The evolve to copy.
       */
      evolve( evolve const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The evolve to copy
       */
      evolve& operator=( evolve const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_evolve_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~evolve(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_evolve_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The evolve to move.
	 */
	evolve( evolve&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The evolve to move.
	 * @return A reference to this.
	 */
	evolve& operator=( evolve&& v ){
	  evolve( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two evolve are identically equal if their elements are identical.
       * @param v The evolve to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( evolve const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two evolve are different if their elements are not identical.
       * @param v The evolve to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( evolve const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a evolve is nonnegative.
       * @param v The evolve to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( evolve const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a evolve is nonnegative.
       * @param v The evolve to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( evolve const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a evolve is nonnegative.
       * @param v The evolve to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( evolve const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a evolve is nonnegative.
       * @param v The evolve to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( evolve const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the evolve is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two evolve objects. This works even if the evolve have different sizes
       * because it swaps pointers.
       * @param v The evolve to swap with @c this.
       */
      void swap( evolve& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_odeiv2_evolve* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_odeiv2_evolve.
       * @return the gsl_odeiv2_evolve
       */
      gsl_odeiv2_evolve* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_odeiv2_evolve.
       * @return @c true or @c falses according as 
       * this is the only evolve object sharing the gsl_odeiv2_evolve.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many evolve objects share this pointer.
       * @return the number of evolve objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_odeiv2_evolve.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
      
      // extra functions

      /**
       * C++ version of gsl_odeiv2_evolve_apply().
       * @param con The control object
       * @param step The step
       * @param dydt The system
       * @param t The initial time
       * @param t1 The maximum time
       * @param h The initial step size (also returns next suggested step size)
       * @param y[] The initial position
       * @return Error code on failure
       */
      int apply( control& con, step& step, system const* dydt,
		 double* t, double t1, double* h, double y[] ){
	return gsl_odeiv2_evolve_apply( get(), con.get(), step.get(), dydt,
					t, t1, h, y ); }
      /**
       * C++ version of gsl_odeiv2_evolve_apply().
       * This version allows y to be a gsl::vector (or any object for which
       * data() returns an array of doubles).
       * @param con The control object
       * @param step The step
       * @param dydt The system
       * @param t The initial time
       * @param t1 The maximum time
       * @param h The initial step size (also returns next suggested step size)
       * @param y The initial position 
       * @return Error code on failure
       */
      template<typename Y>
      int apply( control& con, step& step, system const* dydt,
		 double* t, double t1, double* h, Y& y ){
	return gsl_odeiv2_evolve_apply( get(), con.get(), step.get(), dydt,
					t, t1, h, y.data() ); }

      /**
       * C++ version of gsl_odeiv2_evolve_apply_fixed_step().
       * @param con The control object
       * @param step The step
       * @param dydt The system
       * @param t The initial time
       * @param h0 The initial step size
       * @param y[] The initial position
       * @return Error code on failure
       */
      int apply_fixed_step( control& con, step& step, system const* dydt,
			    double* t, double const h0, double y[] ){
	return gsl_odeiv2_evolve_apply_fixed_step( get(), con.get(), step.get(), dydt,
						   t, h0, y ); }
      /**
       * C++ version of gsl_odeiv2_evolve_apply_fixed_step().
       * This version allows y to be a gsl::vector (or any object for which
       * data() returns an array of doubles).
       * @param con The control object
       * @param step The step
       * @param dydt The system
       * @param t The initial time
       * @param h0 The initial step size
       * @param y The initial position
       * @return Error code on failure
       */
      template<typename Y>
      int apply_fixed_step( control& con, step& step, system const* dydt,
			    double* t, double const h0, Y& y ){
	return gsl_odeiv2_evolve_apply_fixed_step( get(), con.get(), step.get(), dydt,
						   t, h0, y.data() ); }

      /**
       * C++ version of gsl_odeiv2_evolve_reset().
       * @return Error code on failure
       */
      int reset(){ return gsl_odeiv2_evolve_reset( get() ); }

      /**
       * C++ version of gsl_odeiv2_evolve_set_driver().
       * @param d The driver
       * @return Error code on failure
       */
      int set_driver( driver const& d );//{ return gsl_odeiv2_evolve_set_driver( get(), d.get() ); }
    };
    
    /**
     * ODE driver.
     */
    class driver {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      driver(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * Could construct from a gsl_odeiv2_driver.
       * This is not usually a good idea. In this case
       * we should not use gsl_odeiv2_driver_free() to deallocate the memory.
       * @param v The driver
       */
      explicit driver( gsl_odeiv2_driver* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The driver to copy.
       */
      driver( driver const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The driver to copy
       */
      driver& operator=( driver const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_driver_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~driver(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_odeiv2_driver_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The driver to move.
	 */
	driver( driver&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The driver to move.
	 * @return A reference to this.
	 */
	driver& operator=( driver&& v ){
	  driver( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two driver are identically equal if their elements are identical.
       * @param v The driver to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( driver const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two driver are different if their elements are not identical.
       * @param v The driver to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( driver const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a driver is nonnegative.
       * @param v The driver to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( driver const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a driver is nonnegative.
       * @param v The driver to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( driver const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a driver is nonnegative.
       * @param v The driver to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( driver const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a driver is nonnegative.
       * @param v The driver to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( driver const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the driver is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two driver objects. This works even if the driver have different sizes
       * because it swaps pointers.
       * @param v The driver to swap with @c this.
       */
      void swap( driver& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_odeiv2_driver* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_odeiv2_driver.
       * @return the gsl_odeiv2_driver
       */
      gsl_odeiv2_driver* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_odeiv2_driver.
       * @return @c true or @c falses according as 
       * this is the only driver object sharing the gsl_odeiv2_driver.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many driver objects share this pointer.
       * @return the number of driver objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_odeiv2_driver.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      // Extra functions

      /**
       * C++ version of gsl_odeiv2_driver_alloc_y_new().
       * @param sys The system
       * @param T The step type
       * @param hstart The initial value of h
       * @param epsabs The absolute error
       * @param epsrel The relative error
       * @return A driver
       */
      inline static driver y_new( system const* sys, step::type const* T,
				  double const hstart, double const epsabs, double const epsrel ){
	return driver( gsl_odeiv2_driver_alloc_y_new( sys, T, hstart, epsabs, epsrel ) ); }

      /**
       * C++ version of gsl_odeiv2_driver_alloc_yp_new().
       * @param sys The system
       * @param T The step t
       * @param hstart The initial value of h
       * @param epsabs The absolute error
       * @param epsrel The relative error
       * @return A driver
       */
      inline static driver
      yp_new( system const* sys, step::type const* T,
	      double const hstart, double const epsabs, double const epsrel ){
	  return driver( gsl_odeiv2_driver_alloc_yp_new( sys, T, hstart, epsabs, epsrel ) ); }

      /**
       * C++ version of gsl_odeiv2_driver_alloc_scaled_new().
       * @param sys The system
       * @param T The step t
       * @param hstart The initial value of h
       * @param epsabs The absolute error
       * @param epsrel The relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @param scale_abs[] An array of scales for absolute error
       * @return A driver
       */
      inline static driver
      scaled_new( system const* sys, step::type const* T, double const hstart, double const epsabs,
		  double const epsrel, double const a_y, double const a_dydt,
		  double const scale_abs[] ){
	return driver( gsl_odeiv2_driver_alloc_scaled_new( sys, T, hstart, epsabs, epsrel,
							   a_y, a_dydt, scale_abs ) ); }
      /**
       * C++ version of gsl_odeiv2_driver_alloc_scaled_new().
       * This version allows scale_abs to be gsl::vector (or object for which data() returns
       * an array of dpoubles)
       * @param sys The system
       * @param T The step t
       * @param hstart The initial value of h
       * @param epsabs The absolute error
       * @param epsrel The relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @param scale_abs An array of scales for absolute error
       * @return A driver
       */
      template<typename SCALE_ABS>
      inline static driver
      scaled_new( system const* sys, step::type const* T, double const hstart, double const epsabs,
		  double const epsrel, double const a_y, double const a_dydt,
		  SCALE_ABS const& scale_abs ){
	return driver( gsl_odeiv2_driver_alloc_scaled_new( sys, T, hstart, epsabs, epsrel,
							   a_y, a_dydt, scale_abs.data() ) ); }

      /**
       * C++ version of gsl_odeiv2_driver_alloc_standard_new().
       * @param sys The system
       * @param T The step t
       * @param hstart The initial value of h
       * @param epsabs The absolute error
       * @param epsrel The relative error
       * @param a_y Scaling factor for y
       * @param a_dydt Scaling factor for dydt
       * @return A driver
       */
      inline static driver
      standard_new( system const* sys, step::type const* T, double const hstart, double const epsabs,
		    double const epsrel, double const a_y, double const a_dydt ){
	return driver( gsl_odeiv2_driver_alloc_standard_new( sys, T, hstart, epsabs,
							     epsrel, a_y, a_dydt ) ); }

      /**
       * C++ version of gsl_odeiv2_driver_set_hmin().
       * @param hmin Minimum allowed step size
       * @return Error code on failure
       */
      int set_hmin( double const hmin ){ return gsl_odeiv2_driver_set_hmin( get(), hmin ); }

      /**
       * C++ version of gsl_odeiv2_driver_set_hmax().
       * @param hmax Maximum allowed step size
       * @return Error code on failure
       */
      int set_hmax( double const hmax ){ return gsl_odeiv2_driver_set_hmax( get(), hmax ); }

      /**
       * C++ version of gsl_odeiv2_driver_set_nmax().
       * @param nmax Maximum number of steps
       * @return Error code on failure
       */
      int set_nmax( unsigned long int const nmax ){
	return gsl_odeiv2_driver_set_nmax( get(), nmax ); }

      /**
       * C++ version of gsl_odeiv2_driver_apply().
       * @param t Initial value of time t
       * @param t1 Maximum value of time t
       * @param y[] Initial value of y
       * @return Error code on failure
       */
      int apply( double* t, double const t1, double y[] ){
	return gsl_odeiv2_driver_apply( get(), t, t1, y ); }
      /**
       * C++ version of gsl_odeiv2_driver_apply().
       * This version allows y to be gsl::vector (or object for which data() returns
       * an array of doubles)
       * @param t Initial value of time t
       * @param t1 Maximum value of time t
       * @param y Initial value of y
       * @return Error code on failure
       */
      template<typename Y>
      int apply( double* t, double const t1, Y& y ){
	return gsl_odeiv2_driver_apply( get(), t, t1, y.data() ); }

      /**
       * C++ version of gsl_odeiv2_driver_apply_fixed_step().
       * @param t Initial value of time t
       * @param h Fixed step size
       * @param n Number of steps
       * @param y[] Initial value of y
       * @return Error code on faiure
       */
      int apply_fixed_step( double* t, double const h, unsigned long int const n, double y[] ){
	return gsl_odeiv2_driver_apply_fixed_step( get(), t, h, n, y ); }
      /**
       * C++ version of gsl_odeiv2_driver_apply_fixed_step().
       * This version allows y to be gsl::vector (or object for which data() returns
       * an array of doubles)
       * @param t Initial value of time t
       * @param h Fixed step size
       * @param n Number of steps
       * @param y Initial value of y
       * @return Error code on faiure
       */
      template<typename Y>
      int apply_fixed_step( double* t, double const h, unsigned long int const n, Y& y ){
	return gsl_odeiv2_driver_apply_fixed_step( get(), t, h, n, y.data() ); }

      /**
       * C++ version of gsl_odeiv2_driver_reset().
       * @return Error code on failure
       */
      int reset(){ return gsl_odeiv2_driver_reset( get() ); }

    };
    
    inline int step::set_driver( driver const& d ){
      return gsl_odeiv2_step_set_driver( get(), d.get() ); }
    inline int control::set_driver( driver const& d ){
      return gsl_odeiv2_control_set_driver( get(), d.get() ); } 
    inline int evolve::set_driver( driver const& d ){
      return gsl_odeiv2_evolve_set_driver( get(), d.get() ); } 
  }
}
#endif
