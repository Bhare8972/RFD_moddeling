
#include "vector.h"

class functor_1D
{
	public:
	
	virtual double call(double)=0;
	
	gsl::vector call(const gsl::vector& data)
	{
		gsl::vector out(data.size());
		for(size_t i=0; i<data.size(); i++)
		{
			out[i]=call(data[i]);
		}
		return out;
	}
};

