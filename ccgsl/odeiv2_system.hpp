/*
 * $Id: odeiv2_system.hpp 289 2012-12-04 15:32:17Z jdl3 $
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

#ifndef CCGSL_ODEIV2_SYSTEM_HPP
#define CCGSL_ODEIV2_SYSTEM_HPP

#include"exception.hpp"
#include"vector.hpp"
#include"matrix.hpp"
#include<gsl/gsl_odeiv2.h>

namespace gsl {
  namespace odeiv2 {
    /**
     * Class that extends gsl_odeiv2_system so that it can be constructed
     * from function objects.
     * Since gsl::odeiv2::system is a subclass of gsl_odeiv2_system,
     * a pointer to an object of this class can be used whenever a pointer to a gsl_odeiv2_system
     * object is required. Thus, pointers to objects of this class can be used directly with the GNU
     * Scientific Library.
     *
     * You can construct a gsl::odeiv2::system object from function objects.
     * If the function object is not already implemented, implementing it as a subclass T of
     * gsl::odeiv2::system::concept will make it easy to construct using gsl::odeiv2::system<T>(). If
     * the function object uses a single class T that cannot readily be modified, try something like
     * @code
     * class System : public gsl::odeiv2::system::concept {
     * public:
     *   System( T& t ) : t( t );
     *   int function( double t, gsl::vector const& y, gsl::vector& dydt ){ ... }
     *   int jacobian( double t, gsl::vector const& y, gsl::matrix& dfdy, gsl::vector& dfdt ){ ... }
     * private:
     *   T& t;
     * };
     *
     * ...
     *
     * System system( t );
     * gsl::odeiv2::system( system );
     * @endcode
     * You can use similar constructions if function and jacobian are defined in different classes.
     *
     * This class is designed for flexibility rather than efficient copying.
     * So if objects are to be copied frequently, consider using shared pointers
     * or the like. Moving is defined for C++11.
     */
    class system : public gsl_odeiv2_system {
    public:
      /**
       * This is an empty base class. Its main purpose is to define the concept
       * of a class whose objects can be used to construct a system object. Any class that
       * implements function() and size() with suitable T can be used in the constructor of system.
       * If a class is derived from gsl::odeiv2::system::concept, the compiler is likely to give
       * better warnings if one of the required functions is not implemented correctly.
       */
      struct concept {
	/**
	 * The function that stores dydt given y and t.
	 * T may be double* or gsl::vector. U may be * double* or gsl::vector.
	 * @param t The first (time) paramter
	 * @param y The second (y) paramter
	 * @param dydt The derivatives of y with respect to t
	 * @return GSL_SUCCESS on success; otherwise GSL_EBADFUNC
	 */
	template<typename T>
	int function( double const t, T const& y, T& dydt );
	/**
	 * As function but also store Jacobian in dfdy. It is not necessary to supply
	 * this function for all ode methods. T may be double* or gsl::vector. U may be
	 * double*, gsl::vector or gsl::matrix.
	 * @param t The first (time) paramter
	 * @param dfdy The derivatives of f with respect to y
	 * @param dfdt The derivatives of f with respect to t
	 * @return GSL_SUCCESS on success; otherwise GSL_EBADFUNC
	 */
	template<typename T,typename U>
	int jacobian( double const t, T const&, U& dfdy, T& dfdt ){
	  return exception::GSL_SUCCESS; }
	/**
	 * The dimension of the function arguments
	 * @return The dimension (number of elements) of y, dydt and dfdy
	 */
	virtual size_t size() const = 0;
      };
#ifndef DOXYGEN_SKIP
    private:
      /**
       * Used internally to point to function objects.
       */
      struct base_F {
	/**
	 * Typedef for function pointer.
	 */
	typedef int (*function_t)(double const,double const*,double*,void*);
	/**
	 * Typedef for function pointer.
	 */
	typedef int (*jacobian_t)(double const,double const*,double*,double*,void*);
	/**
	 * Virtual destructor
	 */
	virtual ~base_F(){};
	/**
	 * Check for jacobian
	 */
	virtual bool has_jacobian() const = 0;
      };
      /**
       * Used internally to point to function objects.
       */
      template<typename T>
      struct Function : public base_F {
      public:
	/**
	 * The constructor requires an object that implements concept.
	 * @param A function object that implements concept.
	 */
	Function( T& t ) : t( t ){
	  size_t const size = t.size();
	  vy.size = size;
	  vy.stride = 1;
	  vy.data = 0;
	  vy.block = 0;
	  vy.owner = 0;
	  yv.wrap_gsl_vector_without_ownership( &vy );
	  vdydt.size = size;
	  vdydt.stride = 1;
	  vdydt.data = 0;
	  vdydt.block = 0;
	  vdydt.owner = 0;
	  dydtv.wrap_gsl_vector_without_ownership( &vdydt );
	  vdfdy.size = size * size;
	  vdfdy.stride = 1;
	  vdfdy.data = 0;
	  vdfdy.block = 0;
	  vdfdy.owner = 0;
	  dfdyv.wrap_gsl_vector_without_ownership( &vdfdy );
	  mdfdy.size1 = size;
	  mdfdy.size2 = size;
	  mdfdy.tda = size;
	  mdfdy.data = 0;
	  mdfdy.block = 0;
	  mdfdy.owner = 0;
	  dfdym.wrap_gsl_matrix_without_ownership( &mdfdy );
	  vdfdt.size = size;
	  vdfdt.stride = 1;
	  vdfdt.data = 0;
	  vdfdt.block = 0;
	  vdfdt.owner = 0;
	  dfdtv.wrap_gsl_vector_without_ownership( &vdfdt );
	}
      public:
	/**
	 * A static function that gsl::odeiv2_system can use.
	 * @param t The first (time) paramter
	 * @param y The second (y) paramter
	 * @param dydt The derivatives of y with respect to t
	 * @param params Always a pointer to an object of type Function
	 * @return GSL_SUCCESS on success; otherwise GSL_EBADFUNC
	 */
	static int function( double const t, double const* y, double* dydt, void* params ){
	  Function<T>* this_t = reinterpret_cast<Function<T>*>( params );
	  return this_t->function_detail( t, y, dydt );
	}
	/**
	 * A static function that gsl::odeiv2_system can use.
	 * @param t The first (time) paramter
	 * @param y The second (y) paramter
	 * @param dydt The derivatives of y with respect to t
	 * @param params Always a pointer to an object of type Function
	 * @return GSL_SUCCESS on success; otherwise GSL_EBADFUNC
	 */
	static int jacobian( double const t, double const* y, double* dfdy, double* dfdt,
			     void* params ){
	  Function<T>* this_t = reinterpret_cast<Function<T>*>( params );
	  return this_t->jacobian_detail( t, y, dfdy, dfdt );
	}
      private:
	/**
	 * Create a class from bool
	 */
	template<bool N> struct bool_to_type {};
	/**
	 * Identify function with gsl::vector arguments.
	 */
	template<typename V>
	struct Fvector {
	  template<typename W,int (V::*)(double const,gsl::vector const&,gsl::vector&)> struct S{};
	  template<typename W> static char test( S<W,&W::function>* );
	  template<typename W,int (V::*)(double const,gsl::vector&,gsl::vector&)> struct Su{};
	  template<typename W> static char test( Su<W,&W::function>* );
	  template<typename W> static int test( ... );
	  static const bool vector = sizeof(test<V>( 0 )) == sizeof(char);
	};
	/**
	 * Enumerated type for jacobian
	 */
	enum { ptr_ptr = 0, ptr_vec = 1, ptr_mat = 2, vec_ptr = 4, vec_vec = 5, vec_mat = 6,
	       undefined = 8 };
	/**
	 * Identify jacobian arguments. Note empty structs occupy no space.
	 */
	template<typename V>
	struct Jacobian {
	  template<typename W,int (V::*)(double const,double const*,double*,double const*)>
	  struct PtrPtr{};
	  template<typename W> static char test_ptr_ptr( PtrPtr<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,double*,double*,double*)>
	  struct UPtrPtr{};
	  template<typename W> static char test_ptr_ptr( UPtrPtr<W,&W::jacobian>* );
	  template<typename W> static int test_ptr_ptr( ... );
	  template<typename W,int (V::*)(double const,double const*,gsl::vector&,double*)>
	  struct PtrVec{};
	  template<typename W> static char test_ptr_vec( PtrVec<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,double*,gsl::vector&,double*)>
	  struct UPtrVec{};
	  template<typename W> static char test_ptr_vec( UPtrVec<W,&W::jacobian>* );
	  template<typename W> static int test_ptr_vec( ... );
	  template<typename W,int (V::*)(double const,double const*,gsl::matrix&,double*)>
	  struct PtrMat{};
	  template<typename W> static char test_ptr_mat( PtrMat<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,double*,gsl::matrix&,double*)>
	  struct UPtrMat{};
	  template<typename W> static char test_ptr_mat( UPtrMat<W,&W::jacobian>* );
	  template<typename W> static int test_ptr_mat( ... );
	  template<typename W,int (V::*)(double const,gsl::vector const&,double*,gsl::vector&)>
	  struct VecPtr{};
	  template<typename W> static char test_vec_ptr( VecPtr<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,gsl::vector&,double*,gsl::vector&)>
	  struct UVecPtr{};
	  template<typename W> static char test_vec_ptr( UVecPtr<W,&W::jacobian>* );
	  template<typename W> static int test_vec_ptr( ... );
	  template<typename W,int (V::*)(double const,gsl::vector const&,gsl::vector&,gsl::vector&)>
	  struct VecVec{};
	  template<typename W> static char test_vec_vec( VecVec<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,gsl::vector&,gsl::vector&,gsl::vector&)>
	  struct UVecVec{};
	  template<typename W> static char test_vec_vec( UVecVec<W,&W::jacobian>* );
	  template<typename W> static int test_vec_vec( ... );
	  template<typename W,int (V::*)(double const,gsl::vector const&,gsl::matrix,gsl::vector&)>
	  struct VecMat{};
	  template<typename W> static char test_vec_mat( VecMat<W,&W::jacobian>* );
	  template<typename W,int (V::*)(double const,gsl::vector&,gsl::matrix,gsl::vector&)>
	  struct UVecMat{};
	  template<typename W> static char test_vec_mat( UVecMat<W,&W::jacobian>* );
	  template<typename W> static int test_vec_mat( ... );
	  static const int type
	  = sizeof(test_ptr_ptr<V>( 0 )) == sizeof(char) ? ptr_ptr :
	    sizeof(test_ptr_vec<V>( 0 )) == sizeof(char) ? ptr_vec :
	    sizeof(test_ptr_mat<V>( 0 )) == sizeof(char) ? ptr_mat :
	    sizeof(test_vec_ptr<V>( 0 )) == sizeof(char) ? vec_ptr :
	    sizeof(test_vec_vec<V>( 0 )) == sizeof(char) ? vec_vec :
	    sizeof(test_vec_mat<V>( 0 )) == sizeof(char) ? vec_mat :
	    undefined;
	};
	/**
	 * Define has_jacobian according as T defines a jacobian
	 */
	bool has_jacobian() const { return Jacobian<T>::type != undefined; }
	/**
	 * Create a class from int
	 */
	template<int N> struct int_to_type {};
	/**
	 * Function with vector arguments
	 */
	template<typename U>
	int function( double const t, double const* y, double* dydt, U& u, bool_to_type<true> ){
	  vy.data = const_cast<double*>( y );
	  vdydt.data = dydt;
	  return u.function( t, yv, dydtv ); 
	}
	/**
	 * Function with double* arguments
	 */
	template<typename U>
	int function( double const t, double const* y, double* dydt, U& u, bool_to_type<false> ){
	  return u.function( t, y, dydt ); 
	}
	/**
	 * Function detail
	 */
	int function_detail( double const t, double const* y, double* dydt ){
	  return function( t, y, dydt, this->t, bool_to_type<Fvector<T>::vector>() );
	}
	/**
	 * Jacobian undefined.
	 */
	template<typename U>
	int jacobian( double const t, double const* y, double* dfdy, double* dfdt,
		      U& u, int_to_type<undefined> ){
	  return 0;
	}
	/**
	 * Jacobian with double* arguments.
	 */
	template<typename U>
	int jacobian( double const t, double const* y, double* dfdy, double* dfdt,
		      U& u, int_to_type<ptr_ptr> ){
	  return u.jacobian( t, y, dfdy, dfdt );
	}
	/**
	 * Jacobian with double*, and gsl::vector arguments.
	 */
	template<typename U>
	int jacobian( double const t, double const* y, gsl::vector& dfdy, double* dfdt,
		      U& u, int_to_type<ptr_vec> ){
	  vdfdy.data = dfdy; 
	  return u.jacobian( t, y, dfdyv, dfdt );
	}
	/**
	 * Jacobian with double*, and gsl::matrix arguments.
	 */
	template<typename U>
	int jacobian( double const t, double const* y, gsl::vector& dfdy, double* dfdt,
		      U& u, int_to_type<ptr_mat> ){
	  mdfdy.data = dfdy; 
	  return u.jacobian( t, y, dfdym, dfdt );
	}
	/**
	 * Jacobian with gsl::vector and double* arguments.
	 */
	template<typename U>
	int jacobian( double const t, gsl::vector const& y, double* dfdy, gsl::vector& dfdt,
		      U& u, int_to_type<vec_ptr> ){
	  vdfdt.data = dfdt;
	  vy.data = y;
	  return u.jacobian( t, yv, dfdy, dfdtv );
	}
	/**
	 * Jacobian with gsl::vector arguments.
	 */
	template<typename U>
	int jacobian( double const t, gsl::vector const& y, gsl::vector& dfdy, gsl::vector& dfdt,
		      U& u, int_to_type<vec_vec> ){
	  vdfdt.data = dfdt; 
	  vy.data = y;
	  vdfdy.data = dfdy; 
	  return u.jacobian( t, yv, dfdyv, dfdtv );
	}
	/**
	 * Jacobian with gsl::vector and gsl::matrix arguments.
	 * This is the most obvious form.
	 */
	template<typename U>
	int jacobian( double const t, gsl::vector const& y, gsl::matrix& dfdy, gsl::vector& dfdt,
		      U& u, int_to_type<vec_mat> ){
	  vdfdt.data = dfdt; 
	  vy.data = y;
	  mdfdy.data = dfdy; 
	  return u.jacobian( t, yv, dfdym, dfdtv );
	}
	/**
	 * Jacobian detail
	 */
	int jacobian_detail( double const t, double const* y, double* dfdy, double* dfdt ){
	  return jacobian( t, y, dfdy, dfdt, this->t, int_to_type<Jacobian<T>::type>() );
	}
	/**
	 * A gsl_vector object for y
	 */
	gsl_vector vy;
	/**
	 * A gsl::vector object for y.
	 */
	gsl::vector yv;
	/**
	 * A gsl_vector object for dydt
	 */
	gsl_vector vdydt;
	/**
	 * A gsl::vector object for dydt.
	 */
	gsl::vector dydtv;
	/**
	 * A gsl_vector object for dfdy
	 */
	gsl_vector vdfdy;
	/**
	 * A gsl::vector object for dfdy.
	 */
	gsl::vector dfdyv;
	/**
	 * A gsl_matrix object for dfdy
	 */
	gsl_matrix mdfdy;
	/**
	 * A gsl::matrix object for dfdy.
	 */
	gsl::matrix dfdym;
	/**
	 * Reference to function object from which this was constructed.
	 */
	/**
	 * A gsl_vector object for dfdt
	 */
	gsl_vector vdfdt;
	/**
	 * A gsl::vector object for dfdt.
	 */
	gsl::vector dfdtv;
	T& t;
      };
#endif
      /**
       * This gives something for params to point to.
       */
      base_F* f;
      /**
       * The shared reference count: used for copying this.
       */
      size_t* count;
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      system(){
	f = 0;
	function = 0;
	jacobian = 0;
	dimension = 0;
	params = 0;
	count = 0; // initially nullptr will do
      }
      template<typename T>
      friend void system_constructor( system& sys, T& t );
      /**
       * Could construct from a gsl_odeiv2_system object. This constuctor is not actually visible,
       * but gsl::odeiv2::system::system<T>() will construct the object correctly.
       * @param v The gsl_odeiv2_system.
       */
      explicit system( gsl_odeiv2_system& v ){
	// explicitly set f = 0 to indicate nothing to be deleted.
	f = 0;
	// copy
	function = v.function;
	jacobian = v.jacobian;
	dimension = v.dimension;
	params = v.params;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to function
      }
      /**
       * Construct from an object that implements gsl::odeiv2::system::concept.
       * @param t An object that implements gsl::odeiv2::system::concept.
       */
      template<typename T>
      system( T& t ){ system_constructor<T>( *this, t ); }
       
      // Copy and move constructors
      // copy constructor
      /**
       * The copy constructor. This shares f. This constuctor is not actually visible, but
       * gsl::odeiv2::system::system<T>()
       * will construct the object correctly.
       * @param v The system to copy.
       */
      system( system const& v ) : f( v.f ), count( v.count ){
	function = v.function;
	jacobian = v.jacobian;
	dimension = v.dimension;
	params = v.params;
	if( count != 0 ) ++*count; // system is now shared.
      }
      // assignment operator
      /**
       * The assignment operator. This makes a shared copy.
       * @param v The system to copy
       */
      system& operator=( system const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  delete f;
	  delete count;
	}
	// Then copy
	f = v.f;
	function = v.function;
	jacobian = v.jacobian;
	dimension = v.dimension;
	params = v.params;
	// count = v.count;
	if( count != 0 ) ++*count; // system is now shared.
	return *this;
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The system to move.
       */
      system( system&& v ) : f( v.f ), count( nullptr ){
	std::swap( function, v.function );
	std::swap( jacobian, v.jacobian );
	std::swap( dimension, v.dimension );
	std::swap( params, v.params );
	std::swap( count, v.count );
	v.function = nullptr;
      }
      /**
       * Move operator.
       * @param v The system to move.
       * @return A reference to f in this.
       */
      system& operator=( system&& v ){
	std::swap( f, v.f );
	std::swap( function, v.function );
	std::swap( jacobian, v.jacobian );
	std::swap( dimension, v.dimension );
	std::swap( params, v.params );
	std::swap( count, v.count );
	return *this;
      }
#endif
      /**
       * The destructor unshares any shared resource.
       */
      ~system(){ 
	// first, possibly delete anything pointed to by f
	if( count == 0 or --*count == 0 ){
	  delete f;
	  delete count;
	}
      }
    };
