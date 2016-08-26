
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
    double U3_i[4]={1.0, std::cos(PI/3.0), std::cos(2*PI/3.0), -1.0};

    double F3_ij[4][4]={{0.5, 1, 1, 0.5},
                      {0.5, std::cos(PI/3.0), std::cos(2*PI/3.0), -0.5},
                      {0.5, std::cos(2*PI/3.0), std::cos(4*PI/3.0), 0.5},
                      {0.5, -1.0, 1.0, -0.5 } };



    double U4_i[5]={1.0, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75), -1.0};

    double F4_ij[5][5]={{0.5, 1,                 1,                1,                    0.5},
                        {0.5, std::cos(PI*0.25), std::cos(PI*0.5), std::cos(PI*0.75),    -0.5},
                        {0.5, std::cos(PI*0.5),  std::cos(PI),     std::cos(PI*3.0/2.0), 0.5},
                        {0.5, std::cos(PI*0.75), std::cos(PI*1.5), std::cos(PI*9.0/4.0), -0.5},
                        {0.5, std::cos(PI),      std::cos(PI*2.0), std::cos(PI*3.0),     0.5} };
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
        double X1;
        double X2;
        double Xlow;

        double Yhigh;
        double Y1;
        double Y2;
        double Ylow;

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
            X1=(cheby_tables::U3_i[1]*A + B)*0.5;
            X2=(cheby_tables::U3_i[2]*A + B)*0.5;

            //stop if there is an error
            if(float(Xhigh+(Xhigh-X1))==float(Xhigh))
            {
                stage=2;
                return;
            }
            if(float(X2+(X2-X1))==float(X2))
            {
                stage=2;
                return;
            }
            if(float(X2+(X2-Xlow))==float(X2))
            {
                stage=2;
                return;
            }

            Y1=func(X1);
            Y2=func(X2);

            //find coefienents
            double C0=(cheby_tables::F3_ij[0][0]*Yhigh + cheby_tables::F3_ij[0][1]*Y1 + cheby_tables::F3_ij[0][2]*Y2 + cheby_tables::F3_ij[0][3]*Ylow)*2.0/3.0;
            double C1=(cheby_tables::F3_ij[1][0]*Yhigh + cheby_tables::F3_ij[1][1]*Y1 + cheby_tables::F3_ij[1][2]*Y2 + cheby_tables::F3_ij[1][3]*Ylow)*2.0/3.0;
            double C2=(cheby_tables::F3_ij[2][0]*Yhigh + cheby_tables::F3_ij[2][1]*Y1 + cheby_tables::F3_ij[2][2]*Y2 + cheby_tables::F3_ij[2][3]*Ylow)*2.0/3.0;
            double C3=(cheby_tables::F3_ij[3][0]*Yhigh + cheby_tables::F3_ij[3][1]*Y1 + cheby_tables::F3_ij[3][2]*Y2 + cheby_tables::F3_ij[3][3]*Ylow)*2.0/3.0;

            double F=B/A;
            double F2=F*F;
            double F3=F2*F;
            K0=C0*0.5 - C2 - F*(C1-3*C3) + 2*C2*F2 - 4*C3*F3;

            F2=F/A;
            F3=F2*F;
            K1=2*(C1-3*C3)/A - 8*C2*F2 + 24*C3*F3;

            F2=1/(A*A);
            F3=F2*F;
            K2=8*C2*F2 - 48*C3*F3;

            K3=32*C3*F2/A;
        }

        ~sampler_helper()
        {
            if(left_spline){delete left_spline;}
            if(middle_spline){delete middle_spline;}
            if(right_spline){delete right_spline;}
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
            left_spline = new sampler_helper(func, Xlow, X2, Ylow, Y2);
            middle_spline = new sampler_helper(func, X2, X1, Y2, Y1);
            right_spline = new sampler_helper(func, X1, Xhigh, Y1, Yhigh);

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
//return;

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
                    stage=4;
                }
            }
        }

        void get_points(std::list<double>& out_points)
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
                out_points.push_back(X2);
                out_points.push_back(X1);
            }
        }

        void get_values(std::list<double>& out_values)
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

        void get_sorted(std::list<sampler_helper*>& out_samplers)
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
    
    class quartic_inversion_helper
    {
    public:
        double W1;
        double W2;
        double W3;
        double W4;
        //assume that W1, W2, W3, and W4 is set

        //stuff
        double W0;
        double Xlow;
        double Xhigh;
        double rate;

        double set(double _Xlow, double _Xhigh)
        //return rate
        //assume that W1, W2, W3, and W4 is set
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

            return rate;
        }
        
        double invert(double N)
        {
            if(float(W1+W4)==float(W1))
            {
                return invert3(N);
            }
            else
            {
                return invert4(N);
            }
        }

        double invert3(double N)
        //N is from 0 to 1
        //this can be improved significantly
        {
            
            // solve cubic equation x^3 + a*x^2 + b*x + c
            // x - array of size 3
            // In case 3 real roots: => x[0], x[1], x[2], return 3
            //         2 real roots: x[0], x[1],          return 2
            //         1 real root : x[0], x[1] ± i*x[2], return 1
            double sol[3];
            int ret= SolveP3(sol, W2/W3, W1/W3, (W0-N*rate)/W3);


            if( (sol[0]>=Xlow and sol[0]<=Xhigh) or float(Xhigh + float(Xlow-sol[0]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[0]))==float(Xhigh) )
            {
                return sol[0];
            }
            else if((sol[1]>=Xlow and sol[1]<=Xhigh) or float(Xhigh + float(Xlow-sol[1]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[1]))==float(Xhigh) )
            {
                return sol[1];
            }
            else if(ret==3 and ( (sol[2]>=Xlow and sol[2]<=Xhigh) or float(Xhigh + float(Xlow-sol[2]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[2]))==float(Xhigh) ) )
            {
                return sol[2];
            }
            else
            {
                throw gen_exception("3rd O polynomial cannot be inverted");
            }
        }

        double invert4(double N)
        //N is from 0 to 1
        //this can be improved significantly
        {
            // x - array of size 4
            // solve equation x^4 + a*x^3 + b*x^2 + c*x + d by Dekart-Euler method
            // return 4: 4 real roots x[0], x[1], x[2], x[3], possible multiple roots
            // return 2: 2 real roots x[0], x[1] and complex x[2]±i*x[3],
            // return 0: two pair of complex roots: x[0]±i*x[1],  x[2]±i*x[3]

            double sol[4];
            int ret = SolveP4(sol, W3/W4, W2/W4, W1/W4, (W0-N*rate)/W4); //much of this can be pre-calculated


            if( (sol[0]>=Xlow and sol[0]<=Xhigh) or float(Xhigh + float(Xlow-sol[0]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[0]))==float(Xhigh) )
            {
                return sol[0];
            }
            else if((sol[2]>=Xlow and sol[2]<=Xhigh) or float(Xhigh + float(Xlow-sol[2]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[2]))==float(Xhigh) )
            {
                return sol[2];
            }
            else if(ret!=0 and ( (sol[1]>=Xlow and sol[1]<=Xhigh) or float(Xhigh + float(Xlow-sol[1]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[1]))==float(Xhigh) ) )
            {
                return sol[1];
            }
            else if(ret==4 and ((sol[3]>=Xlow and sol[3]<=Xhigh) or float(Xhigh + float(Xlow-sol[3]))==float(Xhigh) or float(Xhigh + float(Xhigh-sol[3]))==float(Xhigh) ) )
            {
                return sol[3];
            }
            else
            {
                print(sol[0], sol[1], sol[2], sol[3], Xlow, Xhigh);
                throw gen_exception("4th O polynomial cannot be inverted");
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
        private:
        class alias_data //needed for initializing the sampler
        {
        public:
            size_t index;
            double size;
        };

        static bool alias_increasing_compare(const alias_data& LHS, const alias_data& RHS)
        {
            return LHS.size<RHS.size;
        }

        static bool alias_decreasing_compare(const alias_data& LHS, const alias_data& RHS)
        {
            return LHS.size>RHS.size;
        }

        public:
        std::vector<fourth_order_weights>  splines;
        gsl::vector_long aliases;
        gsl::vector alias_probabilities;
        
        rand_sampler(){}

        rand_sampler(std::vector<fourth_order_weights>& _splines, gsl::vector weights, double& out_rate)
        {
            splines.swap(_splines);

            out_rate=weights.sum();
            weights/=out_rate;

            //setup walker aliasing
            aliases = gsl::vector_long(splines.size());
            alias_probabilities = gsl::vector(splines.size());

            std::list<alias_data> too_low;
            std::list<alias_data> too_high;

            for(int spline_i=0; spline_i<splines.size(); spline_i++)
            {
                //make alias_data, which is just needed for this algorithm
                alias_data new_data;
                new_data.index=spline_i;
                new_data.size=(weights[spline_i])*splines.size();

                //initialize the alias data most of this will change
                aliases[spline_i]=spline_i;
                alias_probabilities[spline_i]=new_data.size;
                //alias_boundAdjust[spline_i]=1.0;

                if( std::abs(1.0-new_data.size) < 1.0E-10  )
                {
                    //then new_data.size is approxamently 1.0
                    continue;
                }
                else if(new_data.size < 1.0)
                {
                    too_low.push_back(new_data);
                }
                else
                {
                    too_high.push_back(new_data);
                }
            }

            //sort the new lists
            too_low.sort(alias_increasing_compare);
            too_high.sort(alias_decreasing_compare);

            //generate alias data!
            auto low_iter=too_low.begin();
            auto high_iter=too_high.begin();

            while( low_iter != too_low.end() and high_iter!=too_high.end())
            {
                while(true)
                {
                    aliases[low_iter->index]=high_iter->index;
                    alias_probabilities[low_iter->index]=low_iter->size;

                    //alias_boundAdjust[low_iter->index]=spline_sampler.x_vals[high_iter->index] + (high_iter->size-1.0)/double(aliases.size());//

                    high_iter->size+=low_iter->size-1.0;

                    ++low_iter;
                    if(high_iter->size<0)
                    {
                        print("WALKER ALIAS ALGORITHM ERROR 2");
                    }
                    else if( std::abs(1.0-high_iter->size) < 1.0E-10  )
                    {
                        //just right height!
                        break;
                    }
                    else if(high_iter->size > 1.0)
                    {
                        //still too high
                        continue;
                    }
                    else
                    {
                        //now we are too low!!
                        too_low.push_back(*high_iter);
                        if(low_iter==too_low.end())
                        {
                            --low_iter;
                        }
                        break;
                    }
                }
                ++high_iter;
            }

            if(high_iter != too_high.end() or low_iter!=too_low.end())
            {
                print("WALKER ALIAS ALGORITHM ERROR 1");
            }
        }

        double sample(double uniform_rand)
        {
            uniform_rand*=aliases.size();
            int index=int(uniform_rand);
            double remainder=uniform_rand-index;

            /*
            double ret=-1;
            if(remainder<alias_probabilities[index])
            {
                //ret= spline_sampler.splines[index].y( spline_sampler.x_vals[index] + remainder/aliases.size() );
                ret= spline_sampler.splines[index].y( spline_sampler.x_vals[index] + remainder*(spline_sampler.x_vals[index+1]-spline_sampler.x_vals[index]) );
            }
            else
            {
                int alias_index=aliases[index];
                //ret=spline_sampler.splines[alias_index].y( alias_boundAdjust[index] + remainder/aliases.size() );
                ret=spline_sampler.splines[alias_index].y( spline_sampler.x_vals[alias_index] + remainder*(spline_sampler.x_vals[alias_index+1]-spline_sampler.x_vals[alias_index]) );
            }*/

            if(remainder>=alias_probabilities[index])
            {
                index=aliases[index];
            }

            double R2=remainder*remainder;
            double R3=R2*remainder;
            double R4=R3*remainder;

            return splines[index].W0 + splines[index].W1*remainder + splines[index].W2*R2 + splines[index].W3*R3 + splines[index].W4*R4;
        }


    };

    sampler_helper* top_section;

    template<typename functor_T>
    AdaptiveSpline_Cheby_O3(functor_T& func, double precision_factor, double Xlower, double Xupper)
    {
        double Ylow=func(Xlower);
        double Yhigh=func(Xupper);
        top_section = new sampler_helper(func, Xlower, Xupper, Ylow, Yhigh );

        top_section->refine(func, precision_factor);
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
        return make_vector(points);
    }

    gsl::vector get_values()
    {
        std::list<double> values;
        top_section->get_values(values);
        values.push_back(top_section->Yhigh);
        return make_vector(values);
    }

    std::shared_ptr<poly_spline> get_spline()
    {
        std::list<sampler_helper*> samplers;
        top_section->get_sorted(samplers);


        gsl::vector x_vals(samplers.size()+1);
        std::vector<poly_spline::spline_piece> splines;
        splines.reserve(samplers.size());

        int i=0;
        for(auto S : samplers)
        {
            x_vals[i]=S->Xlow;
            gsl::vector W({S->K0, S->K1, S->K2, S->K3});
            splines.emplace_back(W);
            i++;
        }
        x_vals[i]=samplers.back()->Xhigh;

        auto ret=std::make_shared<poly_spline>();
        ret->x_vals=x_vals;
        ret->splines=splines;
        ret->lower_fill=std::nan("");
        ret->upper_fill=std::nan("");
        return ret;
    }
    
    std::shared_ptr<poly_spline> get_inverse_spline(double inverse_precision)//, gsl::vector& inverse_X_samples, gsl::vector& inverse_Y_samples)
    {
        std::list<sampler_helper*> samplers;
        top_section->get_sorted(samplers);
        
        gsl::vector x_vals(samplers.size()+1);
        std::vector<poly_spline::spline_piece> splines;
        splines.reserve(samplers.size());
        
        //inverse_X_samples=gsl::vector(samplers.size()*5); //these were for testing purposes
        //inverse_Y_samples=gsl::vector(samplers.size()*5);

        int i=0;
        double spline_low=0;
        for(auto SH : samplers)
        {
            quartic_inversion_helper inverter; 

            inverter.W1=SH->K0;
            inverter.W2=SH->K1*0.5;
            inverter.W3=SH->K2/3.0;
            inverter.W4=SH->K3*0.25;
            
            double spline_width = inverter.set(SH->Xlow, SH->Xhigh );

            double Y0=inverter.invert((cheby_tables::U4_i[0]+1)*0.5);
            double Y1=inverter.invert((cheby_tables::U4_i[1]+1)*0.5);
            double Y2=inverter.invert((cheby_tables::U4_i[2]+1)*0.5);
            double Y3=inverter.invert((cheby_tables::U4_i[3]+1)*0.5);
            double Y4=inverter.invert((cheby_tables::U4_i[4]+1)*0.5);
            
            /*
            inverse_X_samples[i*5+4]= (cheby_tables::U4_i[0]+1)*0.5*spline_width + spline_low;
            inverse_X_samples[i*5+3]= (cheby_tables::U4_i[1]+1)*0.5*spline_width + spline_low;
            inverse_X_samples[i*5+2]= (cheby_tables::U4_i[2]+1)*0.5*spline_width + spline_low;
            inverse_X_samples[i*5+1]= (cheby_tables::U4_i[3]+1)*0.5*spline_width + spline_low;
            inverse_X_samples[i*5]= (cheby_tables::U4_i[4]+1)*0.5*spline_width + spline_low;
            
            
            inverse_Y_samples[i*5+4]=Y0;
            inverse_Y_samples[i*5+3]=Y1;
            inverse_Y_samples[i*5+2]=Y2;
            inverse_Y_samples[i*5+1]=Y3;
            inverse_Y_samples[i*5]=Y4;
            */
            
            double C0=Y0*cheby_tables::F4_ij[0][0] + Y1*cheby_tables::F4_ij[0][1]  + Y2*cheby_tables::F4_ij[0][2]  + Y3*cheby_tables::F4_ij[0][3]  + Y4*cheby_tables::F4_ij[0][4];
            double C1=Y0*cheby_tables::F4_ij[1][0] + Y1*cheby_tables::F4_ij[1][1]  + Y2*cheby_tables::F4_ij[1][2]  + Y3*cheby_tables::F4_ij[1][3]  + Y4*cheby_tables::F4_ij[1][4];
            double C2=Y0*cheby_tables::F4_ij[2][0] + Y1*cheby_tables::F4_ij[2][1]  + Y2*cheby_tables::F4_ij[2][2]  + Y3*cheby_tables::F4_ij[2][3]  + Y4*cheby_tables::F4_ij[2][4];
            double C3=Y0*cheby_tables::F4_ij[3][0] + Y1*cheby_tables::F4_ij[3][1]  + Y2*cheby_tables::F4_ij[3][2]  + Y3*cheby_tables::F4_ij[3][3]  + Y4*cheby_tables::F4_ij[3][4];
            double C4=Y0*cheby_tables::F4_ij[4][0] + Y1*cheby_tables::F4_ij[4][1]  + Y2*cheby_tables::F4_ij[4][2]  + Y3*cheby_tables::F4_ij[4][3]  + Y4*cheby_tables::F4_ij[4][4];

            C0*=0.5;
            C1*=0.5;
            C2*=0.5;
            C3*=0.5;
            C4*=0.5;

            double W0= C0*0.5 - C1 + C2 -C3 + C4;
            double W1= 2*C1 -8*C2 + 18*C3 - 32*C4;
            double W2= 8*C2 - 48*C3 + 160*C4;
            double W3= 32*C3 - 256*C4;
            double W4= 125*C4;
            
            /*
            double X=0.0/5.0;
            double X_2=X*X;
            double X_3=X_2*X;
            double X_4=X_3*X;
            inverse_Y_samples[i*5]=W0 + W1*X + W2*X_2 + W3*X_3 + W4*X_4;
            inverse_X_samples[i*5]= X*spline_width + spline_low;
            
            X=1.0/5.0;
            X_2=X*X;
            X_3=X_2*X;
            X_4=X_3*X;
            inverse_Y_samples[i*5+1]=W0 + W1*X + W2*X_2 + W3*X_3 + W4*X_4;
            inverse_X_samples[i*5+1]= X*spline_width + spline_low;
            
            X=2.0/5.0;
            X_2=X*X;
            X_3=X_2*X;
            X_4=X_3*X;
            inverse_Y_samples[i*5+2]=W0 + W1*X + W2*X_2 + W3*X_3 + W4*X_4;
            inverse_X_samples[i*5+2]= X*spline_width + spline_low;
            
            X=3.0/5.0;
            X_2=X*X;
            X_3=X_2*X;
            X_4=X_3*X;
            inverse_Y_samples[i*5+3]=W0 + W1*X + W2*X_2 + W3*X_3 + W4*X_4;
            inverse_X_samples[i*5+3]= X*spline_width + spline_low;
            
            X=4.0/5.0;
            X_2=X*X;
            X_3=X_2*X;
            X_4=X_3*X;
            inverse_Y_samples[i*5+4]=W0 + W1*X + W2*X_2 + W3*X_3 + W4*X_4;
            inverse_X_samples[i*5+4]= X*spline_width + spline_low;
            */

            //CHECK PRECISION
            double P=std::abs( W4/(W0+W1+W2+W3) ); //fraction of answer due to W4.
            if( P> inverse_precision)
            {
                print("Low inverse precision:", P, "Consider implmenting 8th order");
            }
            
            //weights are chosen for an x between 0 and 1
            double SL_2=spline_low*spline_low;
            double SL_3=SL_2*spline_low;
            double SL_4=SL_3*spline_low;
            
            double inv_width=1.0/spline_width;
            double inv_width_2=inv_width*inv_width;
            double inv_width_3=inv_width_2*inv_width;
            double inv_width_4=inv_width_3*inv_width;
            
            double new_W0=W0             - W1*spline_low*inv_width       + W2*SL_2*inv_width_2     - W3*SL_3*inv_width_3     + W4*inv_width_4*SL_4;
            double new_W1=W1*inv_width   - 2.0*W2*spline_low*inv_width_2 + 3.0*W3*SL_2*inv_width_3 - 4.0*W4*inv_width_4*SL_3;
            double new_W2=W2*inv_width_2 - 3.0*W3*spline_low*inv_width_3 + 6.0*W4*inv_width_4*SL_2;
            double new_W3=W3*inv_width_3 - 4.0*W4*inv_width_4*spline_low;
            double new_W4=W4*inv_width_4;

            x_vals[i]=spline_low;
            gsl::vector W({new_W0, new_W1, new_W2, new_W3,new_W4});
            splines.emplace_back(W);

            spline_low+=spline_width;
            i++;
        }

        x_vals[i]=spline_low;

        auto ret=std::make_shared<poly_spline>();
        ret->x_vals=x_vals;
        ret->splines=splines;
        ret->lower_fill=std::nan("");
        ret->upper_fill=std::nan("");
        return ret;
    }

    rand_sampler inverse_transform(double inverse_precision, double& rate_out)
    //POWER!!! UNLIMITED POWER!!!
    {
       
        std::list<sampler_helper*> samplers;
        top_section->get_sorted(samplers);

        std::vector<fourth_order_weights>  inverted_splines;
        inverted_splines.reserve(samplers.size());
        gsl::vector spline_weights(samplers.size());

        int i=0;
        for(auto SH : samplers)
        {
            quartic_inversion_helper inverter;

            inverter.W1=SH->K0;
            inverter.W2=SH->K1*0.5;
            inverter.W3=SH->K2/3.0;
            inverter.W4=SH->K3*0.25;

            spline_weights[i] = inverter.set(SH->Xlow, SH->Xhigh );

            double Y0=inverter.invert((cheby_tables::U4_i[0]+1)*0.5);
            double Y1=inverter.invert((cheby_tables::U4_i[1]+1)*0.5);
            double Y2=inverter.invert((cheby_tables::U4_i[2]+1)*0.5);
            double Y3=inverter.invert((cheby_tables::U4_i[3]+1)*0.5);
            double Y4=inverter.invert((cheby_tables::U4_i[4]+1)*0.5);

            double C0=Y0*cheby_tables::F4_ij[0][0] + Y1*cheby_tables::F4_ij[0][1]  + Y2*cheby_tables::F4_ij[0][2]  + Y3*cheby_tables::F4_ij[0][3]  + Y4*cheby_tables::F4_ij[0][4];
            double C1=Y0*cheby_tables::F4_ij[1][0] + Y1*cheby_tables::F4_ij[1][1]  + Y2*cheby_tables::F4_ij[1][2]  + Y3*cheby_tables::F4_ij[1][3]  + Y4*cheby_tables::F4_ij[1][4];
            double C2=Y0*cheby_tables::F4_ij[2][0] + Y1*cheby_tables::F4_ij[2][1]  + Y2*cheby_tables::F4_ij[2][2]  + Y3*cheby_tables::F4_ij[2][3]  + Y4*cheby_tables::F4_ij[2][4];
            double C3=Y0*cheby_tables::F4_ij[3][0] + Y1*cheby_tables::F4_ij[3][1]  + Y2*cheby_tables::F4_ij[3][2]  + Y3*cheby_tables::F4_ij[3][3]  + Y4*cheby_tables::F4_ij[3][4];
            double C4=Y0*cheby_tables::F4_ij[4][0] + Y1*cheby_tables::F4_ij[4][1]  + Y2*cheby_tables::F4_ij[4][2]  + Y3*cheby_tables::F4_ij[4][3]  + Y4*cheby_tables::F4_ij[4][4];

            C0*=0.5;
            C1*=0.5;
            C2*=0.5;
            C3*=0.5;
            C4*=0.5;
            
            double W0= C0*0.5 - C1 + C2 -C3 + C4;
            double W1= 2*C1 -8*C2 + 18*C3 - 32*C4;
            double W2= 8*C2 - 48*C3 + 160*C4;
            double W3= 32*C3 - 256*C4;
            double W4= 125*C4;

            //CHECK PRECISION
            double P=std::abs( W4/(W0+W1+W2+W3) ); //fraction of answer due to W4.
            if( P> inverse_precision)
            {
                print("Low inverse precision:", P, "Consider implmenting 8th order");
            }

            inverted_splines.emplace_back(W0, W1, W2, W3, W4);

            i++;
        }

        return rand_sampler(inverted_splines, spline_weights, rate_out);
    }
};


//should write a 4th order adaptive sampler. Which could use the 2nd order to estimate error??

#endif // CHEBYSHEV_HPP



















