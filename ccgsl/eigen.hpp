/*
 * $Id: eigen.hpp 16 2011-12-26 12:54:57Z jdl3 $
 * Copyright (C) 2010, 2011 John D Lamb
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

#ifndef CCGSL_EIGEN_HPP
#define CCGSL_EIGEN_HPP

#include"eigen_symm.hpp"
#include"eigen_herm.hpp"
#include"eigen_nonsymm.hpp"
#include"eigen_gensymm.hpp"
#include"eigen_genherm.hpp"
#include"eigen_gen.hpp"

namespace gsl {
  namespace eigen {
    /**
     * Sort in ascending order.
     */
    gsl_eigen_sort_t const SORT_VAL_ASC = GSL_EIGEN_SORT_VAL_ASC;
    /**
     * Sort in descending order.
     */
    gsl_eigen_sort_t const SORT_VAL_DESC = GSL_EIGEN_SORT_VAL_DESC;
    /**
     * Sort in ascending order by absoulte size.
     */
    gsl_eigen_sort_t const SORT_VAL_ABS_ASC = GSL_EIGEN_SORT_ABS_ASC;
    /**
     * Sort in descending order by absolute size.
     */
    gsl_eigen_sort_t const SORT_VAL_ABS_DESC = GSL_EIGEN_SORT_ABS_DESC;
    /**
     * C++ version of gsl_eigen_symmv_sort().
     * @param eval A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int symmv_sort( vector& eval, matrix& evec, gsl_eigen_sort_t sort_type ){
      return gsl_eigen_symmv_sort( eval.get(), evec.get(), sort_type ); } 
    /**
     * C++ version of gsl_eigen_hermv_sort().
     * @param eval A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int hermv_sort( vector& eval, matrix_complex& evec, gsl_eigen_sort_t sort_type ){
      return gsl_eigen_hermv_sort( eval.get(), evec.get(), sort_type ); } 
    /**
     * C++ version of gsl_eigen_nonsymmv_sort().
     * @param eval A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int nonsymmv_sort( vector_complex& eval, matrix_complex& evec,
			      gsl_eigen_sort_t sort_type ){
          return gsl_eigen_nonsymmv_sort( eval.get(), evec.get(), sort_type ); } 
    /**
     * C++ version of gsl_eigen_gensymmv_sort().
     * @param eval A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int gensymmv_sort( vector& eval, matrix& evec, gsl_eigen_sort_t sort_type ){
      return gsl_eigen_gensymmv_sort( eval.get(), evec.get(), sort_type ); } 
    /**
     * C++ version of gsl_eigen_genhermv_sort().
     * @param eval A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int genhermv_sort( vector& eval, matrix_complex& evec, gsl_eigen_sort_t sort_type ){
      return gsl_eigen_genhermv_sort( eval.get(), evec.get(), sort_type ); } 
    /**
     * C++ version of gsl_eigen_genv_sort().
     * @param alpha A vector of eigenvalues
     * @param beta A vector of eigenvalues
     * @param evec A matrix of eigenvectors
     * @param sort_type Specify the ordering of the eigenvalues and eigenvectors
     * @return GSL_SUCCESS or GSL_EBADLEN
     */
    inline int genv_sort( vector_complex& alpha, vector& beta, matrix_complex& evec,
			  gsl_eigen_sort_t sort_type ){
      return gsl_eigen_genv_sort( alpha.get(), beta.get(), evec.get(), sort_type ); }
  }
}

#endif
