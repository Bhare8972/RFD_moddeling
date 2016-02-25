/*
 * $Id: histogram2d.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_HISTOGRAM2D_HPP
#define CCGSL_HISTOGRAM2D_HPP

#include<new>
#include<gsl/gsl_histogram2d.h>
#include"exception.hpp"

namespace gsl {
  /**
   * 2D histograms.
   */
  class histogram2d {
  public:
    /**
     * The default constructor is only really useful for assigning to.
     */
    histogram2d(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new histogram2d with n elements.
     * @param nx The number of bin slices in x direction
     * @param ny The number of bin slices in y direction
     */
    explicit histogram2d( size_t const nx, size_t const ny ){
      ccgsl_pointer = gsl_histogram2d_alloc( nx, ny );
      // just plausibly we could allocate histogram2d but not count
      try { count = new size_t; } catch( std::bad_alloc& e ){
	// try to tidy up before rethrowing
	gsl_histogram2d_free( ccgsl_pointer );
	throw e;
      }
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    /**
     * Could construct from a gsl_histogram2d.
     * This is not usually a good idea. In this case
     * we should not use gsl_histogram2d_free() to deallocate the memory.
     * @param v The histogram2d
     */
    explicit histogram2d( gsl_histogram2d* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The histogram2d to copy.
     */
    histogram2d( histogram2d const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The histogram2d to copy
     */
    histogram2d& operator=( histogram2d const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_histogram2d_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~histogram2d(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_histogram2d_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The histogram2d to move.
     */
    histogram2d( histogram2d&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The histogram2d to move.
     * @return A reference to this.
     */
    histogram2d& operator=( histogram2d&& v ){
      histogram2d( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two histogram2d are identically equal if their elements are identical.
     * @param v The histogram2d to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( histogram2d const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two histogram2d are different if their elements are not identical.
     * @param v The histogram2d to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( histogram2d const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram2d is nonnegative.
     * @param v The histogram2d to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( histogram2d const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram2d is nonnegative.
     * @param v The histogram2d to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( histogram2d const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram2d is nonnegative.
     * @param v The histogram2d to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( histogram2d const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram2d is nonnegative.
     * @param v The histogram2d to be compared with @c this
     * @return @c false or @c true according as @c this is no 
     * less than @c v lexicographically
     */
    bool operator>=( histogram2d const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the histogram2d is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two histogram2d objects. This works even if the histogram2d have different sizes
     * because it swaps pointers.
     * @param v The histogram2d to swap with @c this.
     */
    void swap( histogram2d& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_histogram2d* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_histogram2d.
     * @return the gsl_histogram2d
     */
    gsl_histogram2d* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_histogram2d.
     * @return @c true or @c falses according as 
     * this is the only histogram2d object sharing the gsl_histogram2d.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many histogram2d objects share this pointer.
     * @return the number of histogram2d objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_histogram2d.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * C++ version of gsl_histogram2d_calloc().
     * @param nx Number of bin slices in x direction
     * @param ny Number of bin slices in y direction
     * @return A new histogram2d object
     */
    inline static histogram2d calloc( size_t const nx, size_t const ny ){
      return histogram2d( gsl_histogram2d_calloc( nx, ny ) ); }

    /**
     * C++ version of gsl_histogram2d_calloc_uniform().
     * @param nx Number of bin slices in x direction
     * @param ny Number of bin slices in y direction
     * @param xmin Lower bound of bottom x range
     * @param xmax Upper bound of top x range
     * @param ymin Lower bound of bottom y range
     * @param ymax Upper bound of top y range
     * @return A new histogram2d object
     */
    inline static histogram2d calloc_uniform( size_t const nx, size_t const ny,
					      double const xmin, double const xmax,
					      double const ymin, double const ymax ){
      return histogram2d( gsl_histogram2d_calloc_uniform( nx, ny, xmin, xmax, ymin, ymax ) ); }

    /**
     * C++ version of gsl_histogram2d_increment().
     * @param x The x value
     * @param y The y value
     * @return Error code on failure
     */
    int increment( double x, double y ){ return gsl_histogram2d_increment( get(), x, y ); }

    /**
     * C++ version of gsl_histogram2d_accumulate().
     * @param x The x value
     * @param y The y value
     * @param weight The weight given to (x,y)
     * @return Error code on failure
     */
    int accumulate( double x, double y, double weight ){
      return gsl_histogram2d_accumulate( get(), x, y, weight ); } 

    /**
     * C++ version of gsl_histogram2d_find().
     * @param x The x value
     * @param y The y value
     * @param i The index in the x direction (return value)
     * @param j The index in the y direction (return value)
     * @return Error code on failure
     */
    int find( double const x, double const y, size_t* i, size_t* j ) const {
      return gsl_histogram2d_find( get(), x, y, i, j ); }

    /**
     * C++ version of gsl_histogram2d_get().
     * @param i The index in the x direction
     * @param j The index in the y direction
     * @return The size of the bin at (i,j)
     */
    double get( size_t const i, size_t const j ) const {
      return gsl_histogram2d_get( get(), i, j ); }

    /**
     * C++ version of gsl_histogram2d_get_xrange().
     * @param i The index in the x direction
     * @param xlower The lower bound
     * @param xupper The upper bound
     * @return Error code on failure
     */
    int get_xrange( size_t const i, double* xlower, double* xupper ) const {
      return gsl_histogram2d_get_xrange( get(), i, xlower, xupper ); }

    /**
     * C++ version of gsl_histogram2d_get_yrange().
     * @param j The index in the y direction
     * @param ylower The lower bound
     * @param yupper The upper bound
     * @return Error code on failure
     */
    int get_yrange( size_t const j, double* ylower, double* yupper ) const {
      return gsl_histogram2d_get_yrange( get(), j, ylower, yupper ); }

    /**
     * C++ version of gsl_histogram2d_xmax().
     * @return The upper bound of the top x range
     */
    double xmax() const { return gsl_histogram2d_xmax( get() ); }

    /**
     * C++ version of gsl_histogram2d_xmin().
     * @return The lower bound of the bottom x range
     */
    double xmin() const { return gsl_histogram2d_xmin( get() ); }

    /**
     * C++ version of gsl_histogram2d_nx().
     * @return The number of bin slices in the x direction
     */
    size_t nx() const { return gsl_histogram2d_nx( get() ); }

    /**
     * C++ version of gsl_histogram2d_ymax().
     * @return The upper bound of the top y range
     */
    double ymax() const { return gsl_histogram2d_ymax( get() ); }

    /**
     * C++ version of gsl_histogram2d_ymin().
     * @return The lower bound of the bottom y range
     */
    double ymin() const { return gsl_histogram2d_ymin( get() ); }

    /**
     * C++ version of gsl_histogram2d_ny().
     * @return The number of bin slices in the y direction
     */
    size_t ny() const { return gsl_histogram2d_ny( get() ); }

    /**
     * C++ version of gsl_histogram2d_reset().
     */
    void reset(){ gsl_histogram2d_reset( get() ); }

    /**
     * C++ version of gsl_histogram2d_calloc_range().
     * @param nx Number of bin slices in x direction
     * @param ny Number of bin slices in y direction
     * @param xrange The x ranges
     * @param yrange The y ranges
     * @return A new histogram2d object
     */
    inline static histogram2d calloc_range( size_t nx, size_t ny, double* xrange, double* yrange ){
      return histogram2d( gsl_histogram2d_calloc_range( nx, ny, xrange, yrange ) ); }
    /**
     * C++ version of gsl_histogram2d_calloc_range().
     * This version works with std::vector<double> and gsl::vector
     * @param xrange The x ranges
     * @param yrange The y ranges
     * @return A new histogram2d object
     */
    template<typename XRANGE,typename YRANGE> 
    inline static histogram2d calloc_range( XRANGE& xrange, YRANGE& yrange ){
      return histogram2d( gsl_histogram2d_calloc_range( xrange.size(), yrange.size(),
							xrange.data(), yrange.data() ) ); }

    /**
     * C++ version of gsl_histogram2d_set_ranges_uniform().
     * @param xmin Lower bound of bottom x range
     * @param xmax Upper bound of top x range
     * @param ymin Lower bound of bottom y range
     * @param ymax Upper bound of top y range
     * @return Error code on failure
     */
    int set_ranges_uniform( double xmin, double xmax, double ymin, double ymax ){
      return gsl_histogram2d_set_ranges_uniform( get(), xmin, xmax, ymin, ymax ); }

    /**
     * C++ version of gsl_histogram2d_set_ranges().
     * @param xrange[] The x ranges
     * @param xsize The size of xrange
     * @param yrange[] The y ranges
     * @param ysize The size of yrange
     * @return Error code on failure
     */
    int set_ranges( double const xrange[], size_t xsize, double const yrange[], size_t ysize ){
      return gsl_histogram2d_set_ranges( get(), xrange, xsize, yrange, ysize ); }
    /**
     * C++ version of gsl_histogram2d_set_ranges().
     * This version works with std::vector<double> and gsl::vector
     * @param xrange The x ranges
     * @param yrange The y ranges
     * @return Error code on failure
     */
    template<typename XRANGE,typename YRANGE> 
    int set_ranges( XRANGE const& xrange, YRANGE const& yrange ){
      return gsl_histogram2d_set_ranges( get(), xrange.data(), xrange.size(),
					 yrange.data(), yrange.size() ); }

    /**
     * C++ version of gsl_histogram2d_memcpy().
     * @param source A histogram2d object
     * @return Error code on failure
     */
    int memcpy( histogram2d const& source ){
      return gsl_histogram2d_memcpy( get(), source.get() ); }

    /**
     * C++ version of gsl_histogram2d_clone().
     * @return A new histogram2d object
     */
    histogram2d clone() const { return histogram2d( gsl_histogram2d_clone( get() ) ); }

    /**
     * C++ version of gsl_histogram2d_max_val().
     * @return The maximum value contained in the histogram
     */
    double max_val() const { return gsl_histogram2d_max_val( get() ); }

    /**
     * C++ version of gsl_histogram2d_max_bin().
     * @param i The index in the x direction (return value)
     * @param j The index in the y direction (return value)
     */
    void max_bin( size_t* i, size_t* j ) const { gsl_histogram2d_max_bin( get(), i, j ); }

    /**
     * C++ version of gsl_histogram2d_min_val().
     * @return The minimum value contained in the histogram
     */
    double min_val() const { return gsl_histogram2d_min_val( get() ); }

    /**
     * C++ version of gsl_histogram2d_min_bin().
     * @param i The index in the x direction (return value)
     * @param j The index in the y direction (return value)
     */
    void min_bin( size_t* i, size_t* j ) const { gsl_histogram2d_min_bin( get(), i, j ); }

    /**
     * C++ version of gsl_histogram2d_xmean().
     * @return The mean of the histogrammed x values
     */
    double xmean() const { return gsl_histogram2d_xmean( get() ); }

    /**
     * C++ version of gsl_histogram2d_ymean().
     * @return The mean of the histogrammed y values
     */
    double ymean() const { return gsl_histogram2d_ymean( get() ); }

    /**
     * C++ version of gsl_histogram2d_xsigma().
     * @return The standard deviation of the histogrammed x values
     */
    double xsigma() const { return gsl_histogram2d_xsigma( get() ); }

    /**
     * C++ version of gsl_histogram2d_ysigma().
     * @return The standard deviation of the histogrammed x values
     */
    double ysigma() const { return gsl_histogram2d_ysigma( get() ); }

    /**
     * C++ version of gsl_histogram2d_cov().
     * @return The covariance of the histogrammed values
     */
    double cov() const { return gsl_histogram2d_cov( get() ); }

    /**
     * C++ version of gsl_histogram2d_sum().
     * @return The sum of all the histogram values
     */
    double sum() const { return gsl_histogram2d_sum( get() ); }

    /**
     * C++ version of gsl_histogram2d_equal_bins_p().
     * @param h2 Another histogram
     * @return @em true or @em false according as h2 does or does not contain the same
     * number of x and y bin slices (ranges) as @c this
     */
    bool equal_bins_p( histogram2d const& h2 ) const {
      return gsl_histogram2d_equal_bins_p( get(), h2.get() ) != 0; }

    /**
     * C++ version of gsl_histogram2d_add().
     * @param h2 Another histogram
     * @return Error code on failure
     */
    int add( histogram2d const& h2 ){ return gsl_histogram2d_add( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram2d_sub().
     * @param h2 Another histogram
     * @return Error code on failure
     */
    int sub( histogram2d const& h2 ){ return gsl_histogram2d_sub( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram2d_mul().
     * @param h2 Another histogram
     * @return Error code on failure
     */
    int mul( histogram2d const& h2 ){ return gsl_histogram2d_mul( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram2d_div().
     * @param h2 Another histogram
     * @return Error code on failure
     */
    int div( histogram2d const& h2 ){ return gsl_histogram2d_div( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram2d_scale().
     * @param scale A constant
     * @return Error code on failure
     */
    int scale( double scale ){ return gsl_histogram2d_scale( get(), scale ); }

    /**
     * C++ version of gsl_histogram2d_shift().
     * @param shift A constant
     * @return Error code on failure
     */
    int shift( double shift ){ return gsl_histogram2d_shift( get(), shift ); }

    /**
     * C++ version of gsl_histogram2d_fwrite().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_histogram2d_fwrite( stream, get() ); }

    /**
     * C++ version of gsl_histogram2d_fread().
     * @param stream An input stream
     * @return Error code on failure
     */
    int fread( FILE* stream ){ return gsl_histogram2d_fread( stream, get() ); } 

    /**
     * C++ version of gsl_histogram2d_fprintf().
     * @param stream An output stream
     * @param range_format One of the %g, %e or %f formats
     * @param bin_format One of the %g, %e or %f formats
     * @return Error code on failure
     */
    int fprintf( FILE* stream, char const* range_format, char const* bin_format ) const {
      return gsl_histogram2d_fprintf( stream, get(), range_format, bin_format ); }

    /**
     * C++ version of gsl_histogram2d_fscanf().
     * @param stream An input stream
     * @return Error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_histogram2d_fscanf( stream, get() ); }

    //PDF functions

    /**
     * Empirical probability density functions.
     */
    class pdf {
    public:
      /**
       * The default constructor is only really useful for assigning to.
       */
      pdf(){
	ccgsl_pointer = 0;
	count = 0; // ititially nullptr will do
      }
      // Refines random access container
      // Refines assignable
      /**
       * The default constructor creates a new pdf with n elements.
       * @param nx The number of bin slices in x direction
       * @param ny The number of bin slices in y direction
       */
      explicit pdf( size_t const nx, size_t const ny ){
	ccgsl_pointer = gsl_histogram2d_pdf_alloc( nx, ny );
	// just plausibly we could allocate pdf but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_histogram2d_pdf_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_histogram2d_pdf.
       * This is not usually a good idea. In this case
       * we should not use gsl_histogram2d_pdf_free() to deallocate the memory.
       * @param v The pdf
       */
      explicit pdf( gsl_histogram2d_pdf* v ){
	ccgsl_pointer = v;
	// just plausibly we could fail to allocate count: no further action needed.
	count = new size_t;
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      // copy constructor
      /**
       * The copy constructor. This creates a new reference to the workspace.
       * @param v The pdf to copy.
       */
      pdf( pdf const& v ){ ccgsl_pointer = v.ccgsl_pointer;
	count = v.count; if( count != 0 ) ++*count; }
      // assignment operator
      /**
       * The assignment operator. This copies elementwise.
       * @param v The pdf to copy
       */
      pdf& operator=( pdf const& v ){
	// first, possibly delete anything pointed to by this
	if( count == 0 or --*count == 0 ){
	  if( ccgsl_pointer != 0 ) gsl_histogram2d_pdf_free( ccgsl_pointer );
	  delete count;
	} // Then copy
	ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
      }
      // destructor
      /**
       * The destructor only deletes the pointers if count reaches zero.
       */
      ~pdf(){
	if( count == 0 or --*count == 0 ){
	  // could have allocated null pointer
	  if( ccgsl_pointer != 0 ) gsl_histogram2d_pdf_free( ccgsl_pointer );
	  delete count;
	}
      }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      /**
       * Move constructor.
       * @param v The pdf to move.
       */
      pdf( pdf&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
	std::swap( count, v.count );
	v.ccgsl_pointer = nullptr;
      }
      /**
       * Move operator.
       * @param v The pdf to move.
       * @return A reference to this.
       */
      pdf& operator=( pdf&& v ){
	pdf( std::move( v ) ).swap( *this );
	return *this;
      }
#endif
      // Refines equality comparable
      // == operator
      /**
       * Two pdf are identically equal if their elements are identical.
       * @param v The pdf to be compared with @c this
       * @return @c true or @c false according as @c this and @c v have
       * identical elements or not
       */
      bool operator==( pdf const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
      // != operator
      /**
       * Two pdf are different if their elements are not identical.
       * @param v The pdf to be compared with @c this
       * @return @c false or @c true according as @c this and @c v have
       * identical elements or not
       */
      bool operator!=( pdf const& v ) const { return not operator==( v ); }
      // Refines forward container
      // Refines less than comparable
      // operator<
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a pdf is nonnegative.
       * @param v The pdf to be compared with @c this
       * @return @c false or @c true according as @c this is less than @c v
       * lexicographically
       */
      bool operator<( pdf const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
      // operator>
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a pdf is nonnegative.
       * @param v The pdf to be compared with @c this
       * @return @c false or @c true according as @c this is greater than @c v
       * lexicographically
       */
      bool operator>( pdf const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
      // operator<=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a pdf is nonnegative.
       * @param v The pdf to be compared with @c this
       * @return @c false or @c true according as @c this is less than
       * or equal to @c v lexicographically
       */
      bool operator<=( pdf const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
      // operator>=
      /**
       * A container needs to define an ordering for sorting. This uses
       * standard lexicographical ordering and so is not useful,
       * for example, for checking, that a pdf is nonnegative.
       * @param v The pdf to be compared with @c this
       * @return @c false or @c true according as @c this is no 
       * less than @c v lexicographically
       */
      bool operator>=( pdf const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
      /**
       * Find if the pdf is empty.
       * @return @c true if has size zero; otherwise @c false
       */
      bool empty() const { return ccgsl_pointer == 0; }
      // swap() --- should work even if sizes don't match
      /**
       * Swap two pdf objects. This works even if the pdf have different sizes
       * because it swaps pointers.
       * @param v The pdf to swap with @c this.
       */
      void swap( pdf& v ){
	std::swap( ccgsl_pointer, v.ccgsl_pointer );
	std::swap( count, v.count );
      }
    private:
      /**
       * The shared pointer
       */
      gsl_histogram2d_pdf* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_histogram2d_pdf.
       * @return the gsl_histogram2d_pdf
       */
      gsl_histogram2d_pdf* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_histogram2d_pdf.
       * @return @c true or @c falses according as 
       * this is the only pdf object sharing the gsl_histogram2d_pdf.
       */
      bool unique() const { return count != 0 and *count == 1; }
      /**
       * Find how many pdf objects share this pointer.
       * @return the number of pdf objects that share this pointer.
       */
      size_t use_count() const { return count == 0 ? 0 : *count; }
      /**
       * Allow conversion to bool.
       * @return @c true or @c false according as this contains a pointer
       * to a gsl_histogram2d_pdf.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_histogram2d_pdf_init().
       * @param h A histogram2d
       * @return Error code on failure
       */
      int init( histogram2d const& h ) const { return gsl_histogram2d_pdf_init( get(), h.get() ); }

      /**
       * C++ version of gsl_histogram2d_pdf_sample().
       * @param r1 A random number (between zero and 1)
       * @param r2 A random number (between zero and 1)
       * @param x The x value (return value)
       * @param y The y value (return value)
       * @return Error code on failure
       */
      int sample( double r1, double r2, double* x, double* y ){
	return gsl_histogram2d_pdf_sample( get(), r1, r2, x, y ); }
    
    };
  };
}
#endif
