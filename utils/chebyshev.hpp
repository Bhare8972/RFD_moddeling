
#ifndef CHEBYSHEV_HPP
#define CHEBYSHEV_HPP

//this file contains a number of algorithms related to chebyshev splines
#include <cmath>
#include <memory>
#include <list>
#include <vector>

#include "constants.hpp"
//#include "GSL_utils.hpp"
#include "vector.hpp"
#include "vector_long.hpp"
//#include "functor.hpp"
#include "gen_ex.hpp"
#include "spline.hpp"

#include "solve_polynomial.hpp"


namespace cheby_tables
{
    //note that this has an odd order
    double U3_i[]={1.0, -1.0, std::cos(PI/3.0), std::cos(2*PI/3.0)};
    
    double F3_ij[][]={{0.5, 0.5, 1, 1},
                     {0.5, -0.5, std::cos(PI/3.0), std::cos(2*PI/3.0)},
                     {0.5, 0.5, std::cos(2*PI/3.0), std::cos(4*PI/3.0)},
                     {0.5, -0.5, -1.0, 1.0 )} };
                     
                     
    
    double U4_i[]={1.0, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75), -1.0};
    
    double F4_ij[][]={{0.5, 1, 1, 1, -0.5},
                     {0.5, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75), -0.5},
                     {0.5, std::cos(PI*0.5), std::cos(PI), std::cos(PI*3.0/2.0), 0.5},
                     {0.5, std::cos(PI*0.75), std::cos(PI*1.5), std::cos(PI*9.0/4.0), -0.5},
                     {0.5, std::cos(PI), std::cos(PI*2.0), std::cos(PI*3.0), 0.5} };
};

