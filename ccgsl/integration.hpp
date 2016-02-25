/*
 * $Id: integration.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_INTEGRATION_HPP
#define CCGSL_INTEGRATION_HPP

#include<new>
#include<algorithm>
#include<gsl/gsl_integration.h>
#include"exception.hpp"

namespace gsl {
  /**
   * Namespace for numerical integration functions.
   */
  namespace integration {
    /**
     * Workspace for integration.
     */
    class workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      workspace(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new workspace with n elements.
       * @param n The number of double-precision intervals in the workspace
       */
      explicit workspace( size_t const n ){
	ccgsl_pointer = gsl_integration_workspace_alloc( n );
	// just plausibly we could allocate workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_integration_workspace_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_integration_workspace.
       * This is not usually a good idea. In this case
       * you should not use gsl_integration_workspace_free() to deallocate the memory.
       * @param v The workspace
       */
      explicit workspace( gsl_integration_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The integration_workspace to copy.
       */
      workspace( workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The workspace to copy
       */
      workspace& operator=( workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_integration_workspace_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_integration_workspace_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The workspace to move.
	 */
	workspace( workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The workspace to move.
	 * @return A reference to this.
	 */
	workspace& operator=( workspace&& v ){
	  workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two workspace are identically equal if their elements are identical.
       * @param v The workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two workspace are different if their elements are not identical.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two workspace. This works even if the workspace have different sizes
       * because it swaps pointers.
       * @param v The workspace to swap with @c this.
       */
      void swap( workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_integration_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_integration_workspace.
       * @return the gsl_integration_workspace
       */
      gsl_integration_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_integration_workspace.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_integration_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_integration_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * A precomputed table of Chebychev moments. 
     */
    class qaws_table {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      qaws_table(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new qaws_table with singular
       * weight function
       * \f$W(x)=(x − a)^{\alpha}(b − x)^{\beta}\log_{\mu}(x − a)\log_{\nu}(b − x)\f$
       * @param alpha \f$\alpha\f$.
       * @param beta \f$\beta\f$.
       * @param mu \f$\mu\f$.
       * @param nu \f$\nu\f$.
       */
      explicit qaws_table( double const alpha, double const beta, double const mu,
			   double const nu ){
	ccgsl_pointer = gsl_integration_qaws_table_alloc( alpha, beta, mu, nu );
	// just plausibly we could allocate qaws_table but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_integration_qaws_table_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_integration_qaws_table.
       * This is not usually a good idea. In this case
       * you should not use gsl_integration_qaws_table_free() to deallocate the memory.
       * @param v The qaws_table
       */
      explicit qaws_table( gsl_integration_qaws_table* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The qaws_table to copy.
       */
      qaws_table( qaws_table const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The qaws_table to copy
       */
      qaws_table& operator=( qaws_table const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_integration_qaws_table_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~qaws_table(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_integration_qaws_table_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The qaws_table to move.
	 */
	qaws_table( qaws_table&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The qaws_table to move.
	 * @return A reference to this.
	 */
	qaws_table& operator=( qaws_table&& v ){
	  qaws_table( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two qaws_table are identically equal if their elements are identical.
       * @param v The qaws_table to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( qaws_table const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two qaws_table are different if their elements are not identical.
       * @param v The qaws_table to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( qaws_table const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qaws_table is nonnegative.
       * @param v The qaws_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( qaws_table const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qaws_table is nonnegative.
       * @param v The qaws_table to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( qaws_table const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qaws_table is nonnegative.
       * @param v The qaws_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( qaws_table const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qaws_table is nonnegative.
       * @param v The qaws_table to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( qaws_table const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the qaws_table is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two qaws_table. This works even if the qaws_table have different sizes
       * because it swaps pointers.
       * @param v The qaws_table to swap with @c this.
       */
      void swap( qaws_table& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_integration_qaws_table* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_integration_qaws_table.
       * @return the gsl_integration_qaws_table
       */
      gsl_integration_qaws_table* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_integration_qaws_table.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_integration_qaws_table
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_integration_qaws_table
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_integration_qaws_table_set().
     * @param t The qaws_table
     * @param alpha \f$\alpha\f$.
     * @param beta \f$\beta\f$.
     * @param mu \f$\mu\f$.
     * @param nu \f$\nu\f$.
     * @return Erro code on failure.
     */
    inline int qaws_table_set( qaws_table& t, double const alpha, double const beta,
			       int const mu, int const nu ){
      return gsl_integration_qaws_table_set( t.get(), alpha, beta, mu, nu ); }

    /**
     * Enumerated type
     */
    enum qawo_enum { 
      SINE   = GSL_INTEG_SINE,   /**< use sine function in qawo_table */
      COSINE = GSL_INTEG_COSINE  /**< use cosine function in qawo_table */
    };

    /**
     * A precomputed table of Chebychev moments. 
     */
    class qawo_table {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      qawo_table(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new qawo_table with sine or cosine
       * weight function \f$W(x)=\sin(\omega x)\f$ or \f$W(x)=\cos(\omega x)\f$.
       * @param omega \f$\omega\f$.
       * @param L length of the integral over which the function will be evaluated.
       * @param sine gsl::integration::SINE or gsl::integration::COSINE.
       * @param n Number of bisections of interval: subintervals of length \f$L/2^n\f$ are used.
       */
      explicit qawo_table( double const omega, double const L,
			   qawo_enum const sine, size_t const n ){
	ccgsl_pointer
	  = gsl_integration_qawo_table_alloc( omega, L,
					      static_cast<gsl_integration_qawo_enum>( sine ), n );
	// just plausibly we could allocate qawo_table but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_integration_qawo_table_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_integration_qawo_table.
       * This is not usually a good idea. In this case
       * you should not use gsl_integration_qawo_table_free() to deallocate the memory.
       * @param v The qawo_table
       */
      explicit qawo_table( gsl_integration_qawo_table* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The qawo_table to copy.
       */
      qawo_table( qawo_table const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The qawo_table to copy
       */
      qawo_table& operator=( qawo_table const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_integration_qawo_table_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~qawo_table(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_integration_qawo_table_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The qawo_table to move.
	 */
	qawo_table( qawo_table&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The qawo_table to move.
	 * @return A reference to this.
	 */
	qawo_table& operator=( qawo_table&& v ){
	  qawo_table( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two qawo_table are identically equal if their elements are identical.
       * @param v The qawo_table to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( qawo_table const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two qawo_table are different if their elements are not identical.
       * @param v The qawo_table to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( qawo_table const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qawo_table is nonnegative.
       * @param v The qawo_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( qawo_table const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qawo_table is nonnegative.
       * @param v The qawo_table to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( qawo_table const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qawo_table is nonnegative.
       * @param v The qawo_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( qawo_table const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a qawo_table is nonnegative.
       * @param v The qawo_table to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( qawo_table const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the qawo_table is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two qawo_table. This works even if the qawo_table have different sizes
       * because it swaps pointers.
       * @param v The qawo_table to swap with @c this.
       */
      void swap( qawo_table& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_integration_qawo_table* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_integration_qawo_table.
       * @return the gsl_integration_qawo_table
       */
      gsl_integration_qawo_table* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_integration_qawo_table.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_integration_qawo_table
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_integration_qawo_table
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_integration_qawo_table_set().
     * @param t The qawo_table.
     * @param omega \f$\omega\f$.
     * @param L length of the integral over which the function will be evaluated.
     * @param sine gsl::integration::SINE or gsl::integration::COSINE.
     * @return Error code on failure.
     */
    inline int qawo_table_set( qawo_table& t, double const omega, double const L,
			       qawo_enum const sine ){
      return gsl_integration_qawo_table_set( t.get(), omega, L,
					     static_cast<gsl_integration_qawo_enum>( sine ) ); }
    
    /**
     * C++ version of gsl_integration_qawo_table_set_length().
     * @param t The qawo_table.
     * @param L New interval length.
     * @return Error code on failure.
     */
    inline int qawo_table_set_length( qawo_table& t, double L ){
      return gsl_integration_qawo_table_set_length( t.get(), L ); }

    /**
     * C++ version of gsl_integration_qk15().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk15( gsl_function const* f, double a, double b, double* result, double* abserr,
		      double* resabs, double* resasc ){
      gsl_integration_qk15( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qk21().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk21( gsl_function const* f, double a, double b, double* result, double* abserr,
		      double* resabs, double* resasc ){
      gsl_integration_qk21( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qk31().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk31( gsl_function const* f, double a, double b, double* result, double* abserr,
		      double* resabs, double* resasc ){
      gsl_integration_qk31( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qk41().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk41( gsl_function const* f, double a, double b, double* result, double* abserr,
	       double* resabs, double* resasc ){
      gsl_integration_qk41( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qk51().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk51( gsl_function const* f, double a, double b, double* result, double* abserr,
	       double* resabs, double* resasc ){
      gsl_integration_qk51( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qk61().
     * @param f Function to be integrated.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    inline void qk61( gsl_function const* f, double a, double b, double* result, double* abserr,
	       double* resabs, double* resasc ){
      gsl_integration_qk61( f, a, b, result, abserr, resabs, resasc ); }

    /**
     * C++ version of gsl_integration_qcheb().
     * @param f A function.
     * @param a Lower limit.
     * @param b Upper limit.
     * @param cheb12 A vector of dimension 13 containing the Chebyshev coefficients for degree 12.
     * @param cheb24 A vector of dimension 25 containing the Chebyshev coefficients for degree 24.
     */
    inline void qcheb( gsl_function* f, double a, double b, double* cheb12, double* cheb24 ){
      gsl_integration_qcheb( f, a, b, cheb12, cheb24 ); }

    /**
     * C++ version of gsl_integration_qcheb().
     * This version works with std::vector<double> and gsl::vector.
     * @param f A function.
     * @param a Lower limit.
     * @param b Upper limit.
     * @param cheb12 A vector of dimension 13 containing the Chebyshev coefficients for degree 12.
     * @param cheb24 A vector of dimension 25 containing the Chebyshev coefficients for degree 24.
     */
    template<typename T>
    inline void qcheb( gsl_function* f, double a, double b, T& cheb12, T& cheb24 ){
#ifndef GSL_RANGE_CHECK_OFF
      // Check that vectors are long enough
      if( cheb12.size() < 13 ){
  	gsl_error( "expected cheb12 of length 13 (or more)",
		   __FILE__, __LINE__, exception::GSL_EFAILED );
  	return;
      }
      if( cheb24.size() < 25 ){
  	gsl_error( "expected cheb24 of length 25 (or more)",
		   __FILE__, __LINE__, exception::GSL_EFAILED );
  	return;
      }
#endif
      gsl_integration_qcheb( f, a, b, cheb12.data(), cheb24.data() );
    }

    /**
     * C++ version of gsl_integration_qk().
     * @param n An integer
     * @param xgk An array of length n.
     * @param wg An array of length n.
     * @param wgk An array of length n.
     * @param fv1 An array of length n (return value).
     * @param fv2 An array of length n (return value).
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     * @return Error code on failure.
     */
inline void qk( int const n, double const xgk[], double const wg[], double const wgk[],
		    double fv1[], double fv2[], gsl_function const* f, double a, double b,
		    double* result, double* abserr, double* resabs, double* resasc ){
      gsl_integration_qk( n, xgk, wg, wgk, fv1, fv2, f, a, b, result, abserr, resabs, resasc );
    }    

    /**
     * C++ version of gsl_integration_qk().
     * This version works with std::vector<double> and gsl::vector of size n.
     * @param xgk An array of length n.
     * @param wg An array of length n.
     * @param wgk An array of length n.
     * @param fv1 An array of length n (return value).
     * @param fv2 An array of length n (return value).
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param result Approximation to the integral.
     * @param abserr Estimate of the absolute size of the absolute error.
     * @param resabs Approximation to the integral of abs(f).
     * @param resasc Approximation to the integral of abs(f - integral / (b - a) ).
     */
    template<typename T>
    inline void qk( T const& xgk, T const& wg, T const& wgk,
		    T& fv1, T& fv2, gsl_function const* f, double a, double b,
		    double* result, double* abserr, double* resabs, double* resasc ){
#ifndef GSL_RANGE_CHECK_OFF
      size_t const n = xgk.size();
      // Check that vectors are same length
      if( n == 0 ){ gsl_error( "expected xgk of nonzero size",
			       __FILE__, __LINE__, exception::GSL_EFAILED ); return; }
      if( wg.size() != n ){ gsl_error( "size mismatch: xgk and wg",
				       __FILE__, __LINE__, exception::GSL_EFAILED ); return; }
      if( wgk.size() != n ){ gsl_error( "size mismatch: xgk and wgk",
					__FILE__, __LINE__, exception::GSL_EFAILED ); return; }
      if( fv1.size() != n ){ gsl_error( "size mismatch: xgk and fv1",
					__FILE__, __LINE__, exception::GSL_EFAILED ); return; }
      if( fv2.size() != n ){ gsl_error( "size mismatch: xgk and fv2",
					__FILE__, __LINE__, exception::GSL_EFAILED ); return; }
#endif
      gsl_integration_qk( n, xgk.data(), wg.data(), wgk.data(), fv1.data(), fv2.data(),
			  f, a, b, result, abserr, resabs, resasc );
    }

    /**
     * C++ version of gsl_integration_qng().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param epsabs Absolute error limit.
     * @param epsrel Relative error limit.
     * @param result Estimate of integral.
     * @param abserr Estimate of absolute error (return value).
     * @param neval Number of function evaluations used (return value).
     * @return Error code on failure.
     */
    inline int qng( gsl_function const* f, double a, double b, double epsabs, double epsrel,
		    double* result, double* abserr, size_t* neval ){
      return gsl_integration_qng( f, a, b, epsabs, epsrel, result, abserr, neval ); }
    
    /**
     * Keys used for qaq().
     */
    enum qag_key { 
      GAUSS15   = 1,   /**< use 15-point Gauss--Konrod rule */
      GAUSS21   = 2,   /**< use 21-point Gauss--Konrod rule */
      GAUSS31   = 3,   /**< use 31-point Gauss--Konrod rule */
      GAUSS41   = 4,   /**< use 41-point Gauss--Konrod rule */
      GAUSS51   = 5,   /**< use 51-point Gauss--Konrod rule */
      GAUSS61   = 6    /**< use 61-point Gauss--Konrod rule */
    };

    /**
     * C++ version of gsl_integration_qag().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals: limited to and typically identical to
     * workspace.size()
     * @param key A qag_key to determine the rule.
     * @param workspace An integration::workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qag( gsl_function* f, double const a, double const b, double const epsabs,
		    double const epsrel, size_t const limit, qag_key const key, workspace& workspace,
		    double* result, double* abserr ){
#ifndef GSL_RANGE_CHECK_OFF
      if( limit > (workspace.get() == 0 ? 0 : workspace.get()->limit) ){
	gsl_error( "limit must not exceed size of workspace",
		   __FILE__, __LINE__, exception::GSL_EFAILED );
	return GSL_EFAILED; }
#endif
      return gsl_integration_qag( f, a, b, epsabs, epsrel, limit, static_cast<int>( key ),
				  workspace.get(), result, abserr ); }

    /**
     * C++ version of gsl_integration_qagi().
     * @param f A function.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qagi( gsl_function* f, double const epsabs, double const epsrel, size_t const limit,
		     workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qagi( f, epsabs, epsrel, limit, workspace.get(), result, abserr ); }

    /**
     * C++ version of gsl_integration_qagiu().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qagiu( gsl_function* f, double const a, double const epsabs, double const epsrel,
		      size_t const limit, workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qagiu( f, a, epsabs, epsrel, limit, workspace.get(), result, abserr ); }

    /**
     * C++ version of gsl_integration_qagil().
     * @param f A function.
     * @param b Upper limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qagil( gsl_function* f, double const b, double const epsabs, double const epsrel,
		      size_t const limit, workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qagil( f, b, epsabs, epsrel, limit, workspace.get(),
				    result, abserr ); }

    /**
     * C++ version of gsl_integration_qags().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qags( gsl_function* f, double const a, double const b, double const epsabs,
		     double const epsrel, size_t const limit, workspace& workspace, double* result,
		     double* abserr ){
      return gsl_integration_qags( f, a, b, epsabs, epsrel, limit, workspace.get(),
				   result, abserr ); }

    /**
     * C++ version of gsl_integration_qagp().
     * @param f A function.
     * @param pts An array of known singular points: the first and last elements should
     * be the lower and upper integration limits
     * @param npts The number of singular points
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qagp( gsl_function* f, double const* pts, size_t const npts, double const epsabs,
		     double const epsrel,
		     size_t const limit, workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qagp( f, const_cast<double*>( pts ), npts, epsabs, epsrel, limit,
				   workspace.get(), result, abserr ); }

    /**
     * C++ version of gsl_integration_qagp().
     * This version works with std::vector<double> and gsl::vector.
     * @param f A function.
     * @param pts An array of known singular points: the first and last elements should
     * be the lower and upper integration limits
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    template<typename T>
    inline int qagp( gsl_function* f, T const& pts, double const epsabs, double const epsrel,
		     size_t const limit, workspace& workspace, double* result, double* abserr ){
      size_t const npts = pts.size();
#ifndef GSL_RANGE_CHECK_OFF
      // Check that vectors are long enough
      if( npts < 2 ){
  	gsl_error( "expected pts of length 2 or greater",
		   __FILE__, __LINE__, exception::GSL_EFAILED );
  	return GSL_EFAILED;
      }
#endif
      return gsl_integration_qagp( f, const_cast<double*>( pts.data() ), npts, epsabs, epsrel,
				   limit, workspace.get(), result, abserr ); }
					     
    /**
     * C++ version of gsl_integration_qawc().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param c Singularity point.
     * @param epsabs Required absolute error in Cauchy principal value.
     * @param epsrel Required relative error in Cauchy principal value.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the Cauchy principal value of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the Cauchy principal value.
     * @return Error code on failure.
     */
    inline int qawc( gsl_function* f, double const a, double const b, double const c,
		     double const epsabs, double const epsrel, size_t const limit,
		     workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qawc( f, a, b, c, epsabs, epsrel, limit, workspace.get(),
				   result, abserr ); }

    /**
     * C++ version of gsl_integration_qaws().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param t The qaws_table.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qaws( gsl_function* f, double const a, double const b, qaws_table& t,
		     double const epsabs, double const epsrel, size_t const limit,
		     workspace& workspace, double* result, double* abserr ){
      return gsl_integration_qaws( f, a, b, t.get(), epsabs, epsrel, limit,
				   workspace.get(), result, abserr ); }

    /**
     * C++ version of gsl_integration_qawo().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param wf A qawo_table.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qawo( gsl_function* f, double const a, double const epsabs, double const epsrel,
		     size_t const limit, workspace& workspace, qawo_table& wf,
		     double* result, double* abserr ){
      return gsl_integration_qawo( f, a, epsabs, epsrel, limit, workspace.get(), wf.get(),
				   result, abserr ); }

    /**
     * C++ version of gsl_integration_qawf().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param limit The maximum number of subintervals.
     * @param workspace A gsl::integration workspace.
     * @param cycle_workspace A gsl::integration workspace for subintervals.
     * @param wf The qawo_table.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @return Error code on failure.
     */
    inline int qawf( gsl_function* f, double const a, double const epsabs, size_t const limit,
		     integration::workspace& workspace, integration::workspace& cycle_workspace,
		     qawo_table& wf, double* result, double* abserr ){
      return gsl_integration_qawf( f, a, epsabs, limit, workspace.get(), cycle_workspace.get(),
				   wf.get(), result, abserr ); }

    /**
     * Workspace for glfixed_table: used for Gauss--Legendre integration.
     */
    class glfixed_table {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      glfixed_table(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new glfixed_table with n elements.
       * @param n The number of points in the table. The n-point Gauss--Legendre rule is exact for
       * polynomials of order \f$2n-1\f$ or less.
       */
      explicit glfixed_table( size_t const n ){
	ccgsl_pointer = gsl_integration_glfixed_table_alloc( n );
	// just plausibly we could allocate glfixed_table but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_integration_glfixed_table_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_integration_glfixed_table.
       * This is not usually a good idea. In this case
       * you should not use gsl_integration_glfixed_table_free() to deallocate the memory.
       * @param v The glfixed_table
       */
      explicit glfixed_table( gsl_integration_glfixed_table* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The glfixed_table to copy.
       */
      glfixed_table( glfixed_table const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The glfixed_table to copy
       */
      glfixed_table& operator=( glfixed_table const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_integration_glfixed_table_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~glfixed_table(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_integration_glfixed_table_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The glfixed_table to move.
	 */
	glfixed_table( glfixed_table&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The glfixed_table to move.
	 * @return A reference to this.
	 */
	glfixed_table& operator=( glfixed_table&& v ){
	  glfixed_table( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two glfixed_table are identically equal if their elements are identical.
       * @param v The glfixed_table to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( glfixed_table const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two glfixed_table are different if their elements are not identical.
       * @param v The glfixed_table to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( glfixed_table const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a glfixed_table is nonnegative.
       * @param v The glfixed_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( glfixed_table const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a glfixed_table is nonnegative.
       * @param v The glfixed_table to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( glfixed_table const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a glfixed_table is nonnegative.
       * @param v The glfixed_table to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( glfixed_table const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a glfixed_table is nonnegative.
       * @param v The glfixed_table to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( glfixed_table const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the glfixed_table is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two glfixed_table. This works even if the glfixed_table have different sizes
       * because it swaps pointers.
       * @param v The glfixed_table to swap with @c this.
       */
      void swap( glfixed_table& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_integration_glfixed_table* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_integration_glfixed_table.
       * @return the gsl_integration_glfixed_table
       */
      gsl_integration_glfixed_table* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_integration_glfixed_table.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_integration_glfixed_table
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_integration_glfixed_table
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_integration_glfixed().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param t The glfixed_table.
     * @return Error code on failure.
     */
    inline double glfixed( gsl_function const* f, double const a, double const b,
			   glfixed_table const& t ){
      return gsl_integration_glfixed( f, a, b, t.get() ); }

    /**
     * C++ version of gsl_integration_glfixed_point().
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param i Index i.
     * @param xi ith Gauss--Legendre point.
     * @param wi ith Gauss--Legendre weight.
     * @param t The glfixed_table.
     * @return Error code on failure.
     */
    inline int glfixed_point( double const a, double const b, size_t const i, double* xi, double* wi,
			      glfixed_table const& t ){
      return gsl_integration_glfixed_point( a, b, i, xi, wi, t.get() ); } 
    
    /**
     * Workspace for CQUAD quadrature routine.
     */
    class cquad_workspace {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      cquad_workspace(){
	ccgsl_pointer = 0;
	count = 0; // initially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new cquad_workspace with space for n intervals.
       * @param n The number of intervals in the cquad_workspace
       */
      explicit cquad_workspace( size_t const n ){
	ccgsl_pointer = gsl_integration_cquad_workspace_alloc( n );
	// just plausibly we could allocate cquad_workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_integration_cquad_workspace_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_integration_cquad_workspace.
       * This is not usually a good idea. In this case
       * you should not use gsl_integration_cquad_workspace_free() to deallocate the memory.
       * @param v The cquad_workspace
       */
      explicit cquad_workspace( gsl_integration_cquad_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The cquad_workspace to copy.
       */
      cquad_workspace( cquad_workspace const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The cquad_workspace to copy
       */
      cquad_workspace& operator=( cquad_workspace const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_integration_cquad_workspace_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~cquad_workspace(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_integration_cquad_workspace_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	/**
	 * Move constructor.
	 * @param v The cquad_workspace to move.
	 */
	cquad_workspace( cquad_workspace&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	  std::swap( count, v.count );
	  v.ccgsl_pointer = nullptr;
	}
	/**
	 * Move operator.
	 * @param v The cquad_workspace to move.
	 * @return A reference to this.
	 */
	cquad_workspace& operator=( cquad_workspace&& v ){
	  cquad_workspace( std::move( v ) ).swap( *this );
	  return *this;
	}
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two cquad_workspace are identically equal if their elements are identical.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( cquad_workspace const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two cquad_workspace are different if their elements are not identical.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( cquad_workspace const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a cquad_workspace is nonnegative.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( cquad_workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a cquad_workspace is nonnegative.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( cquad_workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a cquad_workspace is nonnegative.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( cquad_workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for eaxmple, for checking, that a cquad_workspace is nonnegative.
       * @param v The cquad_workspace to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( cquad_workspace const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the cquad_workspace is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two cquad_workspace. This works even if the cquad_workspace have different sizes
       * because it swaps pointers.
       * @param v The cquad_workspace to swap with @c this.
       */
      void swap( cquad_workspace& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_integration_cquad_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_integration_cquad_workspace.
       * @return the gsl_integration_cquad_workspace
       */
      gsl_integration_cquad_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_integration_cquad_workspace.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_integration_cquad_workspace
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many cquad_workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_integration_cquad_workspace
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };

    /**
     * C++ version of gsl_integration_cquad().
     * @param f A function.
     * @param a Lower limit of integration.
     * @param b Upper limit of integration.
     * @param epsabs Required absolute error in integral estimate.
     * @param epsrel Required relative error in integral estimate.
     * @param ws The cquad workspace.
     * @param result The estimate of the integral.
     * @param abserr An estimate of the absolute error in the estimate of the integral.
     * @param nevals The number of evaluations required.
     * @return Error code on failure.
     */
    inline int cquad( gsl_function const* f, double const a, double const b, double const epsabs,
		      double const epsrel, cquad_workspace& ws,
		      double* result, double* abserr, size_t* nevals ){
      return gsl_integration_cquad( f, a, b, epsabs, epsrel, ws.get(), result, abserr, nevals ); }
  }
}

#endif

