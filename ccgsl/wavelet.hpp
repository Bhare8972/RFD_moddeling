/*
 * $Id: wavelet.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_WAVELET_HPP
#define CCGSL_WAVELET_HPP

#include<new>
#include<gsl/gsl_wavelet.h>

namespace gsl {
  /**
   * Wavelets in one dimension.
   */
  class wavelet {
  public:
    /**
     * Define gsl::wavelet::static type.
     */
    typedef gsl_wavelet_type const* type;
    /**
     * The default constructor is only really useful for assigning to.
     */
    wavelet(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new wavelet with n elements.
     * @param T The type of the wavelet
     * @param k Parameter to select specific member of wavelet family
     */
    explicit wavelet( type& T, size_t const k ){
      ccgsl_pointer = gsl_wavelet_alloc( T, k );
      // just plausibly we could allocate wavelet but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_wavelet_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_wavelet.
     * This is not usually a good idea. In this case
     * we should not use gsl_wavelet_free() to deallocate the memory.
     * @param v The wavelet
     */
    explicit wavelet( gsl_wavelet* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The wavelet to copy.
     */
    wavelet( wavelet const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The wavelet to copy
     */
    wavelet& operator=( wavelet const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_wavelet_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~wavelet(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_wavelet_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The wavelet to move.
     */
    wavelet( wavelet&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The wavelet to move.
     * @return A reference to this.
     */
    wavelet& operator=( wavelet&& v ){
      wavelet( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two wavelet are identically equal if their elements are identical.
     * @param v The wavelet to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( wavelet const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two wavelet are different if their elements are not identical.
     * @param v The wavelet to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( wavelet const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a wavelet is nonnegative.
     * @param v The wavelet to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( wavelet const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a wavelet is nonnegative.
     * @param v The wavelet to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( wavelet const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a wavelet is nonnegative.
     * @param v The wavelet to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( wavelet const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a wavelet is nonnegative.
     * @param v The wavelet to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( wavelet const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the wavelet is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two wavelet objects. This works even if the wavelet have different sizes
     * because it swaps pointers.
     * @param v The wavelet to swap with @c this.
     */
    void swap( wavelet& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  protected:
    /**
     * The shared pointer
     */
    gsl_wavelet* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_wavelet.
     * @return the gsl_wavelet
     */
    gsl_wavelet* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_wavelet.
     * @return @c true or @c falses according as 
     * this is the only wavelet object sharing the gsl_wavelet.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many wavelet objects share this pointer.
     * @return the number of wavelet objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_wavelet.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }
    /**
     * C++ version of gsl_wavelet_name().
     * @return UNDOCUMENTED
     */
    std::string name() const { return gsl_wavelet_name( get() ); }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type daubechies(){ return gsl_wavelet_daubechies; }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type daubechies_centered(){ return gsl_wavelet_daubechies_centered; }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type haar(){ return gsl_wavelet_haar; }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type haar_centered(){ return gsl_wavelet_haar_centered; }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type bspline(){ return gsl_wavelet_bspline; }
    /**
     * Wavelet static type.
     * @return the named wavelet type
     */
    static type bspline_centered(){ return gsl_wavelet_bspline_centered; }

    /**
     * Workspace for wavelets
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
       * @param n The number of elements in the workspace
       */
      explicit workspace( size_t const n ){
	ccgsl_pointer = gsl_wavelet_workspace_alloc( n );
	// just plausibly we could allocate workspace but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_wavelet_workspace_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_wavelet_workspace.
       * This is not usually a good idea. In this case
       * we should not use gsl_wavelet_workspace_free() to deallocate the memory.
       * @param v The workspace
       */
      explicit workspace( gsl_wavelet_workspace* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The workspace to copy.
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
	  if( ccgsl_pointer != 0 ) gsl_wavelet_workspace_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_wavelet_workspace_free( ccgsl_pointer );
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
       * for example, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( workspace const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( workspace const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a workspace is nonnegative.
       * @param v The workspace to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( workspace const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a workspace is nonnegative.
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
       * Swap two workspace objects. This works even if the workspace have different sizes
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
      gsl_wavelet_workspace* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_wavelet_workspace.
       * @return the gsl_wavelet_workspace
       */
      gsl_wavelet_workspace* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_wavelet_workspace.
       * @return @c true or @c falses according as 
       * this is the only workspace object sharing the gsl_wavelet_workspace.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many workspace objects share this pointer.
       * @return the number of workspace objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_wavelet_workspace.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }
    };
    
    /**
     * Access direction
     * @return direction
     */
    inline gsl_wavelet_direction forward() const { return gsl_wavelet_forward; }
    /**
     * Access direction
     * @return direction
     */
    inline gsl_wavelet_direction backward() const { return gsl_wavelet_backward; }

    /**
     * C++ version of gsl_wavelet_transform().
     * @param data Data array
     * @param stride Stride 
     * @param n Size of array
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    int transform( double* data, size_t stride, size_t n, gsl_wavelet_direction dir,
		   workspace& work ) const {
      return gsl_wavelet_transform( get(), data, stride, n, dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_forward().
     * @param data Data array
     * @param stride Stride
     * @param n Size of array
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    int transform_forward( double* data, size_t stride, size_t n, workspace& work ) const {
      return gsl_wavelet_transform_forward( get(), data, stride, n, work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_inverse().
     * @param data Data array
     * @param stride Stride
     * @param n Size of array
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    int transform_inverse( double* data, size_t stride, size_t n, workspace& work ) const {
      return gsl_wavelet_transform_inverse( get(), data, stride, n, work.get() ); } 

    /**
     * C++ version of gsl_wavelet_transform().
     * This version works on std::vector<double> and gsl::vector
     * @param data Data array
     * @param stride Stride
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform( DATA& data, size_t stride, gsl_wavelet_direction dir,
		   workspace& work ) const {
      return gsl_wavelet_transform( get(), data.data(), stride, data.size() / stride, dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_forward().
     * This version works on std::vector<double> and gsl::vector
     * @param data Data array
     * @param stride Stride
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform_forward( DATA& data, size_t stride, workspace& work ) const {
      return gsl_wavelet_transform_forward( get(), data.data(), stride, data.size() / stride, work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_inverse().
     * This version works on std::vector<double> and gsl::vector
     * @param data Data array
     * @param stride Stride
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform_inverse( DATA& data, size_t stride, workspace& work ) const {
      return gsl_wavelet_transform_inverse( get(), data.data(), stride, data.size() / stride, work.get() ); } 

    /**
     * C++ version of gsl_wavelet_transform().
     * This version handles std::vector and gsl::vector.
     * @param data Data array
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform( DATA& data, gsl_wavelet_direction dir,
		   workspace& work ) const {
      return gsl_wavelet_transform( get(), data.data(), 1, data.size(), dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_forward().
     * This version handles std::vector and gsl::vector.
     * @param data Data array
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform_forward( DATA& data, workspace& work ) const {
      return gsl_wavelet_transform_forward( get(), data.data(), 1, data.size(), work.get() ); }

    /**
     * C++ version of gsl_wavelet_transform_inverse().
     * This version handles std::vector and gsl::vector.
     * @param data Data array
     * @param work Supply a workspace of size n
     * @return Error code on failure
     */
    template<typename DATA>
    int transform_inverse( DATA& data, workspace& work ) const {
      return gsl_wavelet_transform_inverse( get(), data.data(), 1, data.size(), work.get() ); }

  };
}
#endif
