#ifndef RELATIVISTIC_FORMULAS
#define RELATIVISTIC_FORMULAS

//////// usefull converstion functions //////////
// dimensionless units unless specificied otherwise
double KE_to_mom(double KE)
//both KE and momentum are unitless
{
	return sqrt(pow(1+KE , 2.0) - 1.0);
}

double mom_to_KE(gsl::vector mom)
//both KE and momentum are unitless
{
    return sqrt(mom.sum_of_squares()+1.0)-1.0;
}

double gamma(gsl::vector &mom_)
{
    return sqrt(1+mom_[0]*mom_[0]+mom_[1]*mom_[1]+mom_[2]*mom_[2]);
}

double gamma(double momentum_squared_)
{
    return sqrt(1+momentum_squared_);
}

#endif
