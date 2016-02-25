/*
 * $Id: statistics_uchar.hpp 303 2013-10-28 07:48:23Z jdl3 $
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

#ifndef CCGSL_STATISTICS_UCHAR_HPP
#define CCGSL_STATISTICS_UCHAR_HPP
#include<gsl/gsl_statistics_uchar.h>

namespace gsl {
  namespace stats {
    /**
     * C++ version of gsl_stats_uchar_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample mean
     */
    inline double uchar_mean( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_mean( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample variance
     */
    inline double uchar_variance( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_variance( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample standard deviation
     */
    inline double uchar_sd( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_sd( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_with_fixed_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The maximum-likelihood sample variance wih given population mean
     */
    inline double uchar_variance_with_fixed_mean( unsigned char const data[], size_t const stride,
						  size_t const n, double const mean ){
      return gsl_stats_uchar_variance_with_fixed_mean( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_with_fixed_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The maximum-likelihood sample standard deviation wih given population mean
     */
    inline double uchar_sd_with_fixed_mean( unsigned char const data[], size_t const stride,
					    size_t const n, double const mean ){
      return gsl_stats_uchar_sd_with_fixed_mean( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_tss().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample total sum of squares
     */
    inline double uchar_tss( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_tss( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_tss_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The sample total sum of squares with given sample mean
     */
    inline double uchar_tss_m( unsigned char const data[], size_t const stride,
			       size_t const n, double const mean ){
      return gsl_stats_uchar_tss_m( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample absolute deviation
     */
    inline double uchar_absdev( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_absdev( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_skew().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample skewness
     */
    inline double uchar_skew( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_skew( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample kurtosis
     */
    inline double uchar_kurtosis( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_kurtosis( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The sample lag 1 autocorrelation
     */
    inline double uchar_lag1_autocorrelation( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_lag1_autocorrelation( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n The data array size
     * @return The sample covariance
     */
    inline double uchar_covariance( unsigned char const data1[], size_t const stride1,
				    unsigned char const data2[], size_t const stride2,
				    size_t const n ){
      return gsl_stats_uchar_covariance( data1, stride1, data2, stride2, n ); } 
    /**
     * C++ version of gsl_stats_uchar_correlation().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n The data array size
     * @return The sample correlation
     */
    inline double uchar_correlation( unsigned char const data1[], size_t const stride1, unsigned char const data2[],
				     size_t const stride2, size_t const n ){
      return gsl_stats_uchar_correlation( data1, stride1, data2, stride2, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The sample variance with given mean value
     */
    inline double uchar_variance_m( unsigned char const data[], size_t const stride, size_t const n,
				    double const mean ){
      return gsl_stats_uchar_variance_m( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The sample standard deviation with given mean value
     */
    inline double uchar_sd_m( unsigned char const data[], size_t const stride, size_t const n,
			      double const mean ){
      return gsl_stats_uchar_sd_m( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The sample absolute deviation from the given mean value
     */
    inline double uchar_absdev_m( unsigned char const data[], size_t const stride, size_t const n,
				  double const mean ){
      return gsl_stats_uchar_absdev_m( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_skew_m_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample skewness (mean and standard deviation supplied)
     */
    inline double uchar_skew_m_sd( unsigned char const data[], size_t const stride, size_t const n,
				   double const mean, double const sd ){
      return gsl_stats_uchar_skew_m_sd( data, stride, n, mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis_m_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample kurtosis (mean and standard deviation supplied)
     */
    inline double uchar_kurtosis_m_sd( unsigned char const data[], size_t const stride, size_t const n,
				       double const mean, double const sd ){
      return gsl_stats_uchar_kurtosis_m_sd( data, stride, n, mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param mean The mean value
     * @return The sample autocorrelation using the given mean value
     */
    inline double uchar_lag1_autocorrelation_m( unsigned char const data[], size_t const stride,
						size_t const n, double const mean ){
      return gsl_stats_uchar_lag1_autocorrelation_m( data, stride, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance_m().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n The data array size
     * @param mean1 The first mean value
     * @param mean2 The second mean value
     * @return The sample covariance using the given mean value
     */
    inline double uchar_covariance_m( unsigned char const data1[], size_t const stride1,
				      unsigned char const data2[], size_t const stride2,
				      size_t const n, double const mean1, double const mean2 ){
      return gsl_stats_uchar_covariance_m( data1, stride1, data2, stride2, n, mean1, mean2 ); } 
    /**
     * C++ version of gsl_stats_uchar_pvariance().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param n1 The size of the first data array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n2 The sixe of the second data array
     * @return The pooled variance of the data sets
     */
    inline double uchar_pvariance( unsigned char const data1[], size_t const stride1, size_t const n1,
				   unsigned char const data2[], size_t const stride2, size_t const n2 ){
      return gsl_stats_uchar_pvariance( data1, stride1, n1, data2, stride2, n2 ); } 
    /**
     * C++ version of gsl_stats_uchar_ttest().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param n1 The size of the first data array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n2 The sixe of the second data array
     * @return The @em t statistic for an independent samples @em t test of no
     * difference in population means
     */
    inline double uchar_ttest( unsigned char const data1[], size_t const stride1, size_t const n1,
			       unsigned char const data2[], size_t const stride2, size_t const n2 ){
      return gsl_stats_uchar_ttest( data1, stride1, n1, data2, stride2, n2 ); } 
    /**
     * C++ version of gsl_stats_uchar_max().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The maximum value in the data set
     */
    inline double uchar_max( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_max( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_min().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The minimum value in the data set
     */
    inline double uchar_min( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_min( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax().
     * @param min The minimum of the data (return value)
     * @param max The maximum of the data (return value)
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     */
    inline void uchar_minmax( unsigned char* min, unsigned char* max, unsigned char const data[], size_t const stride,
			      size_t const n ){
      gsl_stats_uchar_minmax( min, max, data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_max_index().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The index of the maximum value in the data set
     */
    inline size_t uchar_max_index( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_max_index( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_min_index().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The index of the minimum value in the data set
     */
    inline size_t uchar_min_index( unsigned char const data[], size_t const stride, size_t const n ){
      return gsl_stats_uchar_min_index( data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax_index().
     * @param min_index The index of the minimum of the data (return value)
     * @param max_index The index of the maximum of the data (return value)
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     */
    inline void uchar_minmax_index( size_t* min_index, size_t* max_index, unsigned char const data[],
				    size_t const stride, size_t const n ){
      gsl_stats_uchar_minmax_index( min_index, max_index, data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_median_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @return The median
     */
    inline double uchar_median_from_sorted_data( unsigned char const sorted_data[],
						 size_t const stride, size_t const n ){
      return gsl_stats_uchar_median_from_sorted_data( sorted_data, stride, n ); } 
    /**
     * C++ version of gsl_stats_uchar_quantile_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param stride The number of steps between consecutive data values 
     * @param n The data array size
     * @param f The proportion for which to estimate a quantile
     * @return The \$f\$-quantile
     */
    inline double uchar_quantile_from_sorted_data( unsigned char const sorted_data[], size_t const stride,
						   size_t const n, double const f ){
      return gsl_stats_uchar_quantile_from_sorted_data( sorted_data, stride, n, f ); }
    
    // Same functions but without stride parameters
    
    /**
     * C++ version of gsl_stats_uchar_mean().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample mean
     */
    inline double uchar_mean( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_mean( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample variance
     */
    inline double uchar_variance( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_variance( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_sd().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample standard deviation
     */
    inline double uchar_sd( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_sd( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_with_fixed_mean().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The maximum-likelihood sample variance wih given population mean
     */
    inline double uchar_variance_with_fixed_mean( unsigned char const data[],
						  size_t const n, double const mean ){
      return gsl_stats_uchar_variance_with_fixed_mean( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_with_fixed_mean().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The maximum-likelihood sample standard deviation wih given population mean
     */
    inline double uchar_sd_with_fixed_mean( unsigned char const data[],
					    size_t const n, double const mean ){
      return gsl_stats_uchar_sd_with_fixed_mean( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_tss().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample total sum of squares
     */
    inline double uchar_tss( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_tss( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_tss_m().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The sample total sum of squares with given sample mean
     */
    inline double uchar_tss_m( unsigned char const data[],
			       size_t const n, double const mean ){
      return gsl_stats_uchar_tss_m( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample absolute deviation
     */
    inline double uchar_absdev( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_absdev( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_skew().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample skewness
     */
    inline double uchar_skew( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_skew( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample kurtosis
     */
    inline double uchar_kurtosis( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_kurtosis( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation().
     * @param data An array of data values
     * @param n The data array size
     * @return The sample lag 1 autocorrelation
     */
    inline double uchar_lag1_autocorrelation( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_lag1_autocorrelation( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param n The data array size
     * @return The sample covariance
     */
    inline double uchar_covariance( unsigned char const data1[], unsigned char const data2[], size_t const n ){
      return gsl_stats_uchar_covariance( data1, 1, data2, 1, n ); }   
    /**
     * C++ version of gsl_stats_uchar_correlation().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param n The data array size
     * @return The sample correlation
     */
    inline double uchar_correlation( unsigned char const data1[], unsigned char const data2[], size_t const n ){
      return gsl_stats_uchar_correlation( data1, 1, data2, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_m().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The sample variance with given mean value
     */
    inline double uchar_variance_m( unsigned char const data[], size_t const n, double const mean ){
      return gsl_stats_uchar_variance_m( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_m().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The sample standard deviation with given mean value
     */
    inline double uchar_sd_m( unsigned char const data[], size_t const n, double const mean ){
      return gsl_stats_uchar_sd_m( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev_m().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The sample absolute deviation from the given mean value
     */
    inline double uchar_absdev_m( unsigned char const data[], size_t const n, double const mean ){
      return gsl_stats_uchar_absdev_m( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_skew_m_sd().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample skewness (mean and standard deviation supplied)
     */
    inline double uchar_skew_m_sd( unsigned char const data[], size_t const n,
				   double const mean, double const sd ){
      return gsl_stats_uchar_skew_m_sd( data, 1, n, mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis_m_sd().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample kurtosis (mean and standard deviation supplied)
     */
    inline double uchar_kurtosis_m_sd( unsigned char const data[], size_t const n,
				       double const mean, double const sd ){
      return gsl_stats_uchar_kurtosis_m_sd( data, 1, n, mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation_m().
     * @param data An array of data values
     * @param n The data array size
     * @param mean The mean value
     * @return The sample autocorrelation using the given mean value
     */
    inline double uchar_lag1_autocorrelation_m( unsigned char const data[],
						size_t const n, double const mean ){
      return gsl_stats_uchar_lag1_autocorrelation_m( data, 1, n, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance_m().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param n The data array size
     * @param mean1 The first mean value
     * @param mean2 The second mean value
     * @return The sample covariance using the given mean value
     */
    inline double uchar_covariance_m( unsigned char const data1[], unsigned char const data2[],
				      size_t const n, double const mean1, double const mean2 ){
      return gsl_stats_uchar_covariance_m( data1, 1, data2, 1, n, mean1, mean2 ); } 
    /**
     * C++ version of gsl_stats_uchar_pvariance().
     * @param data1 The first array of data values
     * @param n1 The size of the first data array
     * @param data2 The second array of data values
     * @param n2 The sixe of the second data array
     * @return The pooled variance of the data sets
     */
    inline double uchar_pvariance( unsigned char const data1[], size_t const n1,
				   unsigned char const data2[], size_t const n2 ){
      return gsl_stats_uchar_pvariance( data1, 1, n1, data2, 1, n2 ); } 
    /**
     * C++ version of gsl_stats_uchar_ttest().
     * @param data1 The first array of data values
     * @param n1 The size of the first data array
     * @param data2 The second array of data values
     * @param n2 The sixe of the second data array
     * @return The @em t statistic for an independent samples @em t test of no
     * difference in population means
     */
    inline double uchar_ttest( unsigned char const data1[], size_t const n1,
			       unsigned char const data2[], size_t const n2 ){
      return gsl_stats_uchar_ttest( data1, 1, n1, data2, 1, n2 ); } 
    /**
     * C++ version of gsl_stats_uchar_max().
     * @param data An array of data values
     * @param n The data array size
     * @return The maximum value in the data set
     */
    inline double uchar_max( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_max( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_min().
     * @param data An array of data values
     * @param n The data array size
     * @return The minimum value in the data set
     */
    inline double uchar_min( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_min( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax().
     * @param min The minimum of the data (return value)
     * @param max The maximum of the data (return value)
     * @param data An array of data values
     * @param n The data array size
     */
    inline void uchar_minmax( unsigned char* min, unsigned char* max, unsigned char const data[],
			      size_t const n ){
      gsl_stats_uchar_minmax( min, max, data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_max_index().
     * @param data An array of data values
     * @param n The data array size
     * @return The index of the maximum value in the data set
     */
    inline size_t uchar_max_index( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_max_index( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_min_index().
     * @param data An array of data values
     * @param n The data array size
     * @return The index of the minimum value in the data set
     */
    inline size_t uchar_min_index( unsigned char const data[], size_t const n ){
      return gsl_stats_uchar_min_index( data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax_index().
     * @param min_index The index of the minimum of the data (return value)
     * @param max_index The index of the maximum of the data (return value)
     * @param data An array of data values
     * @param n The data array size
     */
    inline void uchar_minmax_index( size_t* min_index, size_t* max_index, unsigned char const data[],
				    size_t const n ){
      gsl_stats_uchar_minmax_index( min_index, max_index, data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_median_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param n The data array size
     * @return The median
     */
    inline double uchar_median_from_sorted_data( unsigned char const sorted_data[],
						 size_t const n ){
      return gsl_stats_uchar_median_from_sorted_data( sorted_data, 1, n ); } 
    /**
     * C++ version of gsl_stats_uchar_quantile_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param n The data array size
     * @param f The proportion for which to estimate a quantile
     * @return The \$f\$-quantile
     */
    inline double uchar_quantile_from_sorted_data( unsigned char const sorted_data[],
						   size_t const n, double const f ){
      return gsl_stats_uchar_quantile_from_sorted_data( sorted_data, 1, n, f ); }
    
    // Generic versions of the same functions

    /**
     * C++ version of gsl_stats_uchar_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample mean
     */
    template<typename T>
    inline double uchar_mean( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_mean( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_variance().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample variance
     */
    template<typename T>
    inline double uchar_variance( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_variance( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample standard deviation
     */
    template<typename T>
    inline double uchar_sd( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_sd( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_with_fixed_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The maximum-likelihood sample variance wih given population mean
     */
    template<typename T>
    inline double uchar_variance_with_fixed_mean( T const& data, size_t const stride,
						  double const mean ){
      return gsl_stats_uchar_variance_with_fixed_mean( data.data(), stride, data.size() / stride, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_with_fixed_mean().
     * @param data An array of data values
     * @param mean The mean value
     * @return The maximum-likelihood sample variance wih given population mean
     */
    template<typename T>
    inline double uchar_variance_with_fixed_mean( T const& data, double const mean ){
      return gsl_stats_uchar_variance_with_fixed_mean( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_with_fixed_mean().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The maximum-likelihood sample standard deviation wih given population mean
     */
    template<typename T>
    inline double uchar_sd_with_fixed_mean( T const& data, size_t const stride,
					    double const mean ){
      return gsl_stats_uchar_sd_with_fixed_mean( data.data(), stride, data.size() / stride, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_with_fixed_mean().
     * @param data An array of data values
     * @param mean The mean value
     * @return The maximum-likelihood sample standard deviation wih given population mean
     */
    template<typename T>
    inline double uchar_sd_with_fixed_mean( T const& data, double const mean ){
      return gsl_stats_uchar_sd_with_fixed_mean( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_tss().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample total sum of squares
     */
    template<typename T>
    inline double uchar_tss( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_tss( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_tss_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The sample total sum of squares with given sample mean
     */
    template<typename T>
    inline double uchar_tss_m( T const& data, size_t const stride, double const mean ){
      return gsl_stats_uchar_tss_m( data.data(), stride, data.size() / stride, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_tss_m().
     * @param data An array of data values
     * @param mean The mean value
     * @return The sample total sum of squares with given sample mean
     */
    template<typename T>
    inline double uchar_tss_m( T const& data, double const mean ){
      return gsl_stats_uchar_tss_m( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample absolute deviation
     */
    template<typename T>
    inline double uchar_absdev( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_absdev( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_skew().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample skewness
     */
    template<typename T>
    inline double uchar_skew( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_skew( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample kurtosis
     */
    template<typename T>
    inline double uchar_kurtosis( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_kurtosis( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The sample lag 1 autocorrelation
     */
    template<typename T>
    inline double uchar_lag1_autocorrelation( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_lag1_autocorrelation( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @return The sample covariance
     */
    template<typename T, typename U>
    inline double uchar_covariance( T const& data1, size_t const stride1,
				    U const& data2, size_t const stride2 ){
      return gsl_stats_uchar_covariance( data1.data(), stride1, data2.data(), stride2, data1.size() / stride1 ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @return The sample covariance
     */
    template<typename T, typename U>
    inline double uchar_covariance( T const& data1, U const& data2 ){
      return gsl_stats_uchar_covariance( data1.data(), 1, data2.data(), 1, data1.size() ); } 
    /**
     * C++ version of gsl_stats_uchar_correlation().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @return The sample correlation
     */
    template<typename T, typename U>
    inline double uchar_correlation( T const& data1, size_t const stride1, U const& data2,
				     size_t const stride2 ){
      return gsl_stats_uchar_correlation( data1.data(), stride1, data2.data(), stride2, data1.size() / stride1 ); }
    /**
     * C++ version of gsl_stats_uchar_correlation().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @return The sample correlation
     */
    template<typename T, typename U>
    inline double uchar_correlation( T const& data1, U const& data2 ){
      return gsl_stats_uchar_correlation( data1.data(), 1, data2.data(), 1, data1.size() ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The sample variance with given mean value
     */
    template<typename T>
    inline double uchar_variance_m( T const& data, size_t const stride, double const mean ){
      return gsl_stats_uchar_variance_m( data.data(), stride, data.size() / stride, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_variance_m().
     * @param data An array of data values
     * @param mean The mean value
     * @return The sample variance with given mean value
     */
    template<typename T>
    inline double uchar_variance_m( T const& data, double const mean ){
      return gsl_stats_uchar_variance_m( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The sample standard deviation with given mean value
     */
    template<typename T>
    inline double uchar_sd_m( T const& data, size_t const stride, double const mean ){
      return gsl_stats_uchar_sd_m( data.data(), stride, data.size() / stride, mean ); } 
    /**
     * C++ version of gsl_stats_uchar_sd_m().
     * @param data An array of data values
     * @param mean The mean value
     * @return The sample standard deviation with given mean value
     */
    template<typename T>
    inline double uchar_sd_m( T const& data, double const mean ){
      return gsl_stats_uchar_sd_m( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_absdev_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The sample absolute deviation from the given mean value
     */
    template<typename T>
    inline double uchar_absdev_m( T const& data, size_t const stride, double const mean ){
      return gsl_stats_uchar_absdev_m( data.data(), stride, data.size() / stride, mean ); }  
    /**
     * C++ version of gsl_stats_uchar_absdev_m().
     * @param data An array of data values
     * @param mean The mean value
     * @return The sample absolute deviation from the given mean value
     */
    template<typename T>
    inline double uchar_absdev_m( T const& data, double const mean ){
      return gsl_stats_uchar_absdev_m( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_skew_m_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample skewness (mean and standard deviation supplied)
     */
    template<typename T>
    inline double uchar_skew_m_sd( T const& data, size_t const stride, 
				   double const mean, double const sd ){
      return gsl_stats_uchar_skew_m_sd( data.data(), stride, data.size() / stride, mean, sd ); }  
    /**
     * C++ version of gsl_stats_uchar_skew_m_sd().
     * @param data An array of data values
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample skewness (mean and standard deviation supplied)
     */
    template<typename T>
    inline double uchar_skew_m_sd( T const& data, double const mean, double const sd ){
      return gsl_stats_uchar_skew_m_sd( data.data(), 1, data.size(), mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_kurtosis_m_sd().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample kurtosis (mean and standard deviation supplied)
     */
    template<typename T>
    inline double uchar_kurtosis_m_sd( T const& data, size_t const stride, 
				       double const mean, double const sd ){
      return gsl_stats_uchar_kurtosis_m_sd( data.data(), stride, data.size() / stride, mean, sd ); }  
    /**
     * C++ version of gsl_stats_uchar_kurtosis_m_sd().
     * @param data An array of data values
     * @param mean The mean value
     * @param sd The standard deviation
     * @return The sample kurtosis (mean and standard deviation supplied)
     */
    template<typename T>
    inline double uchar_kurtosis_m_sd( T const& data, double const mean, double const sd ){
      return gsl_stats_uchar_kurtosis_m_sd( data.data(), 1, data.size(), mean, sd ); } 
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation_m().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @param mean The mean value
     * @return The sample autocorrelation using the given mean value
     */
    template<typename T>
    inline double uchar_lag1_autocorrelation_m( T const& data, size_t const stride,
						double const mean ){
      return gsl_stats_uchar_lag1_autocorrelation_m( data.data(), stride, data.size() / stride, mean ); }  
    /**
     * C++ version of gsl_stats_uchar_lag1_autocorrelation_m().
     * @param data An array of data values
     * @param mean The mean value
     * @return The sample autocorrelation using the given mean value
     */
    template<typename T>
    inline double uchar_lag1_autocorrelation_m( T const& data, double const mean ){
      return gsl_stats_uchar_lag1_autocorrelation_m( data.data(), 1, data.size(), mean ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance_m().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param mean1 The first mean value
     * @param mean2 The second mean value
     * @return The sample covariance using the given mean value
     */
    template<typename T, typename U>
    inline double uchar_covariance_m( T const& data1, size_t const stride1,
				      U const& data2, size_t const stride2,
				      double const mean1, double const mean2 ){
      return gsl_stats_uchar_covariance_m( data1.data(), stride1, data2.data(), stride2, data1.size() / stride1,
					   mean1, mean2 ); } 
    /**
     * C++ version of gsl_stats_uchar_covariance_m().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param mean1 The first mean value
     * @param mean2 The second mean value
     * @return The sample covariance using the given mean value
     */
    template<typename T>
    inline double uchar_covariance_m( T const& data1, T const& data2,
				      double const mean1, double const mean2 ){
      return gsl_stats_uchar_covariance_m( data1.data(), 1, data2.data(), 1, data1.size(),
					   mean1, mean2 ); } 
    /**
     * C++ version of gsl_stats_uchar_pvariance().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @return The pooled variance of the data sets
     */
    template<typename T, typename U>
    inline double uchar_pvariance( T const& data1, size_t const stride1,
				   U const& data2, size_t const stride2 ){
      return gsl_stats_uchar_pvariance( data1.data(), stride1, data1.size() / stride1, data2.data(),
					stride2, data2.size() / stride2 ); } 
    /**
     * C++ version of gsl_stats_uchar_ttest().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @return The @em t statistic for an independent samples @em t test of no
     * difference in population means
     */
    template<typename T, typename U>
    inline double uchar_ttest( T const& data1, size_t const stride1,
			       U const& data2, size_t const stride2 ){
      return gsl_stats_uchar_ttest( data1.data(), stride1, data1.size() / stride1,
				    data2.data(), stride2, data2.size() / stride2 ); } 
    /**
     * C++ version of gsl_stats_uchar_max().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The maximum value in the data set
     */
    template<typename T>
    inline double uchar_max( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_max( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_min().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The minimum value in the data set
     */
    template<typename T>
    inline double uchar_min( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_min( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax().
     * @param min The minimum of the data (return value)
     * @param max The maximum of the data (return value)
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     */
    template<typename T>
    inline void uchar_minmax( unsigned char* min, unsigned char* max, T const& data, size_t const stride = 1 ){
      gsl_stats_uchar_minmax( min, max, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_max_index().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The index of the maximum value in the data set
     */
    template<typename T>
    inline size_t uchar_max_index( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_max_index( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_min_index().
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     * @return The index of the minimum value in the data set
     */
    template<typename T>
    inline size_t uchar_min_index( T const& data, size_t const stride = 1 ){
      return gsl_stats_uchar_min_index( data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_minmax_index().
     * @param min_index The index of the minimum of the data (return value)
     * @param max_index The index of the maximum of the data (return value)
     * @param data An array of data values
     * @param stride The number of steps between consecutive data values 
     */
    template<typename T>
    inline void uchar_minmax_index( size_t* min_index, size_t* max_index, T const& data,
				    size_t const stride = 1 ){
      gsl_stats_uchar_minmax_index( min_index, max_index, data.data(), stride, data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_median_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param stride The number of steps between consecutive data values 
     * @return The median
     */
    template<typename T>
    inline double uchar_median_from_sorted_data( T const& sorted_data, size_t const stride = 1 ){
      return gsl_stats_uchar_median_from_sorted_data( sorted_data.data(), stride, sorted_data.size() / stride ); } 
    /**
     * C++ version of gsl_stats_uchar_quantile_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param stride The number of steps between consecutive data values 
     * @param f The proportion for which to estimate a quantile
     * @return The \$f\$-quantile
     */
    template<typename T>
    inline double uchar_quantile_from_sorted_data( T const& sorted_data, size_t const stride,
						   double const f ){
      return gsl_stats_uchar_quantile_from_sorted_data( sorted_data.data(), stride,
							sorted_data.size() / stride, f ); }
    /* stride-free versions */
    /**
     * C++ version of gsl_stats_uchar_pvariance().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @return The pooled variance of the data sets
     */
    template<typename T, typename U>
    inline double uchar_pvariance( T const& data1, U const& data2 ){
      return gsl_stats_uchar_pvariance( data1.data(), 1, data1.size(), data2.data(), 1, data1.size() ); } 
    /**
     * C++ version of gsl_stats_uchar_ttest().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @return The @em t statistic for an independent samples @em t test of no
     * difference in population means
     */
    template<typename T, typename U>
    inline double uchar_ttest( T const& data1, U const& data2 ){
      return gsl_stats_uchar_ttest( data1.data(), 1, data1.size(), data2.data(), 1, data1.size() ); } 
    /**
     * C++ version of gsl_stats_uchar_quantile_from_sorted_data().
     * @param sorted_data A sorted data array
     * @param f The proportion for which to estimate a quantile
     * @return The \$f\$-quantile
     */
    template<typename T>
    inline double uchar_quantile_from_sorted_data( T const& sorted_data, double const f ){
      return gsl_stats_uchar_quantile_from_sorted_data( sorted_data.data(), 1, sorted_data.size(), f ); }
    /**
     * C++ version of gsl_stats_uchar_spearman().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param n The data array size
     * @param work A workspace. This should be of size at least \(2n\) or null.
     * A non-null value for work is more effficient if more than one Spearman correlation is
     * needed
     * @return The Spearman correlation
     */
    inline double uchar_spearman( unsigned char const data1[], size_t const stride1,
				 unsigned char const data2[], size_t const stride2,
				 size_t const n, double work[] ){
      if( 0 != work )
	return gsl_stats_uchar_spearman( data1, stride1, data2, stride2, n, work );
      // create workspace
      double* workspace = new double[2 * n];
      double result = gsl_stats_uchar_spearman( data1, stride1, data2, stride2, n, workspace );
      delete[] workspace;
      return result;
    }
    /**
     * C++ version of gsl_stats_uchar_spearman().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param n The data array size
     * @param work A workspace. This should be of size at least \(2n\) or null.
     * A non-null value for work is more effficient if more than one Spearman correlation is
     * needed
     * @return The Spearman correlation
     */
    inline double uchar_spearman( unsigned char const data1[], unsigned char const data2[],
				 size_t const n, double work[] ){
      if( 0 != work )
	return gsl_stats_uchar_spearman( data1, 1, data2, 1, n, work );
      // create workspace
      double* workspace = new double[2 * n];
      double result = gsl_stats_uchar_spearman( data1, 1, data2, 1, n, workspace );
      delete[] workspace;
      return result;
    }
    /**
     * C++ version of gsl_stats_uchar_spearman().
     * @param data1 The first array of data values
     * @param stride1 The number of steps between data values in the first array
     * @param data2 The second array of data values
     * @param stride2 The number of steps between data values in the second array
     * @param work A workspace. This should be of size at least \(2n\) or null.
     * A non-null value for work is more effficient if more than one Spearman correlation is
     * needed
     * @return The Spearman correlation
     */
    template<typename T, typename U>
    inline double uchar_spearman( T const& data1, size_t const stride1,
				 U const& data2, size_t const stride2,
				 double work[] ){
      if( 0 != work )
	return gsl_stats_uchar_spearman( data1, stride1, data2, stride2,
					data1.size() / stride1, work );
      // create workspace
      double* workspace = new double[2 * data1.size() / stride1];
      double result = gsl_stats_uchar_spearman( data1, stride1, data2, stride2,
					       data1.size() / stride1, workspace );
      delete[] workspace;
      return result;
    }
    /**
     * C++ version of gsl_stats_uchar_spearman().
     * @param data1 The first array of data values
     * @param data2 The second array of data values
     * @param work A workspace. This should be of size at least \(2n\) or null.
     * A non-null value for work is more effficient if more than one Spearman correlation is
     * needed
     * @return The Spearman correlation
     */
    template<typename T, typename U>
    inline double uchar_spearman( T const& data1, U const& data2, double work[] ){
      if( 0 != work )
	return gsl_stats_uchar_spearman( data1, 1, data2, 1, data1.size(), work );
      // create workspace
      double* workspace = new double[2 * data1.size()];
      double result = gsl_stats_uchar_spearman( data1, 1, data2, 1, data1.size(), workspace );
      delete[] workspace;
      return result;
    }

  }
}
#endif
