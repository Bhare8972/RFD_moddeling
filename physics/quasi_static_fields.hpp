
#ifndef QUASI_STATIC_FIELDS
#define QUASI_STATIC_FIELDS
/////// tools for representing quasi-static magnetic and electric fields /////////

#include <cmath>

#include "vector.hpp"

class field
{
	public:
	virtual gsl::vector get(gsl::vector &position, double time)=0;//in case we want time dependance later
	virtual gsl::vector get(gsl::vector &position)=0;
	field* pntr(){ return this;}
};

class uniform_field : public field
{
public:
	gsl::vector minimum;
	gsl::vector maximum;
	gsl::vector value;

	uniform_field()
	{
		minimum=gsl::vector({0,0,0});
		maximum=gsl::vector({0,0,0});
		value=gsl::vector({0,0,0});
	}

	void set_minimum(double X, double Y, double Z)
	{
		minimum[0]=X;
		minimum[1]=Y;
		minimum[2]=Z;
	}

	void set_maximum(double X, double Y, double Z)
	{
		maximum[0]=X;
		maximum[1]=Y;
		maximum[2]=Z;
	}

	void set_value(double X, double Y, double Z)
	{
		value[0]=X;
		value[1]=Y;
		value[2]=Z;
	}

	gsl::vector get(gsl::vector &position)
	{
		if(position.vec_greaterThan(minimum).all_true() and position.vec_lessThan(maximum).all_true())
		{
			return value;
		}
		else
		{
			return gsl::vector({0,0,0});
		}
	}


	gsl::vector get(gsl::vector &position, double time)
	{
		return get(position);
	}
};

#endif
