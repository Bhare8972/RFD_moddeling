// poly.cpp : solution of cubic and quartic equation
// (c) Khashin S.I. http://math.ivanovo.ac.ru/dalgebra/Khashin/index.html
// khash2 (at) gmail.com
//
#include <math.h>

#ifndef SOLVE_POLYNOMIAL_HPP
#define SOLVE_POLYNOMIAL_HPP

//NOTE:: this code is HIGHLY unstable, and GSL has an implimentation anyway.
//Use GSL implimentation if necisary, or use root finding.  This code needs to be removed!

#define	TwoPi  6.28318530717958648
const double eps=1e-14;

//beware: that due to floating point arithmatic that sometimes a real root can show up as an complex root with small imaginary component

// x - array of size 4
// solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
// return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
// return 2: 2 real roots x[0], x[1] and complex x[2]�i*x[3],
// return 0: two pair of complex roots: x[0]�i*x[1],  x[2]�i*x[3],
int   SolveP4(double *x,double a,double b,double c,double d);

//---------------------------------------------------------------------------
// x - array of size 3
// In case 3 real roots: => x[0], x[1], x[2], return 3
//         2 real roots: x[0], x[1],          return 2
//         1 real root : x[0], x[1] � i*x[2], return 1
int SolveP3(double *x,double a,double b,double c)
{	// solve cubic equation x^3 + a*x^2 + b*x + c
	double a2 = a*a;
    double q  = (a2 - 3*b)/9;
	double r  = (a*(2*a2-9*b) + 27*c)/54;
    double r2 = r*r;
	double q3 = q*q*q;
	double A,B;
    if(r2<q3) {
        double t=r/std::sqrt(q3);
		if( t<-1) t=-1;
		if( t> 1) t= 1;
        t=acos(t);
        a/=3; q=-2*std::sqrt(q);
        x[0]=q*std::cos(t/3)-a;
        x[1]=q*std::cos((t+TwoPi)/3)-a;
        x[2]=q*std::cos((t-TwoPi)/3)-a;
        return(3);
    } else {
        A =-std::pow(std::fabs(r)+std::sqrt(r2-q3),1./3);
		if( r<0 ) A=-A;

		//B = A==0 ? 0 : B=q/A;
        if(A==0) { B=0;}
        else {B=q/A;}


		a/=3;
		x[0] =(A+B)-a;
        x[1] =-0.5*(A+B)-a;
        x[2] = 0.5*std::sqrt(3.)*(A-B);
		if(std::fabs(x[2])<eps) { x[2]=x[1]; return(2); }
        return(1);
    }
}// SolveP3(double *x,double a,double b,double c) {


//---------------------------------------------------------------------------
// a>=0!
void  CSqrt( double x, double y, double &a, double &b) // returns:  a+i*s = sqrt(x+i*y)
{
	double r  = std::sqrt(x*x+y*y);
	if( y==0 ) {
		r = std::sqrt(r);
		if(x>=0) { a=r; b=0; } else { a=0; b=r; }
	} else {		// y != 0
		a = std::sqrt(0.5*(x+r));
		b = 0.5*y/a;
	}
}


//---------------------------------------------------------------------------
int   SolveP4Bi(double *x, double b, double d)	// solve equation x^4 + b*x^2 + d = 0
{
	double D = b*b-4*d;
	if( D>=0 )
	{
		double sD = std::sqrt(D);
		double x1 = (-b+sD)/2;
		double x2 = (-b-sD)/2;	// x2 <= x1
		if( x2>=0 )				// 0 <= x2 <= x1, 4 real roots
		{
			double sx1 = std::sqrt(x1);
			double sx2 = std::sqrt(x2);
			x[0] = -sx1;
			x[1] =  sx1;
			x[2] = -sx2;
			x[3] =  sx2;
			return 4;
		}
		if( x1 < 0 )				// x2 <= x1 < 0, two pair of imaginary roots
		{
			double sx1 = std::sqrt(-x1);
			double sx2 = std::sqrt(-x2);
			x[0] =    0;
			x[1] =  sx1;
			x[2] =    0;
			x[3] =  sx2;
			return 0;
		}
		// now x2 < 0 <= x1 , two real roots and one pair of imginary root
			double sx1 = std::sqrt( x1);
			double sx2 = std::sqrt(-x2);
			x[0] = -sx1;
			x[1] =  sx1;
			x[2] =    0;
			x[3] =  sx2;
			return 2;
	} else { // if( D < 0 ), two pair of compex roots
		double sD2 = 0.5*std::sqrt(-D);
		CSqrt(-0.5*b, sD2, x[0],x[1]);
		CSqrt(-0.5*b,-sD2, x[2],x[3]);
		return 0;
	} // if( D>=0 )
} // SolveP4Bi(double *x, double b, double d)	// solve equation x^4 + b*x^2 d


