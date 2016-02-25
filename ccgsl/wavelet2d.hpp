/*
 * $Id: wavelet2d.hpp 220 2012-08-07 16:29:22Z jdl3 $
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

#ifndef CCGSL_WAVELET2D_HPP
#define CCGSL_WAVELET2D_HPP

#include<gsl/gsl_wavelet2d.h>
#include"wavelet.hpp"
#include"matrix.hpp"

namespace gsl {
  /**
   * Wavelets in two dimensions.
   */
  class wavelet2d : public wavelet {
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new wavelet2d with n elements.
     * @param T The type of the wavelet2d
     * @param k Parameter to select specific member of wavelet2d family
     */
    explicit wavelet2d( type& T, size_t const k ) : wavelet( T, k ){}
    /**
     * Could construct from a gsl_wavelet.
     * This is not usually a good idea. In this case
     * we should not use gsl_wavelet_free() to deallocate the memory.
     * @param v The wavelet
     */
    explicit wavelet2d( gsl_wavelet* v ) : wavelet( v ){}
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The wavelet2d to copy.
     */
    wavelet2d( wavelet2d const& v ) : wavelet( v ){}
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The wavelet2d to copy
     */
    wavelet2d& operator=( wavelet2d const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_wavelet_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The wavelet2d to move.
     */
    wavelet2d( wavelet2d&& v ){
      ccgsl_pointer = v.ccgsl_pointer;
      count = nullptr;
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The wavelet2d to move.
     * @return A reference to this.
     */
    wavelet2d& operator=( wavelet2d&& v ){
      wavelet2d( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    
    /**
     * C++ version of gsl_wavelet2d_transform().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform( double* data, size_t tda, size_t size1, size_t size2,
		   gsl_wavelet_direction dir, wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform( get(), data, tda, size1, size2, dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_transform_forward().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform_forward( double* data, size_t tda, size_t size1, size_t size2,
			   wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform_forward( get(), data, tda, size1, size2, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_transform_inverse().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform_inverse( double* data, size_t tda, size_t size1, size_t size2,
			   wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform_inverse( get(), data, tda, size1, size2, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_nstransform().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform( double* data, size_t tda, size_t size1, size_t size2,
		     gsl_wavelet_direction dir, wavelet::workspace work ) const {
      return gsl_wavelet2d_nstransform( get(), data, tda, size1, size2, dir, work.get() );
    }

    /**
     * C++ version of gsl_wavelet2d_nstransform_forward().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform_forward( double* data, size_t tda, size_t size1, size_t size2,
			     wavelet::workspace& work ) const {
      return gsl_wavelet2d_nstransform_forward( get(), data, tda, size1, size2, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_nstransform_inverse().
     * @param data Data array
     * @param tda Physical row length
     * @param size1 Number of rows
     * @param size2 Number of columns
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform_inverse( double* data, size_t tda, size_t size1, size_t size2,
			     wavelet::workspace& work ) const {
      return gsl_wavelet2d_nstransform_inverse( get(), data, tda, size1, size2, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_transform_matrix().
     * @param a A matrix
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform_matrix( matrix& a, gsl_wavelet_direction dir,
			  wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform_matrix( get(), a.get(), dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_transform_matrix_forward().
     * @param a A matrix
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform_matrix_forward( matrix& a, wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform_matrix_forward( get(), a.get(), work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_transform_matrix_inverse().
     * @param a A matrix
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int transform_matrix_inverse( matrix& a, wavelet::workspace& work ) const {
      return gsl_wavelet2d_transform_matrix_inverse( get(), a.get(), work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_nstransform_matrix().
     * @param a A matrix
     * @param dir Direction: gsl_wavelet-forward() or gsl_wavelet_backward()
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform_matrix( matrix& a, gsl_wavelet_direction dir,
			    wavelet::workspace& work ) const {
      return gsl_wavelet2d_nstransform_matrix( get(), a.get(), dir, work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_nstransform_matrix_forward().
     * @param a A matrix
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform_matrix_forward( matrix& a, wavelet::workspace& work ) const {
      return gsl_wavelet2d_nstransform_matrix_forward( get(), a.get(), work.get() ); }

    /**
     * C++ version of gsl_wavelet2d_nstransform_matrix_inverse().
     * @param a A matrix
     * @param work Supply a workspace of appropriate (undocumented) size
     * @return Error code on failure
     */
    int nstransform_matrix_inverse( matrix& a, wavelet::workspace& work ) const {
      return gsl_wavelet2d_nstransform_matrix_inverse( get(), a.get(), work.get() ); }
    
  };
}
#endif
