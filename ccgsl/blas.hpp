/*
 * $Id: blas.hpp 16 2011-12-26 12:54:57Z jdl3 $
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

#ifndef CCGSL_CBLAS_HPP
#define CCGSL_CBLAS_HPP

#include<cmath>
#include<gsl/gsl_blas.h>
#include<vector>
#include"vector_float.hpp"
#include"vector_complex.hpp"
#include"vector_complex_float.hpp"
#include"matrix.hpp"
#include"matrix_float.hpp"
#include"matrix_complex.hpp"
#include"matrix_complex_float.hpp"

namespace gsl {
  /**
   * This namespace gives access to all the gsl_cblas functions.
   */
  namespace blas {
    CBLAS_TRANSPOSE_t  const NoTrans = CblasNoTrans; /**< No transpose */
    CBLAS_TRANSPOSE_t const Trans = CblasTrans; /**< Transpose */
    CBLAS_TRANSPOSE_t const ConjTrans = CblasConjTrans; /**< Hermitian transpose */
    CBLAS_UPLO_t const Upper = CblasUpper; /**< Upper triangular */
    CBLAS_UPLO_t const Lower = CblasLower; /**< Lower triangular */
    CBLAS_DIAG_t const NonUnit = CblasNonUnit; /**< Non unit diagonal */
    CBLAS_DIAG_t const Unit = CblasUnit; /**< Unit diagonal */
    CBLAS_SIDE_t const Left = CblasLeft; /**< Apply to left matrix */
    CBLAS_SIDE_t const Right = CblasRight; /**< Apply to right matrix */

    // Level 1
    
    /**
     * C++ version of gsl_blas_dsdot().
     * @param X First vector
     * @param Y Second vector
     * @param result Vector product
     * @return Error code on failure
     */
    inline int dsdot( vector_float const& X, vector_float const& Y, double* result ){
      return gsl_blas_dsdot( X.get(), Y.get(), result ); }

    /**
     * C++ version of gsl_blas_sdot().
     * @param X First vector
     * @param Y Second vector
     * @param result Vector product
     * @return Error code on failure
     */
    inline int sdot( vector_float const& X, vector_float const& Y, float* result ){
      return gsl_blas_sdot( X.get(), Y.get(), result ); }

    /**
     * C++ version of gsl_blas_ddot().
     * @param X First vector
     * @param Y Second vector
     * @param result Vector product
     * @return Error code on failure
     */
    inline int ddot( vector const& X, vector const& Y, double* result ){
      return gsl_blas_ddot( X.get(), Y.get(), result ); }

    /**
     * C++ version of gsl_blas_cdotu().
     * @param X First vector
     * @param Y Second vector
     * @param dotu Vector product
     * @return Error code on failure
     */
    inline int cdotu( vector_complex_float const& X, vector_complex_float const& Y, complex_float* dotu ){
      return gsl_blas_cdotu( X.get(), Y.get(), &dotu->get() ); }

    /**
     * C++ version of gsl_blas_cdotc().
     * @param X First vector
     * @param Y Second vector
     * @param dotc Vector product
     * @return Error code on failure
     */
    inline int cdotc( vector_complex_float const& X, vector_complex_float const& Y, complex_float* dotc ){
      return gsl_blas_cdotc( X.get(), Y.get(), &dotc->get() ); }

    /**
     * C++ version of gsl_blas_zdotu().
     * @param X First vector
     * @param Y Second vector
     * @param dotu Vector product
     * @return Error code on failure
     */
    inline int zdotu( vector_complex const& X, vector_complex const& Y, complex* dotu ){
      return gsl_blas_zdotu( X.get(), Y.get(), &dotu->get() ); }

    /**
     * C++ version of gsl_blas_zdotc().
     * @param X First vector
     * @param Y Second vector
     * @param dotc Vector product
     * @return Error code on failure
     */
    inline int zdotc( vector_complex const& X, vector_complex const& Y, complex* dotc ){
      return gsl_blas_zdotc( X.get(), Y.get(), &dotc->get() ); }

    /**
     * C++ version of gsl_blas_snrm2().
     * @param X A vector
     * @return The Euclidean norm
     */
    inline float snrm2( vector_float const& X ){ return gsl_blas_snrm2( X.get() ); }

    /**
     * C++ version of gsl_blas_sasum().
     * @param X A vector
     * @return The absolute sum of the elements
     */
    inline float sasum( vector_float const& X ){ return gsl_blas_sasum( X.get() ); }

    /**
     * C++ version of gsl_blas_dnrm2().
     * @param X A vector
     * @return The Euclidean norm
     */
    inline double dnrm2( vector const& X ){ return gsl_blas_dnrm2( X.get() ); }

    /**
     * C++ version of gsl_blas_dasum().
     * @param X A vector
     * @return The absolute sum of the elements
     */
    inline double dasum( vector const& X ){ return gsl_blas_dasum( X.get() ); }

    /**
     * C++ version of gsl_blas_scnrm2().
     * @param X A vector
     * @return The Euclidean norm
     */
    inline float scnrm2( vector_complex_float const& X ){ return gsl_blas_scnrm2( X.get() ); }

    /**
     * C++ version of gsl_blas_scasum().
     * @param X A vector
     * @return The absolute sum of the elements
     */
    inline float scasum( vector_complex_float const& X ){ return gsl_blas_scasum( X.get() ); }

    /**
     * C++ version of gsl_blas_dznrm2().
     * @param X A vector
     * @return The Euclidean norm
     */
    inline double dznrm2( vector_complex const& X ){ return gsl_blas_dznrm2( X.get() ); }

    /**
     * C++ version of gsl_blas_dzasum().
     * @param X A vector
     * @return The absolute sum of the elements
     */
    inline double dzasum( vector_complex const& X ){ return gsl_blas_dzasum( X.get() ); }

    /**
     * C++ version of gsl_blas_isamax().
     * @param X A vector
     * @return Index of the largest-magnitude element
     */
    inline CBLAS_INDEX_t isamax( vector_float const& X ){ return gsl_blas_isamax( X.get() ); }

    /**
     * C++ version of gsl_blas_idamax().
     * @param X A vector
     * @return Index of largest-magnitude element
     */
    inline CBLAS_INDEX_t idamax( vector const& X ){ return gsl_blas_idamax( X.get() ); }

    /**
     * C++ version of gsl_blas_icamax().
     * @param X A vector
     * @return Index of largest-magnitude element
     */
    inline CBLAS_INDEX_t icamax( vector_complex_float const& X ){ return gsl_blas_icamax( X.get() ); }

    /**
     * C++ version of gsl_blas_izamax().
     * @param X A vector
     * @return Index of largest-magnitude element
     */
    inline CBLAS_INDEX_t izamax( vector_complex const& X ){ return gsl_blas_izamax( X.get() ); }

    /**
     * C++ version of gsl_blas_sswap().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int sswap( vector_float& X, vector_float& Y ){ return gsl_blas_sswap( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_scopy().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int scopy( vector_float const& X, vector_float& Y ){ return gsl_blas_scopy( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_saxpy().
     * @param alpha A vector
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int saxpy( float alpha, vector_float const& X, vector_float& Y ){
      return gsl_blas_saxpy( alpha, X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_dswap().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int dswap( vector& X, vector& Y ){ return gsl_blas_dswap( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_dcopy().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int dcopy( vector const& X, vector& Y ){ return gsl_blas_dcopy( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_daxpy().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int daxpy( double alpha, vector const& X, vector& Y ){
      return gsl_blas_daxpy( alpha, X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_cswap().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int cswap( vector_complex_float& X, vector_complex_float& Y ){
      return gsl_blas_cswap( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_ccopy().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int ccopy( vector_complex_float const& X, vector_complex_float& Y ){
      return gsl_blas_ccopy( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_caxpy().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int caxpy( complex_float const& alpha, vector_complex_float const& X, vector_complex_float& Y ){
      return gsl_blas_caxpy( alpha.get(), X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_zswap().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int zswap( vector_complex& X, vector_complex& Y ){
      return gsl_blas_zswap( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_zcopy().
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int zcopy( vector_complex const& X, vector_complex& Y ){
      return gsl_blas_zcopy( X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_zaxpy().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @return Error code on failure
     */
    inline int zaxpy( complex const& alpha, vector_complex const& X, vector_complex& Y ){
      return gsl_blas_zaxpy( alpha.get(), X.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_srotg().
     * @param a An array
     * @param b An array
     * @param c An array
     * @param s An array
     * @return Error code on failure
     */
    inline int srotg( float a[], float b[], float c[], float s[] ){ return gsl_blas_srotg( a, b, c, s ); }

    /**
     * C++ version of gsl_blas_srotmg().
     * @param d1 An array
     * @param d2 An array
     * @param b1 An array
     * @param b2 A constant
     * @param P An array
     * @return Error code on failure
     */
    inline int srotmg( float d1[], float d2[], float b1[], float b2, float P[] ){
          return gsl_blas_srotmg( d1, d2, b1, b2, P ); }

    /**
     * C++ version of gsl_blas_srot().
     * @param X A vector
     * @param Y A vector
     * @param c A constant
     * @param s A constant
     * @return Error code on failure
     */
    inline int srot( vector_float& X, vector_float& Y, float c, float s ){
      return gsl_blas_srot( X.get(), Y.get(), c, s );
    }

    /**
     * C++ version of gsl_blas_srotm().
     * @param X A vector
     * @param Y A vector
     * @param P An array
     * @return Error code on failure
     */
    inline int srotm( vector_float& X, vector_float& Y, float const P[] ){
      return gsl_blas_srotm( X.get(), Y.get(), P ); }

    /**
     * C++ version of gsl_blas_drotg().
     * @param a An array
     * @param b An array
     * @param c An array
     * @param s An array
     * @return Error code on failure
     */
    inline int drotg( double a[], double b[], double c[], double s[] ){ return gsl_blas_drotg( a, b, c, s ); }

    /**
     * C++ version of gsl_blas_drotmg().
     * @param d1 An array
     * @param d2 An array
     * @param b1 An array
     * @param b2 A constant
     * @param P An array
     * @return Error code on failure
     */
    inline int drotmg( double d1[], double d2[], double b1[], double b2, double P[] ){
      return gsl_blas_drotmg( d1, d2, b1, b2, P ); }

    /**
     * C++ version of gsl_blas_drot().
     * @param X A vector
     * @param Y A vector
     * @param c A constant
     * @param s A constant
     * @return Error code on failure
     */
    inline int drot( vector& X, vector& Y, double const c, double const s ){
      return gsl_blas_drot( X.get(), Y.get(), c, s ); }

    /**
     * C++ version of gsl_blas_drotm().
     * @param X A vector
     * @param Y A vector
     * @param P An array
     * @return Error code on failure
     */
    inline int drotm( vector& X, vector& Y, double const P[] ){
      return gsl_blas_drotm( X.get(), Y.get(), P ); }

    /**
     * C++ version of gsl_blas_sscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void sscal( float alpha, vector_float& X ){ gsl_blas_sscal( alpha, X.get() ); }

    /**
     * C++ version of gsl_blas_dscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void dscal( double alpha, vector& X ){ gsl_blas_dscal( alpha, X.get() ); }

    /**
     * C++ version of gsl_blas_cscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void cscal( complex_float const& alpha, vector_complex_float& X ){
      gsl_blas_cscal( alpha.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_zscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void zscal( complex const& alpha, vector_complex& X ){ gsl_blas_zscal( alpha.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_csscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void csscal( float alpha, vector_complex_float& X ){ gsl_blas_csscal( alpha, X.get() ); }

    /**
     * C++ version of gsl_blas_zdscal().
     * @param alpha A constant
     * @param X A vector
     */
    inline void zdscal( double alpha, vector_complex& X ){ gsl_blas_zdscal( alpha, X.get() ); }

    /**
     * C++ version of gsl_blas_sgemv().
     * @param TransA Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int sgemv( CBLAS_TRANSPOSE_t TransA, float alpha, matrix_float const& A,
	       vector_float const& X, float beta, vector_float& Y ){
      return gsl_blas_sgemv( TransA, alpha, A.get(), X.get(), beta, Y.get() ); }

    /**
     * C++ version of gsl_blas_strmv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int
    strmv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	   matrix_float const& A, vector_float& X ){
      return gsl_blas_strmv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_strsv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int strsv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, matrix_float const& A, vector_float& X ){
      return gsl_blas_strsv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_dgemv().
     * @param TransA Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int dgemv( CBLAS_TRANSPOSE_t TransA, double alpha, matrix const& A,
	       vector const& X, double beta, vector& Y ){
      return gsl_blas_dgemv( TransA, alpha, A.get(), X.get(), beta, Y.get() ); }

    /**
     * C++ version of gsl_blas_dtrmv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int dtrmv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix const& A, vector& X ){
      return gsl_blas_dtrmv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_dtrsv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int dtrsv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix const& A, vector& X ){
      return gsl_blas_dtrsv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_cgemv().
     * @param TransA Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int cgemv( CBLAS_TRANSPOSE_t TransA, complex_float const& alpha,
	       matrix_complex_float const& A, vector_complex_float const& X,
	       complex_float const& beta, vector_complex_float& Y ){
      return gsl_blas_cgemv( TransA, alpha.get(), A.get(), X.get(), beta.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_ctrmv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int ctrmv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix_complex_float const& A, vector_complex_float& X ){
      return gsl_blas_ctrmv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_ctrsv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int ctrsv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix_complex_float const& A, vector_complex_float& X ){
      return gsl_blas_ctrsv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_zgemv().
     * @param TransA Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int zgemv( CBLAS_TRANSPOSE_t TransA, complex const& alpha, matrix_complex const& A,
	       vector_complex const& X, complex const& beta, vector_complex& Y ){
      return gsl_blas_zgemv( TransA, alpha.get(), A.get(), X.get(), beta.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_ztrmv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int ztrmv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix_complex const& A, vector_complex& X ){
      return gsl_blas_ztrmv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_ztrsv().
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param A A matrix
     * @param X A vector
     * @return Error code on failure
     */
    inline int ztrsv( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA, CBLAS_DIAG_t Diag,
	       matrix_complex const& A, vector_complex& X ){
      return gsl_blas_ztrsv( Uplo, TransA, Diag, A.get(), X.get() ); }

    /**
     * C++ version of gsl_blas_ssymv().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int ssymv( CBLAS_UPLO_t Uplo, float alpha, matrix_float const& A,
	       vector_float const& X, float beta, vector_float& Y ){
      return gsl_blas_ssymv( Uplo, alpha, A.get(), X.get(), beta, Y.get() ); }

    /**
     * C++ version of gsl_blas_sger().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int sger( float alpha, vector_float const& X, vector_float const& Y, matrix_float& A ){
      return gsl_blas_sger( alpha, X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_ssyr().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int ssyr( CBLAS_UPLO_t Uplo, float alpha, vector_float const& X, matrix_float& A ){
      return gsl_blas_ssyr( Uplo, alpha, X.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_ssyr2().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int ssyr2( CBLAS_UPLO_t Uplo, float alpha, vector_float const& X, vector_float const& Y,
	       matrix_float& A ){ return gsl_blas_ssyr2( Uplo, alpha, X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_dsymv().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int dsymv( CBLAS_UPLO_t Uplo, double alpha,
	       matrix const& A, vector const& X, double beta, vector& Y ){
          return gsl_blas_dsymv( Uplo, alpha, A.get(), X.get(), beta, Y.get() ); }

    /**
     * C++ version of gsl_blas_dger().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int dger( double alpha, vector const& X, vector const& Y, matrix& A ){
      return gsl_blas_dger( alpha, X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_dsyr().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int dsyr( CBLAS_UPLO_t Uplo, double alpha, vector const& X, matrix& A ){
      return gsl_blas_dsyr( Uplo, alpha, X.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_dsyr2().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int dsyr2( CBLAS_UPLO_t Uplo, double alpha, vector const& X, vector const& Y, matrix& A ){
      return gsl_blas_dsyr2( Uplo, alpha, X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_chemv().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int chemv( CBLAS_UPLO_t Uplo, complex_float const& alpha,
	       matrix_complex_float const& A, vector_complex_float const& X,
	       complex_float const& beta, vector_complex_float& Y ){
      return gsl_blas_chemv( Uplo, alpha.get(), A.get(), X.get(), beta.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_cgeru().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int cgeru( complex_float const& alpha, vector_complex_float const& X,
        vector_complex_float const& Y, matrix_complex_float& A ){
      return gsl_blas_cgeru( alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_cgerc().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int cgerc( complex_float const& alpha, vector_complex_float const& X,
	       vector_complex_float const& Y, matrix_complex_float& A ){
      return gsl_blas_cgerc( alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_cher().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int cher( CBLAS_UPLO_t Uplo, float alpha, vector_complex_float const& X,
	      matrix_complex_float& A ){
          return gsl_blas_cher( Uplo, alpha, X.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_cher2().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int cher2( CBLAS_UPLO_t Uplo, complex_float const& alpha,
        vector_complex_float const& X, vector_complex_float const& Y, matrix_complex_float& A ){
      return gsl_blas_cher2( Uplo, alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_zhemv().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param X A vector
     * @param beta Another constant
     * @param Y A vector
     * @return Error code on failure
     */
    inline int zhemv( CBLAS_UPLO_t Uplo, complex const& alpha, matrix_complex const& A,
	       vector_complex const& X, complex const& beta, vector_complex& Y ){
      return gsl_blas_zhemv( Uplo, alpha.get(), A.get(), X.get(), beta.get(), Y.get() ); }

    /**
     * C++ version of gsl_blas_zgeru().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int zgeru( complex const& alpha, vector_complex const& X, vector_complex const& Y,
	       matrix_complex& A ){
          return gsl_blas_zgeru( alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_zgerc().
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int zgerc( complex const& alpha, vector_complex const& X, vector_complex const& Y,
	       matrix_complex& A ){
      return gsl_blas_zgerc( alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_zher().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int zher( CBLAS_UPLO_t Uplo, double alpha, vector_complex const& X,
	      matrix_complex& A ){
      return gsl_blas_zher( Uplo, alpha, X.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_zher2().
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param X A vector
     * @param Y A vector
     * @param A A matrix
     * @return Error code on failure
     */
    inline int zher2( CBLAS_UPLO_t Uplo, complex const alpha, vector_complex const& X,
	       vector_complex const& Y, matrix_complex& A ){
      return gsl_blas_zher2( Uplo, alpha.get(), X.get(), Y.get(), A.get() ); }

    /**
     * C++ version of gsl_blas_sgemm().
     * @param TransA Transpose type
     * @param TransB Transpose type for B
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int sgemm( CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB, float alpha,
	       matrix_float const& A, matrix_float const& B, float beta, matrix_float& C ){
      return gsl_blas_sgemm( TransA, TransB, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_ssymm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int ssymm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, float alpha,
	       matrix_float const& A, matrix_float const& B, float beta, matrix_float& C ){
      return gsl_blas_ssymm( Side, Uplo, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_ssyrk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int ssyrk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
	       matrix_float const& A, float beta, matrix_float& C ){
      return gsl_blas_ssyrk( Uplo, Trans, alpha, A.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_ssyr2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int ssyr2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
		matrix_float const& A, matrix_float const& B, float beta, matrix_float& C ){
      return gsl_blas_ssyr2k( Uplo, Trans, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_strmm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int strmm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, float alpha, matrix_float const& A, matrix_float& B ){
      return gsl_blas_strmm( Side, Uplo, TransA, Diag, alpha, A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_strsm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int strsm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, float alpha, matrix_float const& A, matrix_float& B ){
      return gsl_blas_strsm( Side, Uplo, TransA, Diag, alpha, A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_dgemm().
     * @param TransA Transpose type
     * @param TransB Transpose type for B
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int dgemm( CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB, double alpha,
	       matrix const& A, matrix const& B, double beta, matrix& C ){
      return gsl_blas_dgemm( TransA, TransB, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_dsymm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int dsymm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, double alpha,
	       matrix const& A, matrix const& B, double beta, matrix& C ){
      return gsl_blas_dsymm( Side, Uplo, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_dsyrk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int dsyrk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
	       matrix const& A, double beta, matrix& C ){
      return gsl_blas_dsyrk( Uplo, Trans, alpha, A.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_dsyr2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int dsyr2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
		matrix const& A, matrix const& B, double beta, matrix& C ){
      return gsl_blas_dsyr2k( Uplo, Trans, alpha, A.get(), B.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_dtrmm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int dtrmm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, double alpha, matrix const& A, matrix& B ){
      return gsl_blas_dtrmm( Side, Uplo, TransA, Diag, alpha, A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_dtrsm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int dtrsm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, double alpha, matrix const& A, matrix& B ){
      return gsl_blas_dtrsm( Side, Uplo, TransA, Diag, alpha, A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_cgemm().
     * @param TransA Transpose type
     * @param TransB Transpose type for B
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int cgemm( CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB,
	       complex_float const& alpha, matrix_complex_float const& A,
	       matrix_complex_float const& B, complex_float const& beta, matrix_complex_float& C ){
      return gsl_blas_cgemm( TransA, TransB, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_csymm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int csymm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, complex_float const& alpha,
	       matrix_complex_float const& A, matrix_complex_float const& B,
	       complex_float const& beta, matrix_complex_float& C ){
      return gsl_blas_csymm( Side, Uplo, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_csyrk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int csyrk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex_float const& alpha,
	       matrix_complex_float const& A, complex_float const& beta, matrix_complex_float& C ){
      return gsl_blas_csyrk( Uplo, Trans, alpha.get(), A.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_csyr2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int csyr2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex_float const& alpha,
		matrix_complex_float const& A, matrix_complex_float const& B,
		complex_float const& beta, matrix_complex_float& C ){
      return gsl_blas_csyr2k( Uplo, Trans, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_ctrmm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int ctrmm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, complex_float const& alpha, matrix_complex_float const& A,
	       matrix_complex_float& B ){
      return gsl_blas_ctrmm( Side, Uplo, TransA, Diag, alpha.get(), A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_ctrsm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int ctrsm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, complex_float const& alpha, matrix_complex_float const& A,
	       matrix_complex_float& B ){
      return gsl_blas_ctrsm( Side, Uplo, TransA, Diag, alpha.get(), A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_zgemm().
     * @param TransA Transpose type
     * @param TransB Transpose type for B
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zgemm( CBLAS_TRANSPOSE_t TransA, CBLAS_TRANSPOSE_t TransB, complex const& alpha,
	       matrix_complex const& A, matrix_complex const& B, complex const& beta,
	       matrix_complex& C ){
          return gsl_blas_zgemm( TransA, TransB, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_zsymm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zsymm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, complex const& alpha,
	       matrix_complex const& A, matrix_complex const& B, complex const& beta,
	       matrix_complex& C ){
      return gsl_blas_zsymm( Side, Uplo, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_zsyrk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zsyrk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex const& alpha,
	       matrix_complex const& A, complex const& beta, matrix_complex& C ){
      return gsl_blas_zsyrk( Uplo, Trans, alpha.get(), A.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_zsyr2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zsyr2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex const& alpha,
		matrix_complex const& A, matrix_complex const& B, complex const& beta,
		matrix_complex& C ){
          return gsl_blas_zsyr2k( Uplo, Trans, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_ztrmm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int ztrmm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, complex const& alpha, matrix_complex const& A,
	       matrix_complex& B ){
      return gsl_blas_ztrmm( Side, Uplo, TransA, Diag, alpha.get(), A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_ztrsm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param TransA Transpose type
     * @param Diag Diagonal type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @return Error code on failure
     */
    inline int ztrsm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t TransA,
	       CBLAS_DIAG_t Diag, complex const& alpha, matrix_complex const& A,
	       matrix_complex& B ){
          return gsl_blas_ztrsm( Side, Uplo, TransA, Diag, alpha.get(), A.get(), B.get() ); }

    /**
     * C++ version of gsl_blas_chemm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int chemm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, complex_float const& alpha,
	       matrix_complex_float const& A, matrix_complex_float const& B,
	       complex_float const& beta, matrix_complex_float& C ){
      return gsl_blas_chemm( Side, Uplo, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_cherk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int cherk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, float alpha,
	       matrix_complex_float const& A, float beta, matrix_complex_float& C ){
      return gsl_blas_cherk( Uplo, Trans, alpha, A.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_cher2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int cher2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex_float const& alpha,
		matrix_complex_float const& A, matrix_complex_float const& B, float beta,
		matrix_complex_float& C ){
      return gsl_blas_cher2k( Uplo, Trans, alpha.get(), A.get(), B.get(), beta, C.get() ); }
    
    /**
     * C++ version of gsl_blas_zhemm().
     * @param Side Side to apply operation to
     * @param Uplo Upper or lower triangular
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zhemm( CBLAS_SIDE_t Side, CBLAS_UPLO_t Uplo, complex const& alpha,
	       matrix_complex const& A, matrix_complex const& B, complex const& beta,
	       matrix_complex& C ){
      return gsl_blas_zhemm( Side, Uplo, alpha.get(), A.get(), B.get(), beta.get(), C.get() ); }

    /**
     * C++ version of gsl_blas_zherk().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zherk( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, double alpha,
	       matrix_complex const& A, double beta, matrix_complex& C ){
      return gsl_blas_zherk( Uplo, Trans, alpha, A.get(), beta, C.get() ); }

    /**
     * C++ version of gsl_blas_zher2k().
     * @param Uplo Upper or lower triangular
     * @param Trans Transpose type
     * @param alpha A constant
     * @param A A matrix
     * @param B Another matrix
     * @param beta Another constant
     * @param C Another matrix
     * @return Error code on failure
     */
    inline int zher2k( CBLAS_UPLO_t Uplo, CBLAS_TRANSPOSE_t Trans, complex const& alpha,
		matrix_complex const& A, matrix_complex const& B, double beta,
		matrix_complex& C ){
          return gsl_blas_zher2k( Uplo, Trans, alpha.get(), A.get(), B.get(), beta, C.get() ); }

  }

}
#endif
