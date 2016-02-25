/*
 * $Id: siman.hpp 200 2012-08-05 19:11:27Z jdl3 $
 * Copyright (C) 2012 John D Lamb
 * Copyright (C) 2007 Brian Gough
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Mark Galassi
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

#ifndef CCGSL_SIMAN_HPP
#define CCGSL_SIMAN_HPP

#include<cstring>
#include<gsl/gsl_machine.h>
#include<gsl/gsl_siman.h>
#include"rng.hpp"

namespace gsl {
  /**
   * Simulated annealing. Use this class as siman<XP>, where XP is the class whose
   * objects represent configurations.
   * 
   * Ideally these functions would wrap the GSL functions as almost all of ccgsl does.
   * However, gsl_siman_solve() uses as arguments pointers to functions with void pointer
   * arguments. In C++ we want to avoid creating such functions explicitly, 
   * currently (2012) I know of no way to create them inside a class in such a way that they
   * can be generically constructed as required. It is not possible to substitute a pointer to
   * a function object for a pointer to a function and, while it is possible to create a 
   * lambda function in C++11, it is not legal to pass its address as a function pointer.
   *
   * The compromise here is to wrap the original code from gsl_siman.c in template functions
   * with suitable template arguments. It has the desired effect but means that if gsl_siman.c
   * is updated, the code for ccgsl/siman.hpp must be updated separately to take advantage of
   * any changes.
   */
  template<typename XP>
  struct siman {
    // Types for function pointes
    /**
     * Function returning then energy of a configuration.
     */
    typedef double (*Efunc_t)( XP& xp );
    /**
     * Function to modify configuration with random number up to maximum step size
     */
    typedef void (*step_t)( rng& r, XP& xp, double step_size );
    /**
     * Function to find distance between two configurations.
     */
    typedef double (*metric_t)( XP& xp, XP& yp );
    /**
     * Function to print a configuration.
     */
    typedef void (*print_t)( XP& xp );
    /**
     * Function to copy configuration contents from source to dest
     */
    typedef void (*copy_t)( XP& source, XP& dest );
    /**
     * Function to construct a configuration.
     */
    typedef XP* (*copy_construct_t)( XP& xp );
    /**
     * Function to destruct a configuration.
     */
    typedef void (*destroy_t)( XP* xp );
    /**
     * Parameters: same as gsl_siman_params_t.
     */
    typedef gsl_siman_params_t params_t;
    /**
     * C++ version of gsl_siman_solve(). If configurations are C++ objects, typically
     * you should provide copyfunc, copy_construct and destructor as non-null pointers and
     * element_size will not be used. Otherwise, element_size must be the size of an object and
     * the object will be copied directly, which is unwise if a configuration object contains
     * pointers.
     * @param r A random number generator
     * @param x0_p The initial configuration (also final configuration on return)
     * @param Ef The Energy fiunction: gives energy of configuration
     * @param take_step The step function
     * @param distance The distance function
     * @param print_position Function to print position
     * @param copyfunc Function to copy a configuration
     * @param copy_constructor Function to copy-construct a configuration
     * @param destructor Function to delete a configuration
     * @param params Parameters of the Simulated Annealing heuristic
     */
    inline static void solve( rng& r, XP& x0_p, Efunc_t Ef, step_t take_step,
			      metric_t distance, print_t print_position,
			      copy_t copyfunc, copy_construct_t copy_constructor,
			      destroy_t destructor, params_t& params ){
      // This is adapted directly from the GSL. This isn't ideal. But it's the only
      // current way to allow us to pass suitable function pointers.

      // inline functions adapted as local functions
      struct {
	double operator()( double E, double new_E, double T, params_t& params ) const {
	  double x = -(new_E - E) / (params.k * T);
	  /* avoid underflow errors for large uphill steps */
	  return (x < GSL_LOG_DBL_MIN) ? 0.0 : exp(x);
	}
      } boltzmann;
      
      struct {
	void operator()( XP& src, XP& dst, size_t size, copy_t copyfunc ) const {
	  if (copyfunc) {
	    copyfunc(src, dst);
	  } else {
	    memcpy(reinterpret_cast<void*>( &dst ), reinterpret_cast<void*>( &src ), size);
	  }
	}
      } copy_state;
      
      XP *x, *new_x, *best_x;
      double E, new_E, best_E;
      int i;
      double T, T_factor;
      int n_evals = 1, n_iter = 0, n_accepts, n_rejects, n_eless;

      /* this function requires that either the dynamic functions (copy,
	 copy_constructor and destructor) are passed, or that an element
	 size is given */
      size_t element_size = 0;
      if( copyfunc == NULL ){
	if( copy_constructor != NULL || destructor != NULL ){
	  std::cerr << "ccgsl::siman<XP>::solve(): "
		    << "this function requires that either the dynamic functions (copy, "
		    << "copy_constructor and destructor) are all passed, or that none are."
		    <<  std::endl;
	  std::exit( 1 );
	} 
	element_size = sizeof( XP );
      }
      
      distance = 0 ; /* This parameter is not currently used */
      E = Ef(x0_p);

      if (copyfunc) {
	x = copy_constructor(x0_p);
	new_x = copy_constructor(x0_p);
	best_x = copy_constructor(x0_p);
      } else {
	x = (XP *) malloc (element_size);
	memcpy (x, reinterpret_cast<void*>( &x0_p ), element_size);
	new_x = (XP *) malloc (element_size);
	best_x =  (XP *) malloc (element_size);
	memcpy (best_x, reinterpret_cast<void*>( &x0_p ), element_size);
      }

      best_E = E;

      T = params.t_initial;
      T_factor = 1.0 / params.mu_t;

      if (print_position) {
	printf ("#-iter  #-evals   temperature     position   energy\n");
      }

      while (1) {

	n_accepts = 0;
	n_rejects = 0;
	n_eless = 0;

	for (i = 0; i < params.iters_fixed_T; ++i) {

	  copy_state(*x, *new_x, element_size, copyfunc);

	  take_step (r, *new_x, params.step_size);
	  new_E = Ef (*new_x);

	  if(new_E <= best_E){
	    if (copyfunc) {
	      copyfunc(*new_x,*best_x);
	    } else {
	      memcpy (best_x, new_x, element_size);
	    }
	    best_E=new_E;
	  }

	  ++n_evals;                /* keep track of Ef() evaluations */
	  /* now take the crucial step: see if the new point is accepted
	     or not, as determined by the boltzmann probability */
	  if (new_E < E) {

	    if (new_E < best_E) {
	      copy_state(*new_x, *best_x, element_size, copyfunc);
	      best_E = new_E;
	    }

	    /* yay! take a step */
	    copy_state(*new_x, *x, element_size, copyfunc);
	    E = new_E;
	    ++n_eless;

	  } else if (r.uniform() < boltzmann(E, new_E, T, params)) {
	    /* yay! take a step */
	    copy_state(*new_x, *x, element_size, copyfunc);
	    E = new_E;
	    ++n_accepts;

	  } else {
	    ++n_rejects;
	  }
	}

	if (print_position) {
	  /* see if we need to print stuff as we go */
	  /*       printf("%5d %12g %5d %3d %3d %3d", n_iter, T, n_evals, */
	  /*           100*n_eless/n_steps, 100*n_accepts/n_steps, */
	  /*           100*n_rejects/n_steps); */
	  printf ("%5d   %7d  %12g", n_iter, n_evals, T);
	  print_position (*x);
	  printf ("  %12g  %12g\n", E, best_E);
	}

	/* apply the cooling schedule to the temperature */
	/* FIXME: I should also introduce a cooling schedule for the iters */
	T *= T_factor;
	++n_iter;
	if (T < params.t_min) {
	  break;
	}
      }

      /* at the end, copy the result onto the initial point, so we pass it
	 back to the caller */
      copy_state(*best_x, x0_p, element_size, copyfunc);

      if (copyfunc) {
	destructor(x);
	destructor(new_x);
	destructor(best_x);
      } else {
	free (x);
	free (new_x);
	free (best_x);
      }
    }

    /**
     * C++ version of gsl_siman_solve_many(). Element_size must be the size of an object and
     * the object will be copied directly, which is unwise if a configuration object contains
     * pointers.
     * @param r A random number generator
     * @param x0_p The initial configuration (also final configuration on return)
     * @param Ef The Energy fiunction: gives energy of configuration
     * @param take_step The step function
     * @param distance The distance function
     * @param print_position Function to print position
     * @param params Parameters of the Simulated Annealing heuristic
     */
    void solve_many( rng& r, XP& x0_p, Efunc_t Ef, step_t take_step, metric_t distance,
		     print_t print_position, params_t params ){
      // This is adapted directly from the GSL. This isn't ideal. But it's the only
      // current way to allow us to pass suitable function pointers.

      // inline function adapted as local function
      struct {
	double operator()( double E, double new_E, double T, params_t& params ) const {
	  double x = -(new_E - E) / (params.k * T);
	  /* avoid underflow errors for large uphill steps */
	  return (x < GSL_LOG_DBL_MIN) ? 0.0 : exp(x);
	}
      } boltzmann;
      
      size_t const element_size = sizeof( XP );
      
      /* the new set of trial points, and their energies and probabilities */
      void *x, *new_x;
      double *energies, *probs, *sum_probs;
      double Ex;                    /* energy of the chosen point */
      double T, T_factor;           /* the temperature and a step multiplier */
      int i;
      double u;                     /* throw the die to choose a new "x" */
      int n_iter;
      
      if (print_position) {
	printf ("#-iter    temperature       position");
	printf ("         delta_pos        energy\n");
      }
      
      x = (void *) malloc (params.n_tries * element_size);
      new_x = (void *) malloc (params.n_tries * element_size);
      energies = (double *) malloc (params.n_tries * sizeof (double));
      probs = (double *) malloc (params.n_tries * sizeof (double));
      sum_probs = (double *) malloc (params.n_tries * sizeof (double));
      
      T = params.t_initial;
      T_factor = 1.0 / params.mu_t;

      memcpy (x, x0_p, element_size);

      n_iter = 0;
      while (1)
	{
	  Ex = Ef (x);
	  for (i = 0; i < params.n_tries - 1; ++i)
	    {                       /* only go to N_TRIES-2 */
	      /* center the new_x[] around x, then pass it to take_step() */
	      sum_probs[i] = 0;
	      memcpy ((char *)new_x + i * element_size, x, element_size);
	      take_step (r, (char *)new_x + i * element_size, params.step_size);
	      energies[i] = Ef ((char *)new_x + i * element_size);
	      probs[i] = boltzmann(Ex, energies[i], T, &params);
	    }
	  /* now add in the old value of "x", so it is a contendor */
	  memcpy ((char *)new_x + (params.n_tries - 1) * element_size, x, element_size);
	  energies[params.n_tries - 1] = Ex;
	  probs[params.n_tries - 1] = boltzmann(Ex, energies[i], T, &params);

	  /* now throw biased die to see which new_x[i] we choose */
	  sum_probs[0] = probs[0];
	  for (i = 1; i < params.n_tries; ++i)
	    {
	      sum_probs[i] = sum_probs[i - 1] + probs[i];
	    }
	  u = r.uniform () * sum_probs[params.n_tries - 1];
	  for (i = 0; i < params.n_tries; ++i)
	    {
	      if (u < sum_probs[i])
		{
		  memcpy (x, (char *) new_x + i * element_size, element_size);
		  break;
		}
	    }
	  if (print_position)
	    {
	      printf ("%5d\t%12g\t", n_iter, T);
	      print_position (x);
	      printf ("\t%12g\t%12g\n", distance (x, x0_p), Ex);
	    }
	  T *= T_factor;
	  ++n_iter;
	  if (T < params.t_min)
	    {
	      break;
	    }
	}

      /* now return the value via x0_p */
      memcpy (x0_p, x, element_size);

      /*  printf("the result is: %g (E=%g)\n", x, Ex); */

      free (x);
      free (new_x);
      free (energies);
      free (probs);
      free (sum_probs);
    }
  };
}
#endif