class AdaptiveSpline_Cheby_O3
//adaptively sample a 1D function using chebyshev polys
// order 3 is useful, because integration produces order 4, which is the largest order that can be inverted
{
public:
    //useful tables for third order cheby spline.

    class sampler_helper
    {
    public:
        int stage;//0 means good, but does not have subsections, 1 has subsections. 2 is in error, 3 tried subsections, but lower subsections had error, 4 one of the recursivly lower subsections is a 3
        
        double Xhigh;
        double Xlow;
        double X1;
        double X2;

        double Yhigh;
        double Ylow;
        double Y1;
        double Y2;

        double K0;
        double K1;
        double K2;
        double K3;

        sampler_helper* left_spline;
        sampler_helper* middle_spline;
        sampler_helper* right_spline;

        template<typename functor_T>
        sampler_helper(functor_T& func, double Xlower, double Xupper, double Ylower, double Yupper)
        {
            left_spline=0;
            middle_spline=0;
            right_spline=0;
            stage=0;
            
            Xhigh=Xupper;
            Xlow=Xlower;

            Yhigh=Yupper;
            Ylow=Ylower;

            //sample at two new locations
            double A=(Xupper-Xlower);
            double B=(Xupper+Xlower);
            X1=(cheby_tables::U3_i[2]*A + B)*0.5;
            X2=(cheby_tables::U3_i[3]*A + B)*0.5;
            
            //stop if there is an error
            if(float(Xhigh+(Xhigh-X2))==float(Xhigh))
            {
                stage=2;
                return;
            }
            if(float(X2+(X2-X1))==float(X2))
            {
                stage=2;
                return;
            }
            if(float(X1+(X1-Xlow))==float(X1))
            {
                stage=2;
                return;
            }

            Y1=func(X1);
            Y2=func(X2);

            //find coefienents
            double C0=(cheby_tables::F3_ij[0][0]*Yhigh + cheby_tables::F3_ij[0][1]*Ylow + cheby_tables::F3_ij[0][2]*Y1 + cheby_tables::F3_ij[0][3]*Y2)*2.0/3.0;
            double C1=(cheby_tables::F3_ij[1][0]*Yhigh + cheby_tables::F3_ij[1][1]*Ylow + cheby_tables::F3_ij[1][2]*Y1 + cheby_tables::F3_ij[1][3]*Y2)*2.0/3.0;
            double C2=(cheby_tables::F3_ij[2][0]*Yhigh + cheby_tables::F3_ij[2][1]*Ylow + cheby_tables::F3_ij[2][2]*Y1 + cheby_tables::F3_ij[2][3]*Y2)*2.0/3.0;
            double C3=(cheby_tables::F3_ij[3][0]*Yhigh + cheby_tables::F3_ij[3][1]*Ylow + cheby_tables::F3_ij[3][2]*Y1 + cheby_tables::F3_ij[3][3]*Y2)*2.0/3.0;

            double F=B/A;
            double F2=F*F;
            double F3=F2*F;
            K0=C0*0.5 - C2 - F*(C1-3*C3) + 2*C2*F2 - 4*C3*F3;

            F2=F/B;
            F3=F2*F
            K1=2*(C1-3*C3)/B - 8*C2*F2 + 24*C3*F3;

            F2=1/(B*B);
            F3=F2*F;
            K2=8*C2*F2 - 40*C3*F3;

            K3=32*C3*F2/B;
        }

        ~sampler_helper()
        {
            if(left_spline){delete left_spline};
            if(middle_spline){delete middle_spline};
            if(right_spline){delete right_spline};
        }

        double sample(double X)
        {
            double F2=X*X;
            double F3=F2*X;
            return K0 + K1*X + K2*F2 + K3*F3;
        }

        template<typename functor_T>
        void refine(functor_T& func, double precision_factor)
        {
            //step one, make lower subsections
            left_spline = new sampler_helper(func, Xlow, X1, Ylow, Y1);
            middle_spline = new sampler_helper(func, X1, X2, Y1, Y2);
            right_spline = new sampler_helper(func, X2, Xhigh, Y2, Yhigh);
            
            //check for error
            if(left_spline->stage == 2 or middle_spline->stage==2 or right_spline->stage==2)
            {
                stage=3;
                delete left_spline;
                delete right_spline;
                delete middle_spline;
                return;
            }
            stage=1;
            
            //check to see if precision is reached
            double guess=sample(left_spline->X1);
            bool left_accurate=float(guess*precision_factor + float( guess - (left_spline->Y1))) == float(guess*precision_factor);
            
            guess=sample(middle_spline->X1);
            bool middle_accurate=float(guess*precision_factor + float( guess - (middle_spline->Y1))) == float(guess*precision_factor);
            
            guess=sample(right_spline->X1);
            bool right_accurate=float(guess*precision_factor + float( guess - (right_spline->Y1))) == float(guess*precision_factor);
            
            if( (not left_accurate) or (not middle_accurate) or (not right_accurate))
            {
                left_spline->refine(func, precision_factor);
                middle_spline->refine(func, precision_factor);
                right_spline->refine(func, precision_factor);
                
                if( right_spline->stage==3 or right_spline->stage==4 or middle_spline->stage==3 or middle_spline->stage==4 or left_spline->stage==3 or left_spline->stage==4)
                {
                    stage==4;
                }
            }
        }
        
        void get_points(list<double>& out_points)
        {
            if(stage!=0)
            {
                left_spline->get_points(out_points);
                middle_spline->get_points(out_points);
                right_spline->get_points(out_points);
            }
            else
            {
                out_points.push_back(Xlow);
                out_points.push_back(X1);
                out_points.push_back(X2);
            }
        }
        
        void get_values(list<double>& out_values)
        {
            if(stage!=0)
            {
                left_spline->get_values(out_values);
                middle_spline->get_values(out_values);
                right_spline->get_values(out_values);
            }
            else
            {
                out_values.push_back(Ylow);
                out_values.push_back(Y1);
                out_values.push_back(Y2);
            }
        }
        
        void get_sorted(list<sampler_helper*>& out_samplers)
        {
            if(stage!=0)
            {
                left_spline->get_sorted(out_samplers);
                middle_spline->get_sorted(out_samplers);
                right_spline->get_sorted(out_samplers);
            }
            else
            {
                out_samplers.push_back(this);
            }
        }
    };
    
    class fourth_order_weights
    {
    public:
        double W0;
        double W1;
        double W2;
        double W3;
        double W4;
        
        fourth_order_weights(double _W0, double _W1, double _W2, double _W3, double _W4)
        {
            W0=_W0;
            W1=_W1;
            W2=_W2;
            W3=_W3;
            W4=_W4;
        }
    };
    
    class rand_sampler
    {
        public:
        std::vector<fourth_order_weights>  splines;
        gsl::vector_long aliases;
        gsl::vector alias_probabilities;
        
        rand_sampler(std::vector<fourth_order_weights>& _splines, gsl::vector weights, double& out_rate)
        {
            splines.swap(_splines);
            
            out_rate=weights.sum();
            
            FANCY ALGORITHM HERE
        }
    };
    
    sampler_helper* top_section;

    template<typename functor_T>
    AdaptiveSpline_Cheby_O3(functor_T& func, double precision_factor, double Xlower, double Xupper)
    {
        double Ylow=func(Xlower);
        double Yhigh=func(Xupper);
        top_section = new sampler_helper(func, Ylow, Yhigh);
    }
    
    ~AdaptiveSpline_Cheby_O3()
    {
        if(top_section){delete top_section;}
    }
    
    gsl::vector get_points()
    {
        std::list<double> points;
        top_section->get_points(points);
        points.push_back(top_section->Xhigh);
        return gsl::vector(points);
    }
    
    gsl::vector get_values()
    {
        std::list<double> values;
        top_section->get_values(values);
        values.push_back(top_section->Yhigh);
        return gsl::vector(values);
    }
    
    std::shared_ptr<poly_spline> get_spline()
    {
        gsl::vector x_vals=get_points();
        std::vector<spline_piece> splines;
        splines.reserve(x_vals.size()-1);
        
        std::list<sampler_helper*> samplers;
        top_section->get_sorted(samplers);
        
        for(auto S : samplers)
        {
            gsl::vector W({S->K0, S->K1, S->K2, S->K3});
            splines.emplace_back(W);
        }
        
        std::shared_ptr<poly_spline> ret=std::make_shared(poly_spline);
        ret->x_vals=x_vals;
        ret->splines=splines;
        return ret;
    }
    
    rand_sampler inverse_transform(double inverse_precision, double& rate_out)
    //POWER!!! UNLIMITED POWER!!!
    {
        class invert_quartic_spline
        {
        public:
            double W0;
            double W1;
            double W2;
            double W3;
            double W4;
            
            //assume that W1, W2, W3, and W4 is set
            
            //stuff
            double Xlow;
            double Xhigh;
            double rate;
            
            double set(double _Xlow, double _Xhigh)
            //return rate
            {
                Xlow=_Xlow;
                Xhigh=_Xhigh;
                
                double X2=Xlow*Xlow;
                double X3=X2*Xlow;
                double X4=X3*Xlow;
                W0=-(W1*Xlow + W2*X2 + W3*X3 + W4*X4);
                
                X2=Xhigh*Xhigh;
                X3=X2*Xhigh;
                X4=X3*Xhigh;
                rate=W0 + W1*Xhigh + W2*X2 + W3*X3 + W4*X4;
                
                //we can do some fancy stuff here to make next function faster (Which needs to be done)
            }
            
            double invert(double N)
            //N is from 0 to 1
            //this can be improved significantly
            {
                // x - array of size 4
                // solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
                // return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
                // return 2: 2 real roots x[0], x[1] and complex x[2]±i*x[3],
                // return 0: two pair of complex roots: x[0]±i*x[1],  x[2]±i*x[3]
                
                int sol[4];
                int ret = SolveP4(sol, W3/W4, W2/W4, W1/W4, (W0-N*rate)/W4); //much of this can be pre-calculated
                
                if(sol[0]>=Xlow and sol[0]<=Xhigh)
                {
                    return sol[0];
                }
                else if(sol[2]>=Xlow and sol[2]<=Xhigh)
                {
                    return sol[2];
                }
                else if(sol[1]>=Xlow and sol[1]<=Xhigh and ret!=0)
                {
                    return sol[1];
                }
                else if(sol[3]>=Xlow and sol[3]<=Xhigh and ret==4)
                {
                    return sol[3];
                }
                else
                {
                    throw gen_exception("polynomial cannot be inverted");
                }
            }
        };
        
        gsl::vector x_vals=get_points();
        std::list<sampler_helper*> samplers;
        top_section->get_sorted(samplers);
        
        std::vector<fourth_order_weights>  inverted_splines;
        inverted_splines.reserve(x_vals.size()-1);
        gsl::vector spline_weights(x_vals.size()-1);
        
        int i=0;
        for(auto SH : samplers)
        {
            invert_quartic_spline inverter;
            
            inverter.W1=SH->K0;
            inverter.W2=SH->K1*0.5;
            inverter.W3=SH->K2/3.0;
            inverter.W4=SH->K3*0.25;
            
            spline_weights[i] = inverter.set(SH->Xlow, SH->Xhigh );
            
            double Y0=inverter.invert();
            
            
            i++;
        }
    }

};


//should write a 4th order adaptive sampler. Which could use the 2nd order to estimate error??

#endif // CHEBYSHEV_HPP



















