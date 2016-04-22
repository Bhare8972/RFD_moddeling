/*
 * $Id: histogram.hpp 293 2012-12-17 20:27:36Z jdl3 $
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

#ifndef CCGSL_HISTOGRAM_HPP
#define CCGSL_HISTOGRAM_HPP

#include<new>
#include<gsl/gsl_histogram.h>
#include"exception.hpp"

namespace gsl {
  /**
   * Histograms.
   */
  class histogram {
  public:

  /////my extra functions
    histogram( size_t const n, double xmin, double xmax )
    {
          ccgsl_pointer = gsl_histogram_alloc( n );
          // just plausibly we could allocate histogram but not count
          try { count = new size_t; } catch( std::bad_alloc& e ){
        // try to tidy up before rethrowing
        gsl_histogram_free( ccgsl_pointer );
        throw e;
          }
          *count = 1; // initially there is just one reference to ccgsl_pointer

          gsl_histogram_set_ranges_uniform( ccgsl_pointer, xmin, xmax );
    }

    histogram( gsl::vector ranges, gsl::vector values )
    {
          ccgsl_pointer = gsl_histogram_alloc( values.size() );
          // just plausibly we could allocate histogram but not count
          try { count = new size_t; } catch( std::bad_alloc& e ){
        // try to tidy up before rethrowing
        gsl_histogram_free( ccgsl_pointer );
        throw e;
          }
          *count = 1; // initially there is just one reference to ccgsl_pointer

          gsl_histogram_set_ranges( ccgsl_pointer, ranges.data(), ranges.size() );

          for(size_t bin_i=0; bin_i<values.size(); bin_i++)
          {
                ccgsl_pointer->bin[bin_i]=values[bin_i];
          }

    }


    gsl::vector get_bin_ranges()
    {
        gsl::vector out(ccgsl_pointer->n+1);
        for(size_t bin_i=0; bin_i<(ccgsl_pointer->n+1); bin_i++)
        {
            out[bin_i]=ccgsl_pointer->range[bin_i];
        }
        return out;
    }

    gsl::vector get_bin_values()
    {
        gsl::vector out(ccgsl_pointer->n);
        for(size_t bin_i=0; bin_i<(ccgsl_pointer->n); bin_i++)
        {
            out[bin_i]=ccgsl_pointer->bin[bin_i];
        }
        return out;
    }


    /**
     * The default constructor is only really useful for assigning to.
     */
    histogram(){
      ccgsl_pointer = 0;
      count = 0; // ititially nullptr will do
    }
    // Refines random access container
    // Refines assignable
    /**
     * The default constructor creates a new histogram with n elements.
     * @param n number of bins
     */
    explicit histogram( size_t const n )
    {
          ccgsl_pointer = gsl_histogram_alloc( n );
          // just plausibly we could allocate histogram but not count
          try { count = new size_t; } catch( std::bad_alloc& e ){
        // try to tidy up before rethrowing
        gsl_histogram_free( ccgsl_pointer );
        throw e;
          }
          *count = 1; // initially there is just one reference to ccgsl_pointer
    }

    /**
     * Could construct from a gsl_histogram.
     * This is not usually a good idea. In this case
     * we should not use gsl_histogram_free() to deallocate the memory.
     * @param v The histogram
     */
    explicit histogram( gsl_histogram* v ){
      ccgsl_pointer = v;
      // just plausibly we could fail to allocate count: no further action needed.
      count = new size_t;
      *count = 1; // initially there is just one reference to ccgsl_pointer
    }
    // copy constructor
    /**
     * The copy constructor. This creates a new reference to the workspace.
     * @param v The histogram to copy.
     */
    histogram( histogram const& v ){ ccgsl_pointer = v.ccgsl_pointer;
      count = v.count; if( count != 0 ) ++*count; }
    // assignment operator
    /**
     * The assignment operator. This copies elementwise.
     * @param v The histogram to copy
     */
    histogram& operator=( histogram const& v ){
      // first, possibly delete anything pointed to by this
      if( count == 0 or --*count == 0 ){
	if( ccgsl_pointer != 0 ) gsl_histogram_free( ccgsl_pointer );
	delete count;
      } // Then copy
      ccgsl_pointer = v.ccgsl_pointer; count = v.count; if( count != 0 ) ++*count; return *this;
    }
    // destructor
    /**
     * The destructor only deletes the pointers if count reaches zero.
     */
    ~histogram(){
      if( count == 0 or --*count == 0 ){
	// could have allocated null pointer
	if( ccgsl_pointer != 0 ) gsl_histogram_free( ccgsl_pointer );
	delete count;
      }
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    /**
     * Move constructor.
     * @param v The histogram to move.
     */
    histogram( histogram&& v ) : ccgsl_pointer( v.ccgsl_pointer ), count( nullptr ){
      std::swap( count, v.count );
      v.ccgsl_pointer = nullptr;
    }
    /**
     * Move operator.
     * @param v The histogram to move.
     * @return A reference to this.
     */
    histogram& operator=( histogram&& v ){
      histogram( std::move( v ) ).swap( *this );
      return *this;
    }
#endif
    // Refines equality comparable
    // == operator
    /**
     * Two histogram are identically equal if their elements are identical.
     * @param v The histogram to be compared with @c this
     * @return @c true or @c false according as @c this and @c v have
     * identical elements or not
     */
    bool operator==( histogram const& v ) const { return ccgsl_pointer == v.ccgsl_pointer; }
    // != operator
    /**
     * Two histogram are different if their elements are not identical.
     * @param v The histogram to be compared with @c this
     * @return @c false or @c true according as @c this and @c v have
     * identical elements or not
     */
    bool operator!=( histogram const& v ) const { return not operator==( v ); }
    // Refines forward container
    // Refines less than comparable
    // operator<
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram is nonnegative.
     * @param v The histogram to be compared with @c this
     * @return @c false or @c true according as @c this is less than @c v
     * lexicographically
     */
    bool operator<( histogram const& v ) const { return ccgsl_pointer < v.ccgsl_pointer; }
    // operator>
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram is nonnegative.
     * @param v The histogram to be compared with @c this
     * @return @c false or @c true according as @c this is greater than @c v
     * lexicographically
     */
    bool operator>( histogram const& v ) const { return ccgsl_pointer > v.ccgsl_pointer; }
    // operator<=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram is nonnegative.
     * @param v The histogram to be compared with @c this
     * @return @c false or @c true according as @c this is less than
     * or equal to @c v lexicographically
     */
    bool operator<=( histogram const& v ) const { return ccgsl_pointer <= v.ccgsl_pointer; }
    // operator>=
    /**
     * A container needs to define an ordering for sorting. This uses
     * standard lexicographical ordering and so is not useful,
     * for example, for checking, that a histogram is nonnegative.
     * @param v The histogram to be compared with @c this
     * @return @c false or @c true according as @c this is no
     * less than @c v lexicographically
     */
    bool operator>=( histogram const& v ) const { return ccgsl_pointer >= v.ccgsl_pointer; }
    /**
     * Find if the histogram is empty.
     * @return @c true if has size zero; otherwise @c false
     */
    bool empty() const { return ccgsl_pointer == 0; }
    // swap() --- should work even if sizes don't match
    /**
     * Swap two histogram objects. This works even if the histogram have different sizes
     * because it swaps pointers.
     * @param v The histogram to swap with @c this.
     */
    void swap( histogram& v ){
      std::swap( ccgsl_pointer, v.ccgsl_pointer );
      std::swap( count, v.count );
    }
  private:
    /**
     * The shared pointer
     */
    gsl_histogram* ccgsl_pointer;
    /**
     * The shared reference count
     */
    size_t* count;
  public:
    // shared reference functions
    /**
     * Get the gsl_histogram.
     * @return the gsl_histogram
     */
    gsl_histogram* get() const { return ccgsl_pointer; }
    /**
     * Find if this is the only object sharing the gsl_histogram.
     * @return @c true or @c falses according as
     * this is the only histogram object sharing the gsl_histogram.
     */
    bool unique() const { return count != 0 and *count == 1; }
    /**
     * Find how many histogram objects share this pointer.
     * @return the number of histogram objects that share this pointer.
     */
    size_t use_count() const { return count == 0 ? 0 : *count; }
    /**
     * Allow conversion to bool.
     * @return @c true or @c false according as this contains a pointer
     * to a gsl_histogram.
     */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    explicit
#endif
    operator bool() const { return ccgsl_pointer != 0; }

    /**
     * C++ version of gsl_histogram_calloc().
     * @param n The number of bins
     * @return The histogram
     */
    inline static histogram calloc( size_t n ){ return histogram( gsl_histogram_calloc( n ) ); }

    /**
     * C++ version of gsl_histogram_calloc_uniform().
     * @param n The number of bins
     * @param xmin The lower bound on bottom bin
     * @param xmax The upper bound on top bin
     * @return The number of histograms
     */
    inline static histogram calloc_uniform( size_t const n, double const xmin, double const xmax ){
      return histogram( gsl_histogram_calloc_uniform( n, xmin, xmax ) ); }

    /**
     * C++ version of gsl_histogram_increment().
     * @param x A real value
     * @return Error code on failure
     */
    int increment( double x ){ return gsl_histogram_increment( get(), x ); }


    void increment( gsl::vector X )
    {
        for(double v : X)
        {
            int i=gsl_histogram_increment( get(), v );
            if(i!=0)
            {
                throw gsl::exception("histrogram increment value out of range", "histogram.hpp", 277, i);
            }
        }
    }

    /**
     * C++ version of gsl_histogram_accumulate().
     * @param x A real value
     * @param weight The weight to give to x
     * @return Error code on failure
     */
    int accumulate( double x, double weight ){
      return gsl_histogram_accumulate( get(), x, weight ); }

    /**
     * C++ version of gsl_histogram_find().
     * @param x A real value
     * @param i The index of the bin containing x (return value)
     * @return Error code on failure
     */
    int find( double const x, size_t* i ) const {
      return gsl_histogram_find( get(), x, i ); }

    /**
     * C++ version of gsl_histogram_get().
     * @param i An index
     * @return The value of the ith bin
     */
    double get( size_t i ) const { return gsl_histogram_get( get(), i ); }

    /**
     * C++ version of gsl_histogram_get().
     * @param i An index
     * @return The value of the ith bin
     */
    double operator[]( size_t i ) const { return gsl_histogram_get( get(), i ); }

    /**
     * C++ version of gsl_histogram_get_range().
     * @param i An index
     * @param lower The lower bound of the bin (return value)
     * @param upper The upper bound of the bin (return value)
     * @return Error code on failure
     */
    int get_range( size_t i, double* lower, double* upper ) const {
      return gsl_histogram_get_range( get(), i, lower, upper ); }

    /**
     * C++ version of gsl_histogram_max().
     * @return The maximum upper bound of the histogram
     */
    double max() const { return gsl_histogram_max( get() ); }

    /**
     * C++ version of gsl_histogram_min().
     * @return The minimum lower bound of the histogram
     */
    double min() const { return gsl_histogram_min( get() ); }

    /**
     * C++ version of gsl_histogram_bins().
     * @return The number of bins
     */
    size_t bins() const { return gsl_histogram_bins( get() ); }

    /**
     * C++ version of gsl_histogram_reset().
     */
    void reset(){ gsl_histogram_reset( get() ); }

    /**
     * C++ version of gsl_histogram_calloc_range().
     * @param n The number of bins
     * @param range The range
     * @return The histogram
     */
    inline static histogram calloc_range( size_t n, double* range ){
      return histogram( gsl_histogram_calloc_range( n, range ) ); }
    /**
     * C++ version of gsl_histogram_calloc_range().
     * This version works with std::vector<double> and gsl::vector
     * @param range The range
     * @return The histogram
     */
    template<typename RANGE>
    inline static histogram calloc_range( RANGE& range ){
      return histogram( gsl_histogram_calloc_range( range.size(), range.data() ) ); }

    /**
     * C++ version of gsl_histogram_set_ranges().
     * This version works with std::vector<double> and gsl::vector
     * @param range[] A set of range bounds
     * @return Error code on failure
     */
    template<typename RANGE>
    int set_ranges( RANGE const& range ){
      return gsl_histogram_set_ranges( get(), range.data(), range.size() ); }

    /**
     * C++ version of gsl_histogram_set_ranges_uniform().
     * @param xmin The new minimum lower bound
     * @param xmax The new maximum upper bound
     * @return Error code on failure
     */
    int set_ranges_uniform( double xmin, double xmax ){
      return gsl_histogram_set_ranges_uniform( get(), xmin, xmax ); }

    /**
     * C++ version of gsl_histogram_memcpy().
     * @param source A histogram
     * @return Error code on failure
     */
    int memcpy( histogram const& source ){ return gsl_histogram_memcpy( get(), source.get() ); }

    /**
     * C++ version of gsl_histogram_clone().
     * @return UNDOCUMENTED
     */
    histogram clone() const { return histogram( gsl_histogram_clone( get() ) ); }

    /**
     * C++ version of gsl_histogram_max_val().
     * @return The maximum value
     */
    double max_val() const { return gsl_histogram_max_val( get() ); }

    /**
     * C++ version of gsl_histogram_max_bin().
     * @return The index of the bin containing the maximum value
     */
    size_t max_bin() const { return gsl_histogram_max_bin( get() ); }

    /**
     * C++ version of gsl_histogram_min_val().
     * @return The minimum value
     */
    double min_val() const { return gsl_histogram_min_val( get() ); }

    /**
     * C++ version of gsl_histogram_min_bin().
     * @return The index of the bin containing the minimum value
     */
    size_t min_bin() const { return gsl_histogram_min_bin( get() ); }

    /**
     * C++ version of gsl_histogram_equal_bins_p().
     * @param h2 A histogram
     * @return @em true or @em false according as @c this and h2 do or do not have
     * identical ranges
     */
    bool equal_bins_p( histogram const& h2 ) const {
      return gsl_histogram_equal_bins_p( get(), h2.get() ) != 0; }

    /**
     * C++ version of gsl_histogram_add().
     * @param h2 A histogram
     * @return Error code on failure
     */
    int add( histogram const& h2 ){ return gsl_histogram_add( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram_sub().
     * @param h2 A histogram
     * @return Error code on failure
     */
    int sub( histogram const& h2 ){ return gsl_histogram_sub( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram_mul().
     * @param h2 A histogram
     * @return Error code on failure
     */
    int mul( histogram const& h2 ){ return gsl_histogram_mul( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram_div().
     * @param h2 A histogram
     * @return Error code on failure
     */
    int div( histogram const& h2 ){ return gsl_histogram_div( get(), h2.get() ); }

    /**
     * C++ version of gsl_histogram_scale().
     * @param scale A constant
     * @return Error code on failure
     */
    int scale( double const scale ){ return gsl_histogram_scale( get(), scale ); }

    /**
     * C++ version of gsl_histogram_shift().
     * @param shift A constant
     * @return Error code on failure
     */
    int shift( double const shift ){ return gsl_histogram_shift( get(), shift ); }

    /**
     * C++ version of gsl_histogram_sigma().
     * @return The standard deviation of the histogrammed variable
     */
    double sigma() const { return gsl_histogram_sigma( get() ); }

    /**
     * C++ version of gsl_histogram_mean().
     * @return The mean of the histogrammed variable
     */
    double mean() const { return gsl_histogram_mean( get() ); }

    /**
     * C++ version of gsl_histogram_sum().
     * @return The sum of all the binned values
     */
    double sum() const { return gsl_histogram_sum( get() ); }

    /**
     * C++ version of gsl_histogram_fwrite().
     * @param stream An output stream
     * @return Error code on failure
     */
    int fwrite( FILE* stream ) const { return gsl_histogram_fwrite( stream, get() ); }

    /**
     * C++ version of gsl_histogram_fread().
     * @param stream An input stream
     * @return Error code on failure
     */
    int fread( FILE* stream ){ return gsl_histogram_fread( stream, get() ); }

    /**
     * C++ version of gsl_histogram_fprintf().
     * @param stream An output stream
     * @param range_format One of the %g, %e or %f formats
     * @param bin_format One of the %g, %e or %f formats
     * @return Error code on failure
     */
    int fprintf( FILE* stream, char const* range_format, char const* bin_format ) const {
      return gsl_histogram_fprintf( stream, get(), range_format, bin_format ); }

    /**
     * C++ version of gsl_histogram_fscanf().
     * @param stream An inoutput stream
     * @return Error code on failure
     */
    int fscanf( FILE* stream ){ return gsl_histogram_fscanf( stream, get() ); }

    //PDF functions
    /**
     * Histogram probability (empirical) density functions.
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
       * @param n The number of bins to approximate the pdf
       */
      explicit pdf( size_t const n ){
	ccgsl_pointer = gsl_histogram_pdf_alloc( n );
	// just plausibly we could allocate pdf but not count
	try { count = new size_t; } catch( std::bad_alloc& e ){
	  // try to tidy up before rethrowing
	  gsl_histogram_pdf_free( ccgsl_pointer );
	  throw e;
	}
	*count = 1; // initially there is just one reference to ccgsl_pointer
      }
      /**
       * Could construct from a gsl_histogram_pdf.
       * This is not usually a good idea. In this case
       * we should not use gsl_histogram_pdf_free() to deallocate the memory.
       * @param v The pdf
       */
      explicit pdf( gsl_histogram_pdf* v ){
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
	  if( ccgsl_pointer != 0 ) gsl_histogram_pdf_free( ccgsl_pointer );
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
	  if( ccgsl_pointer != 0 ) gsl_histogram_pdf_free( ccgsl_pointer );
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
      gsl_histogram_pdf* ccgsl_pointer;
      /**
       * The shared reference count
       */
      size_t* count;
    public:
      // shared reference functions
      /**
       * Get the gsl_histogram_pdf.
       * @return the gsl_histogram_pdf
       */
      gsl_histogram_pdf* get() const { return ccgsl_pointer; }
      /**
       * Find if this is the only object sharing the gsl_histogram_pdf.
       * @return @c true or @c falses according as
       * this is the only pdf object sharing the gsl_histogram_pdf.
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
       * to a gsl_histogram_pdf.
       */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      explicit
#endif
      operator bool() const { return ccgsl_pointer != 0; }

      /**
       * C++ version of gsl_histogram_pdf_init().
       * @param h A histogram
       * @return Error code on failure
       */
      int init( histogram const& h ){ return gsl_histogram_pdf_init( get(), h.get() ); }

      /**
       * C++ version of gsl_histogram_pdf_sample().
       * @param r A random number (between zero and 1)
       * @return A random variate from the histogram
       */
      double sample( double r ){ return gsl_histogram_pdf_sample( get(), r ); }
    };
  };
}
#endif