//---------------------------------------------------------------------------
//#define SWAP(a,b) { t=b; b=a; a=t; }
static void  dblSort3( double & a, double &b, double &c) // make: a <= b <= c
{
	double t;
	if( a>b ) { t=b; b=a; a=t; }//(a,b);	// now a<=b
	if( c<b )
    {
		{ t=b; b=c; c=t; }//(b,c);			// now a<=b, b<=c
		if( a>b ) { t=b; b=a; a=t; }//(a,b);// now a<=b
	}
}

//---------------------------------------------------------------------------
int   SolveP4De(double *x, double b, double c, double d)	// solve equation x^4 + b*x^2 + c*x + d
{
	//if( c==0 ) return SolveP4Bi(x,b,d); // After that, c!=0
	if( std::fabs(c)<1e-14*(std::fabs(b)+std::fabs(d)) ) return SolveP4Bi(x,b,d); // After that, c!=0

	int res3 = SolveP3( x, 2*b, b*b-4*d, -c*c);	// solve resolvent
	// by Viet theorem:  x1*x2*x3=-c*c not equals to 0, so x1!=0, x2!=0, x3!=0
	if( res3>1 )	// 3 real roots,
	{
		dblSort3(x[0], x[1], x[2]);	// sort roots to x[0] <= x[1] <= x[2]
		// Note: x[0]*x[1]*x[2]= c*c > 0
		if( x[0] > 0) // all roots are positive
		{
			double sz1 = std::sqrt(x[0]);
			double sz2 = std::sqrt(x[1]);
			double sz3 = std::sqrt(x[2]);
			// Note: sz1*sz2*sz3= -c (and not equal to 0)
			if( c>0 )
			{
				x[0] = (-sz1 -sz2 -sz3)/2;
				x[1] = (-sz1 +sz2 +sz3)/2;
				x[2] = (+sz1 -sz2 +sz3)/2;
				x[3] = (+sz1 +sz2 -sz3)/2;
				return 4;
			}
			// now: c<0
			x[0] = (-sz1 -sz2 +sz3)/2;
			x[1] = (-sz1 +sz2 -sz3)/2;
			x[2] = (+sz1 -sz2 -sz3)/2;
			x[3] = (+sz1 +sz2 +sz3)/2;
			return 4;
		} // if( x[0] > 0) // all roots are positive
		// now x[0] <= x[1] < 0, x[2] > 0
		// two pair of comlex roots

		double sz1 = std::sqrt(-x[0]);
		double sz2 = std::sqrt(-x[1]);
		double sz3 = std::sqrt( x[2]);

		if( c>0 )	// sign = -1
		{
			x[0] = -sz3/2;
			x[1] = ( sz1 -sz2)/2;		// x[0]�i*x[1]
			x[2] =  sz3/2;
			x[3] = (-sz1 -sz2)/2;		// x[2]�i*x[3]
			return 0;
		}
		// now: c<0 , sign = +1
		x[0] =   sz3/2;
		x[1] = (-sz1 +sz2)/2;
		x[2] =  -sz3/2;
		x[3] = ( sz1 +sz2)/2;
		return 0;
	} // if( res3>1 )	// 3 real roots,
	// now resoventa have 1 real and pair of compex roots
	// x[0] - real root, and x[0]>0,
	// x[1]�i*x[2] - complex roots,
	double sz1 = std::sqrt(x[0]);
	double szr, szi;
	CSqrt(x[1], x[2], szr, szi);  // (szr+i*szi)^2 = x[1]+i*x[2]
	if( c>0 )	// sign = -1
	{
		x[0] = -sz1/2-szr;			// 1st real root
		x[1] = -sz1/2+szr;			// 2nd real root
		x[2] = sz1/2;
		x[3] = szi;
		return 2;
	}
	// now: c<0 , sign = +1
	x[0] = sz1/2-szr;			// 1st real root
	x[1] = sz1/2+szr;			// 2nd real root
	x[2] = -sz1/2;
	x[3] = szi;
	return 2;
} // SolveP4De(double *x, double b, double c, double d)	// solve equation x^4 + b*x^2 + c*x + d
//-----------------------------------------------------------------------------
double N4Step(double x, double a,double b,double c,double d)	// one Newton step for x^4 + a*x^3 + b*x^2 + c*x + d
{
	double fxs= ((4*x+3*a)*x+2*b)*x+c;	// f'(x)
	if( fxs==0 ) return 1e99;
	double fx = (((x+a)*x+b)*x+c)*x+d;	// f(x)
	return x - fx/fxs;
}

//-----------------------------------------------------------------------------
// x - array of size 4
// return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
// return 2: 2 real roots x[0], x[1] and complex x[2]�i*x[3],
// return 0: two pair of complex roots: x[0]�i*x[1],  x[2]�i*x[3],
int   SolveP4(double *x,double a,double b,double c,double d)
{	// solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
	// move to a=0:
	double d1 = d + 0.25*a*( 0.25*b*a - 3./64*a*a*a - c);
	double c1 = c + 0.5*a*(0.25*a*a - b);
	double b1 = b - 0.375*a*a;
	int res = SolveP4De( x, b1, c1, d1);

	if( res==4) { x[0]-= a/4; x[1]-= a/4; x[2]-= a/4; x[3]-= a/4; }
	else if (res==2) { x[0]-= a/4; x[1]-= a/4; x[2]-= a/4; }
	else             { x[0]-= a/4; x[2]-= a/4; }
	// one Newton step for each real root:
	if( res>0 )
	{
		x[0] = N4Step(x[0], a,b,c,d);
		x[1] = N4Step(x[1], a,b,c,d);
	}
	if( res>2 )
	{
		x[2] = N4Step(x[2], a,b,c,d);
		x[3] = N4Step(x[3], a,b,c,d);
	}
	return res;
}