#ifndef DOXYGEN_SKIP
    /**
     * Specialisation for copy constructor.
     */ 
    template<>
    inline void system_constructor( system& sys, system& v ){
      sys.f = v.f; sys.function = v.function; sys.jacobian = v.jacobian;
      sys.dimension = v.dimension; sys.params = v.params;
      if( sys.count != 0 ) ++*sys.count; // system is now shared.
      /* sys is now shared. */ }
    /**
     * Specialisation for gsl_odeiv2_system.
     */ 
    template<>
    inline void system_constructor( system& sys, gsl_odeiv2_system& v ){
      sys.function = v.function; sys.jacobian = v.jacobian;
      sys.dimension = v.dimension; sys.params = v.params; }
    /**
     * This function allows us to specialise constructors with one argument. It handles three
     * cases: constructing from an object that implements concept; constructing from a
     * system object; and copy construction.
     */
    template<typename T>
    inline void system_constructor( system& sys, T& t ){
      sys.function = 0;
      sys.count = 0;
      // try constructing from T
      sys.f = new system::Function<T>( t );
      sys.function = &system::Function<T>::function;
      if( sys.f->has_jacobian() )
	sys.jacobian = 0;
      else
	sys.jacobian = &system::Function<T>::jacobian;
      sys.dimension = t.size();
      sys.params = sys.f;
      // just plausibly we could fail to allocate count: no further action needed.
      sys.count = new size_t;
      *sys.count = 1; // initially there is just one reference to function
    }
#endif
  
    /**
     * Make a gsl::odeiv2::system from a function object that implements 
     * gsl::odeiv2::system::concept.
     * @param t The function object.
     */
    template<typename T>
    inline system make_system( T& t ){
      system fn( t );
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      return std::move( fn );
#else
      return fn;
#endif
    }
      
  }
}
#endif