/*
//-----------------------------------------------------------------------------
#define F5(t) (((((t+a)*t+b)*t+c)*t+d)*t+e)
//-----------------------------------------------------------------------------
double SolveP5_1(double a,double b,double c,double d,double e)	// return real root of x^5 + a*x^4 + b*x^3 + c*x^2 + d*x + e = 0
{
	int cnt;
	if( fabs(e)<eps ) return 0;

	double brd =  fabs(a);			// brd - border of real roots
	if( fabs(b)>brd ) brd = fabs(b);
	if( fabs(c)>brd ) brd = fabs(c);
	if( fabs(d)>brd ) brd = fabs(d);
	if( fabs(e)>brd ) brd = fabs(e);
	brd++;							// brd - border of real roots

	double x0, f0;					// less, than root
	double x1, f1;					// greater, than root
	double x2, f2, f2s;				// next values, f(x2), f'(x2)
	double dx;

	if( e<0 ) { x0 =   0; x1 = brd; f0=e; f1=F5(x1); x2 = 0.01*brd; }
	else	  { x0 =-brd; x1 =   0; f0=F5(x0); f1=e; x2 =-0.01*brd; }

	if( fabs(f0)<eps ) return x0;
	if( fabs(f1)<eps ) return x1;

	// now x0<x1, f(x0)<0, f(x1)>0
	// Firstly 5 bisections
	for( cnt=0; cnt<5; cnt++)
	{
		x2 = (x0 + x1)/2;			// next point
		f2 = F5(x2);				// f(x2)
		if( fabs(f2)<eps ) return x2;
		if( f2>0 ) { x1=x2; f1=f2; }
		else       { x0=x2; f0=f2; }
	}

	// At each step:
	// x0<x1, f(x0)<0, f(x1)>0.
	// x2 - next value
	// we hope that x0 < x2 < x1, but not necessarily
	do {
		cnt++;
		if( x2<=x0 || x2>= x1 ) x2 = (x0 + x1)/2;	// now  x0 < x2 < x1
		f2 = F5(x2);								// f(x2)
		if( fabs(f2)<eps ) return x2;
		if( f2>0 ) { x1=x2; f1=f2; }
		else       { x0=x2; f0=f2; }
		f2s= (((5*x2+4*a)*x2+3*b)*x2+2*c)*x2+d;		// f'(x2)
		if( fabs(f2s)<eps ) { x2=1e99; continue; }
		dx = f2/f2s;
		x2 -= dx;
	} while(fabs(dx)>eps);
	return x2;
} // SolveP5_1(double a,double b,double c,double d,double e)	// return real root of x^5 + a*x^4 + b*x^3 + c*x^2 + d*x + e = 0
//-----------------------------------------------------------------------------
int   SolveP5(double *x,double a,double b,double c,double d,double e)	// solve equation x^5 + a*x^4 + b*x^3 + c*x^2 + d*x + e = 0
{
	double r = x[0] = SolveP5_1(a,b,c,d,e);
	double a1 = a+r, b1=b+r*a1, c1=c+r*b1, d1=d+r*c1;
	return 1+SolveP4(x+1, a1,b1,c1,d1);
} // SolveP5(double *x,double a,double b,double c,double d,double e)	// solve equation x^5 + a*x^4 + b*x^3 + c*x^2 + d*x + e = 0
//-----------------------------------------------------------------------------
// let f(x ) = a*x^2 + b*x + c and
//     f(x0) = f0,
//     f(x1) = f1,
//     f(x2) = f3
// Then r1, r2 - root of f(x)=0.
// Returns 0, if there are no roots, else return 2.
int Solve2( double x0, double x1, double x2, double f0, double f1, double f2, double &r1, double &r2)
{
	double w0 = f0*(x1-x2);
	double w1 = f1*(x2-x0);
	double w2 = f2*(x0-x1);
	double a1 =  w0         + w1         + w2;
	double b1 = -w0*(x1+x2) - w1*(x2+x0) - w2*(x0+x1);
	double c1 =  w0*x1*x2   + w1*x2*x0   + w2*x0*x1;
	double Di =  b1*b1 - 4*a1*c1;	// must be>0!
	if( Di<0 ) { r1=r2=1e99; return 0; }
	Di =  sqrt(Di);
	r1 =  (-b1 + Di)/2/a1;
	r2 =  (-b1 - Di)/2/a1;
	return 2;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
*/
#endif // SOLVE_POLYNOMIAL_HPP
