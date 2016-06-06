
#ifndef SPLINE_2D_HPP
#define SPLINE_2D_HPP

#include <memory>
#include <list>
#include <vector>
#include <gsl/gsl_sf_gamma.h>


#include "GSL_utils.hpp"
#include "vector.hpp"
#include "functor.hpp"
#include "gen_ex.hpp"

#include "spline.hpp"



class adaptive_2DSpline
//not exactly memory effiecent
//not exactly simple
{
    public:

    class tmp_spline_1D
    {
        public:
        gsl::vector weights;
        double left_X;
        double right_X;

        bool add(std::list<tmp_spline_1D*>& input_interpolant, bool subtract=false)
        //adds this to the input_interpolant
        //returns true if input_interpolant took charge of the pointer to this object. False otherwise
        //if subtact is true, subtract this FROM input_interplant
        {
            auto iter=input_interpolant.begin();
            auto end=input_interpolant.end();

            if(subtract)
            {
                weights*=-1;
            }

            if(iter==end)
            //input interpolant is empty
            {
                input_interpolant.push_back(this); //insert this if and only if it is followed by return true
                //we cannot insert this twice.
                return true;
            }

            if(right_X <= (*iter)->left_X)
            //this spline is to the left of the entire interpolant
            {
                input_interpolant.push_front(this);
                return true;
            }

            if(left_X < (*iter)->left_X )
            //only part of this spline is to the left of the interpolant
            {
                auto new_spline=new tmp_spline_1D;
                new_spline->weights=weights.clone();
                new_spline->left_X=left_X;
                new_spline->right_X=(*iter)->left_X ;

                input_interpolant.push_front(new_spline);
            }

            for( ; iter!=end; iter++ )
            {
                if( right_X< (*iter)->left_X) return false; //this spline is to left of iter, with no overlap. thus we are done

                if( left_X>=(*iter)->right_X ) //this spline is to the right of iter, with no overlap
                {
                    //check for space after iter
                    auto NX=std::next(iter);
                    if(NX==end or right_X< (*NX)->left_X )
                    //this spline fits into a break in the interpolant
                    {
                        input_interpolant.insert(NX, this);
                        return true;
                    }
                    else if (left_X < (*NX)->left_X )
                    //the beginning of this spline fits into the break in the interpolant
                    {
                        auto new_spline=new tmp_spline_1D;
                        new_spline->weights=weights.clone();
                        new_spline->left_X=left_X;
                        new_spline->right_X=(*NX)->left_X ;

                        input_interpolant.insert(NX,new_spline);
                    }
                    continue;
                }

                //left_X< (*iter)->right_X garenteed to be true

                if(left_X <= (*iter)->left_X  and right_X>=(*iter)->right_X)
                //iter is totally subsumed by this spline
                {
                    (*iter)->weights+=weights;

                    if( right_X>(*iter)->right_X )
                    {
                        auto NX=std::next(iter);
                        if(NX==end)
                        {
                            left_X=(*iter)->right_X;
                            input_interpolant.push_back(this);
                            return true;
                        }
                        else if ( (*NX)->left_X > (*iter)->right_X)
                        //there is a break between splines
                        {
                            auto new_spline=new tmp_spline_1D;
                            new_spline->weights=weights.clone();
                            new_spline->left_X=(*iter)->right_X;

                            if(right_X<=(*NX)->left_X)
                                new_spline->right_X=right_X ;
                            else
                                new_spline->right_X=(*NX)->left_X ;
                            input_interpolant.insert(NX,new_spline);
                        }
                    }
                    continue;
                }

                if(left_X>(*iter)->left_X and right_X<(*iter)->right_X)
                //this spline is inside of iter
                {
                    auto NX=std::next(iter);

                    //insert this
                    weights+=(*iter)->weights;
                    input_interpolant.insert(NX, this);

                    //rightmost section
                    auto new_spline=new tmp_spline_1D;
                    new_spline->weights=(*iter)->weights.clone();
                    new_spline->left_X=right_X;
                    new_spline->right_X=(*iter)->right_X ;
                    input_interpolant.insert(NX, new_spline);

                    //adjusts iter
                    (*iter)->right_X=left_X;

                    //nothing left to do!
                    return true;
                }

                if(left_X==(*iter)->left_X and right_X<(*iter)->right_X)
                //this spline is on the left of iter
                {
                    weights+=(*iter)->weights;
                    (*iter)->left_X=right_X;
                    input_interpolant.insert(iter, this);

                    //nothing left to do!
                    return true;
                }

                if(left_X>(*iter)->left_X and right_X==(*iter)->right_X)
                //this spline is in and on the right of iter
                {
                    auto NX=std::next(iter);
                    weights+=(*iter)->weights;
                    (*iter)->right_X=left_X;
                    input_interpolant.insert(NX, this);

                    //nothing left to do!
                    return true;
                }

                if(left_X> (*iter)->left_X and right_X>(*iter)->right_X)
                //iter is to the left but intersects with this
                {
                    auto NX=std::next(iter);

                    //new section
                    auto new_spline=new tmp_spline_1D;
                    new_spline->weights=weights+(*iter)->weights;
                    new_spline->left_X=left_X;
                    new_spline->right_X=(*iter)->right_X ;
                    input_interpolant.insert(NX, new_spline);

                    (*iter)->right_X=left_X;

                    //check right edge
                    if(NX==end)
                    {
                        left_X=new_spline->right_X;
                        input_interpolant.insert(NX, this);
                        return true;
                    }
                    else if( (*NX)->left_X != (*iter)->right_X )
                    {
                        left_X=new_spline->right_X;

                        //new section
                        auto new_spline=new tmp_spline_1D;
                        new_spline->weights=weights.clone();
                        new_spline->left_X=new_spline->right_X;

                        if(right_X<=(*NX)->left_X)
                            new_spline->right_X=right_X;
                        else
                            new_spline->right_X=(*NX)->left_X;

                        input_interpolant.insert(NX, new_spline);
                    }
                }

                if(left_X< (*iter)->left_X and right_X<(*iter)->right_X)
                //this is to the left itersection of iter
                {
                    weights+=(*iter)->weights;
                    left_X=(*iter)->left_X;
                    (*iter)->left_X=right_X;
                    input_interpolant.insert(iter, this);
                    return true;
                }

            }
            return false;
        }
    };

    typedef std::list<tmp_spline_1D*> spline_list;

    static void add_splines(spline_list& lst_A, spline_list& lst_B, bool subtract=false)
    //if subtract is true, then subtract A from B
    {
        for(auto S : lst_A)
        {
            if( not S->add(lst_B, subtract) )
            {
                delete S;
            }
        }
        lst_A.clear();
    }


    static std::shared_ptr<poly_spline> convert(spline_list& lst)
    //converts a spling list into a poly_spline, and deletes the spline list
    {
        auto out_spline=std::make_shared<poly_spline>();
        out_spline->lower_fill=std::nan("");
        out_spline->upper_fill=std::nan("");

        out_spline->x_vals=gsl::vector(lst.size()+1);
        out_spline->splines.reserve(lst.size());

        double last_spline_end=lst.front()->left_X;
        size_t i=0;
        for(auto sp : lst)
        {
            if(sp->left_X != last_spline_end) throw gen_exception("vague error in spline conversion");
            out_spline->x_vals[i]=sp->left_X;
            out_spline->splines.emplace_back(sp->weights);
            last_spline_end=sp->right_X;
            i++;
            delete sp;
        }
        out_spline->x_vals[i]=last_spline_end;
        lst.clear();

        return out_spline;
    }

    class quad_section;

    class tri_section
    {
        public:
        //these are set externally
        int type; //0 is border is to left, 1 is up, 2 is down 3 is to right

        quad_section* bordering_quad;

        double X1;//lower or left point
        double Y1;
        double F1;

        double X2;//upper or right point
        double Y2;
        double F2;

        double X3;//middle of quad
        double Y3;
        double F3;

        //these are calculated
        double weight_A;
        double weight_B;
        double weight_C;

        double Iweight_00;
        double Iweight_01;
        double Iweight_02;
        double Iweight_03;
        double Iweight_04;
        double Iweight_05;

        double Iweight_10;
        double Iweight_11;
        double Iweight_12;
        double Iweight_13;
        double Iweight_14;
        double Iweight_15;

        //own these two pointers
        tri_section* section_A;
        tri_section* section_B;

        tri_section()
        {
            section_A=0;
            section_B=0;
        }

        ~tri_section()
        {
            if(section_A) delete section_A;
            if(section_B) delete section_B;
        }

        void set_weights()
        //assume the three points have been externally set
        {

            //set weights for the spline

            if(type==0 or type==3)//vertical style tri
            {
                weight_C=(F2-F1)/(Y2-Y1);
                weight_B=( (F3-F1)-weight_C*(Y3-Y1) )/(X3-X1);

            }
            else//horizontal tri
            {
                weight_C=( (X2-X1)*(F3-F1) - (F2-F1)*(X3-X1) )/( (Y3-Y1)*(X2-X1) );
                weight_B=(F2-F1)/(X2-X1);
            }

            weight_A=F1 - Y1*weight_C - X1*weight_B;

            if(weight_A != weight_A)
            {
                print(type);
                print(X1, X2, X3, Y1, Y2, Y3, F1, F2, F3);
                print(weight_C, weight_B, weight_A);
                throw gen_exception("function cannot be interpolated by this spline");
            }

            //set weights for intesection testing
            double E1x=X2-X1;
            double E1y=Y2-Y1;
            double E2x=X3-X1;
            double E2y=Y3-Y1;
            double E3x=X3-X2;
            double E3y=Y3-Y2;

            double K0=E1x*Y1 - E1y*X1;
            double K1=E3x*Y2 - E3y*X2;
            double K2=E2y*X3 - E2x*Y3;

            Iweight_00=K0*K1;
            Iweight_01=E3y*K0+E1y*K1;
            Iweight_02=-(E3x*K0 + E1x*K1);
            Iweight_03=-(E1y*E3x + E3y*E1x);
            Iweight_04=E1y*E3y;
            Iweight_05=E1x*E3x;

            Iweight_10=K0*K2;
            Iweight_11=E1y*K2 - E2y*K0;
            Iweight_12=E2x*K0 - E1x*K2;
            Iweight_13=E1y*E2x + E2y*E1x;
            Iweight_14=-E1y*E2y;
            Iweight_15=-E1x*E2x;
        }

        inline bool intersection(double X, double Y)
        {
            double T1=Iweight_00 + Iweight_01*X + Iweight_02*Y + Iweight_03*X*Y + Iweight_04*X*X + Iweight_05*Y*Y;
            if(T1>=0)
            {
                double T2=Iweight_10 + Iweight_11*X + Iweight_12*Y + Iweight_13*X*Y + Iweight_14*X*X + Iweight_15*Y*Y;
                if(T2>=0)
                {
                    return true;
                }
            }
            return false;
        }

        inline double rough_call(double X, double Y)
        {
            return weight_A + weight_B*X + weight_C*Y;
        }

        inline double call(double X, double Y)
        {
            if(section_A)//we have sub-tri
            {
                if(section_A->intersection(X,Y))
                {
                    return section_A->call(X,Y);
                }
                else
                {
                    return section_B->call(X,Y);
                }
            }
            else
            {
                return rough_call(X,Y);
            }
        }

        void triangularize()
        //split into smaller triangles, so that spline is smooth
        {
            if(bordering_quad and (bordering_quad->state==3 or bordering_quad->state==4))
            {
                if(type==0)//0 is border is to left, 1 is up, 2 is down 3 is to right
                {
                    double middle_Y=(Y1+Y2)*0.5;
                    double middle_F=bordering_quad->quadrent_B->F3;
                    section_A= new tri_section();
                    section_B= new tri_section();

                    section_A->type=0;
                    section_A->bordering_quad=bordering_quad->quadrent_B;

                    section_A->X1=X1;
                    section_A->Y1=middle_Y;
                    section_A->F1=middle_F;

                    section_A->X2=X2;
                    section_A->Y2=Y2;
                    section_A->F2=F2;

                    section_A->X3=X3;
                    section_A->Y3=Y3;
                    section_A->F3=F3;

                    section_B->type=0;
                    section_B->bordering_quad=bordering_quad->quadrent_C;

                    section_B->X1=X1;
                    section_B->Y1=Y1;
                    section_B->F1=F1;

                    section_B->X2=X2;
                    section_B->Y2=middle_Y;
                    section_B->F2=middle_F;

                    section_B->X3=X3;
                    section_B->Y3=Y3;
                    section_B->F3=F3;
                }
                else if(type==1)
                {
                    double middle_X=(X1+X2)*0.5;
                    double middle_F=bordering_quad->quadrent_C->F4;
                    section_A= new tri_section();
                    section_B= new tri_section();

                    section_A->type=1;
                    section_A->bordering_quad=bordering_quad->quadrent_C;

                    section_A->X1=middle_X;
                    section_A->Y1=Y1;
                    section_A->F1=middle_F;

                    section_A->X2=X2;
                    section_A->Y2=Y2;
                    section_A->F2=F2;

                    section_A->X3=X3;
                    section_A->Y3=Y3;
                    section_A->F3=F3;

                    section_B->type=1;
                    section_B->bordering_quad=bordering_quad->quadrent_D;

                    section_B->X1=X1;
                    section_B->Y1=Y1;
                    section_B->F1=F1;

                    section_B->X2=middle_X;
                    section_B->Y2=Y2;
                    section_B->F2=middle_F;

                    section_B->X3=X3;
                    section_B->Y3=Y3;
                    section_B->F3=F3;
                }
                else if(type==2)
                {
                    double middle_X=(X1+X2)*0.5;
                    double middle_F=bordering_quad->quadrent_A->F2;
                    section_A= new tri_section();
                    section_B= new tri_section();

                    section_A->type=2;
                    section_A->bordering_quad=bordering_quad->quadrent_B;

                    section_A->X1=middle_X;
                    section_A->Y1=Y1;
                    section_A->F1=middle_F;

                    section_A->X2=X2;
                    section_A->Y2=Y2;
                    section_A->F2=F2;

                    section_A->X3=X3;
                    section_A->Y3=Y3;
                    section_A->F3=F3;

                    section_B->type=2;
                    section_B->bordering_quad=bordering_quad->quadrent_A;

                    section_B->X1=X1;
                    section_B->Y1=Y1;
                    section_B->F1=F1;

                    section_B->X2=middle_X;
                    section_B->Y2=Y2;
                    section_B->F2=middle_F;

                    section_B->X3=X3;
                    section_B->Y3=Y3;
                    section_B->F3=F3;
                }
                else //type==3
                {
                    double middle_Y=(Y1+Y2)*0.5;
                    double middle_F=bordering_quad->quadrent_A->F4;
                    section_A= new tri_section();
                    section_B= new tri_section();

                    section_A->type=3;
                    section_A->bordering_quad=bordering_quad->quadrent_A;

                    section_A->X1=X1;
                    section_A->Y1=middle_Y;
                    section_A->F1=middle_F;

                    section_A->X2=X2;
                    section_A->Y2=Y2;
                    section_A->F2=F2;

                    section_A->X3=X3;
                    section_A->Y3=Y3;
                    section_A->F3=F3;

                    section_B->type=3;
                    section_B->bordering_quad=bordering_quad->quadrent_D;

                    section_B->X1=X1;
                    section_B->Y1=Y1;
                    section_B->F1=F1;

                    section_B->X2=X2;
                    section_B->Y2=middle_Y;
                    section_B->F2=middle_F;

                    section_B->X3=X3;
                    section_B->Y3=Y3;
                    section_B->F3=F3;
                }

                section_A->set_weights();
                section_B->set_weights();

                section_A->triangularize();
                section_B->triangularize();
            }
        }

        ////functions for integration ////
        inline double gappa_function(int N, int K, double T0, double T1, double B0, double B1)
        {
            return gsl_sf_choose(N, K)*(std::pow(T0, N-K)*std::pow(T1,K) - std::pow(B0, N-K)*std::pow(B1,K));
        }

        inline size_t search(gsl::vector& vals, double Y)
        {

            if(Y==vals[0])
            {
                return 0;
            }
            else if(Y==vals[vals.size()-1])
            {
                return vals.size()-2;
            }
            else if(Y<vals[0])
            {
                throw gen_exception("value ", Y, " is below spline range");
            }
            else if(Y>vals[vals.size()-1])
            {
                throw gen_exception("value ", Y, " is above spline range");
            }
            else
            {
                return search_sorted_d(vals, Y);
            }
        }

        void nosplit_singleIntegrate(poly_spline::spline_piece& IN, spline_list& out)
        {
            if(type==1 or type==3)//the horizontal types
            {
                double L1=-(Y3-Y1)/(X1-X3);
                double L0=Y1-X1*L1;
                double R1=-(Y2-Y3)/(X3-X2);
                double R0=Y3-X3*R1;

                int N=IN.weights.size()-1; //max power of input spline

                //the top bit same for left and right tri
                //get for KA and KB
                double KAB_top=0;
                for(int I=0; I<=N; I++)
                {
                    KAB_top+=IN.weights[I]*std::pow(Y1, I+1)/(I+1);
                }

                //KC
                double KC_top=0;
                for(int I=0; I<=N; I++)
                {
                    KC_top+=IN.weights[I]*std::pow(Y1, I+2)/(I+2);
                }


                if(X3!=X1)//left part
                {
                    gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                    //the bottom bit
                    for(int p=0; p<weights.size(); p++)
                    {
                        //first get KA
                        double KA=0;
                        for(int I=fmax(0,p-1); I<=N; I++)
                        {
                            KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, L0, L1)/(I+1);
                        }

                        //KB
                        double KB=0;
                        for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                        {
                            KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, L0, L1)/(I+1);
                        }

                        //KC
                        double KC=0;
                        for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                        {
                            KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, L0, L1)/(I+2);
                        }
                        weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                    }
                    //top
                    weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                    weights[1]+=weight_B*KAB_top;

                    if(Y3>Y1)
                    {
                        weights*=-1;
                    }

                    auto spl_seg=new tmp_spline_1D;
                    spl_seg->weights=weights;
                    spl_seg->left_X=X1;
                    spl_seg->right_X=X3;

                    if(not spl_seg->add(out))
                    {
                        delete spl_seg;
                    }


                }
                if(X3!=X2)//right part
                {
                    gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                    //the bottom bit
                    for(int p=0; p<weights.size(); p++)
                    {
                        //first get KA
                        double KA=0;
                        for(int I=fmax(0,p-1); I<=N; I++)
                        {
                            KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, R0, R1)/(I+1);
                        }

                        //KB
                        double KB=0;
                        for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                        {
                            KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, R0, R1)/(I+1);
                        }

                        //KC
                        double KC=0;
                        for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                        {
                            KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, R0, R1)/(I+2);
                        }
                        weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                    }
                    //the top bit
                    weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                    weights[1]+=weight_B*KAB_top;

                    if(Y3>Y1)
                    {
                        weights*=-1;
                    }

                    auto spl_seg=new tmp_spline_1D;
                    spl_seg->weights=weights;
                    spl_seg->left_X=X3;
                    spl_seg->right_X=X2;

                    if(not spl_seg->add(out))
                    {
                        delete spl_seg;
                    }
                }
            }
            else if(type==0 or type==2)//vertical types
            {

                double T1=-(Y3-Y2)/(X2-X3);
                double T0=Y2-X2*T1;
                double B1=-(Y3-Y1)/(X1-X3);
                double B0=Y1-X1*B1;

                int N=IN.weights.size()-1; //max power of input spline
                gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=std::fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, T0, T1, B0, B1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=std::fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, T0, T1, B0, B1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=std::fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, T0, T1, B0, B1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=std::fmin(X1, X3);
                spl_seg->right_X=std::fmax(X2, X3);

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }
            }
        }

        void split_vertical_singleIntegrand(double split_y_coord, poly_spline::spline_piece& IN, spline_list& out, bool do_bottom)
        {

            double T1=-(Y3-Y2)/(X2-X3);
            double T0=Y2-X2*T1;
            double B1=-(Y3-Y1)/(X1-X3);
            double B0=Y1-X1*B1;

            double X_split; //x coordinate where split_y_coord intercepts the top or bottom
            bool intercept_bottom=false;
            if(split_y_coord<Y3) //the horizontal line intercepts the bottom line
            {
                intercept_bottom=true;
                X_split=(split_y_coord-B0)/B1;
            }
            else if(split_y_coord>=Y3) //the horizontal line intercepts the top line
            {
                intercept_bottom=false;
                X_split=(split_y_coord-T0)/T1;
            }

            double U0=B0;
            double U1=B1;
            if(not do_bottom)
            {
                U0=T0;
                U1=T1;
                //be sure to multiply final result by -1
            }

            //note that X_split COULD be on the LEFT side of X1,X2  IF X3< X1,X2

            int N=IN.weights.size()-1; //max power of input spline

            gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
            //the bottom bit
            for(int p=0; p<weights.size(); p++)
            {
                //first get KA
                double KA=0;
                for(int I=fmax(0,p-1); I<=N; I++)
                {
                    KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, U0, U1)/(I+1);
                }

                //KB
                double KB=0;
                for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                {
                    KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, U0, U1)/(I+1);
                }

                //KC
                double KC=0;
                for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                {
                    KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, U0, U1)/(I+2);
                }
                weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
            }
            //the top bit
            //get for KA and KB
            double KAB_top=0;
            for(int I=0; I<=N; I++)
            {
                KAB_top+=IN.weights[I]*std::pow(split_y_coord, I+1)/(I+1);
            }

            //KC
            double KC_top=0;
            for(int I=0; I<=N; I++)
            {
                KC_top+=IN.weights[I]*std::pow(split_y_coord, I+2)/(I+2);
            }
            weights[0]+=weight_A*KAB_top + KC_top*weight_C;
            weights[1]+=weight_B*KAB_top;

            if(not do_bottom) weights*=-1;

            auto spl_seg=new tmp_spline_1D;
            spl_seg->weights=weights;

            spl_seg->left_X=std::fmin(X1, X_split);
            spl_seg->right_X=std::fmax(X1, X_split);

            if(not spl_seg->add(out))
            {
                delete spl_seg;
            }


            ////now we add anouther spline IF the horiontal line intercepts the top line
            if( (do_bottom and not intercept_bottom) or (not do_bottom and intercept_bottom) )
            {
                weights=gsl::vector(N+2+1);//clear weights
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, T0, T1, B0, B1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, T0, T1, B0, B1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, T0, T1, B0, B1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=std::fmin(X_split, X3);
                spl_seg->right_X=std::fmax(X_split, X3);

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }
            }
        }


        inline void vertical_integration(std::shared_ptr<poly_spline> IN, spline_list& out)
        {
            size_t lower_spline_index=search(IN->x_vals, Y1);
            size_t upper_spline_index=search(IN->x_vals, Y2);

            if(upper_spline_index==lower_spline_index) //then the poly_spline does not split this tri
            {
                nosplit_singleIntegrate(IN->splines[lower_spline_index], out);
            }
            else
            {   //get the bottom bit
                split_vertical_singleIntegrand(IN->x_vals[lower_spline_index+1], IN->splines[lower_spline_index], out, true);

                for(size_t SP_i=lower_spline_index+1; SP_i!=upper_spline_index; SP_i++)
                {
                    spline_list tmp_lower;
                    spline_list tmp_upper;

                    split_vertical_singleIntegrand(IN->x_vals[SP_i], IN->splines[SP_i], tmp_lower, true);
                    split_vertical_singleIntegrand(IN->x_vals[SP_i+1], IN->splines[SP_i], tmp_upper, true);
                    add_splines(tmp_lower, tmp_upper, true); //subtract lower from upper
                    add_splines(tmp_upper, out);
                }

                split_vertical_singleIntegrand(IN->x_vals[upper_spline_index], IN->splines[upper_spline_index], out, false);
            }
        }

        void horizontal_Y1Integrate(double split_y_coord, poly_spline::spline_piece& IN, spline_list& out)
        {
            double L1=-(Y3-Y1)/(X1-X3);
            double L0=Y1-X1*L1;
            double R1=-(Y2-Y3)/(X3-X2);
            double R0=Y3-X3*R1;

            double left_X_split=(split_y_coord-L0)/L1;
            double right_X_split=(split_y_coord-R0)/R1;

            int N=IN.weights.size()-1; //max power of input spline

            //the top bit same for left and right tri
            //get for KA and KB
            double KAB_top=0;
            for(int I=0; I<=N; I++)
            {
                KAB_top+=IN.weights[I]*std::pow(Y1, I+1)/(I+1);
            }

            //KC
            double KC_top=0;
            for(int I=0; I<=N; I++)
            {
                KC_top+=IN.weights[I]*std::pow(Y1, I+2)/(I+2);
            }


            if(X3!=X1)//left part
            {
                gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                //the bottom bit
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, L0, L1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, L0, L1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, L0, L1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                //top
                weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                weights[1]+=weight_B*KAB_top;

                if(Y3>Y1)
                {
                    weights*=-1;
                }

                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=X1;
                spl_seg->right_X=left_X_split;

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }


            }
            if(X3!=X2)//right part
            {
                gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                //the bottom bit
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, R0, R1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, R0, R1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, R0, R1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                //the top bit
                weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                weights[1]+=weight_B*KAB_top;

                if(Y3>Y1)
                {
                    weights*=-1;
                }

                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=right_X_split;
                spl_seg->right_X=X2;

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }
            }
            if(right_X_split!=left_X_split)//middle section
            {
                gsl::vector weights=make_vector(N+2+1, 0.0);//this bit is, oddly enough, linear
                //however, we want to keep the power the same in all places, so we will pad the coifeicents with zeros

                //bottom
                //get for KA and KB
                double KAB_bottom=0;
                for(int I=0; I<=N; I++)
                {
                    KAB_bottom+=IN.weights[I]*std::pow(split_y_coord, I+1)/(I+1);
                }

                //KC
                double KC_bottom=0;
                for(int I=0; I<=N; I++)
                {
                    KC_bottom+=IN.weights[I]*std::pow(split_y_coord, I+2)/(I+2);
                }

                weights[0]+=weight_A*(KAB_top-KAB_bottom) + (KC_top-KC_bottom)*weight_C;
                weights[1]+=weight_B*(KAB_top-KAB_bottom);

                if(Y3>Y1)
                {
                    weights*=-1;
                }

                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=left_X_split;
                spl_seg->right_X=right_X_split;

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }
            }
        }

        void horizontal_Y3Integrate(double split_y_coord, poly_spline::spline_piece& IN, spline_list& out)
        {
            double L1=-(Y3-Y1)/(X1-X3);
            double L0=Y1-X1*L1;
            double R1=-(Y2-Y3)/(X3-X2);
            double R0=Y3-X3*R1;

            double left_X_split=(split_y_coord-L0)/L1;
            double right_X_split=(split_y_coord-R0)/R1;

            int N=IN.weights.size()-1; //max power of input spline

            //the top bit same for left and right tri
            //get for KA and KB
            double KAB_top=0;
            for(int I=0; I<=N; I++)
            {
                KAB_top+=IN.weights[I]*std::pow(split_y_coord, I+1)/(I+1);
            }

            //KC
            double KC_top=0;
            for(int I=0; I<=N; I++)
            {
                KC_top+=IN.weights[I]*std::pow(split_y_coord, I+2)/(I+2);
            }


            if(X3!=X1)//left part
            {
                gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                //the bottom bit
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, L0, L1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, L0, L1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, L0, L1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                //top
                weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                weights[1]+=weight_B*KAB_top;

                if(Y3>Y1)
                {
                    weights*=-1;
                }

                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=left_X_split;
                spl_seg->right_X=X3;

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }


            }
            if(X3!=X2)//right part
            {
                gsl::vector weights(N+2+1);//power is two greater then spline. size is one greater than power
                //the bottom bit
                for(int p=0; p<weights.size(); p++)
                {
                    //first get KA
                    double KA=0;
                    for(int I=fmax(0,p-1); I<=N; I++)
                    {
                        KA+=IN.weights[I]*gappa_function(I+1, p, 0, 0, R0, R1)/(I+1);
                    }

                    //KB
                    double KB=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KB+=IN.weights[I]*gappa_function(I+1, p-1, 0, 0, R0, R1)/(I+1);
                    }

                    //KC
                    double KC=0;
                    for(int I=fmax(0,p-2); I<=N and p!=0; I++)
                    {
                        KC+=IN.weights[I]*gappa_function(I+2, p, 0, 0, R0, R1)/(I+2);
                    }
                    weights[p]=KA*weight_A + KB*weight_B + KC*weight_C;
                }
                //the top bit
                weights[0]+=weight_A*KAB_top + KC_top*weight_C;
                weights[1]+=weight_B*KAB_top;

                if(Y3>Y1)
                {
                    weights*=-1;
                }

                auto spl_seg=new tmp_spline_1D;
                spl_seg->weights=weights;
                spl_seg->left_X=X3;
                spl_seg->right_X=right_X_split;

                if(not spl_seg->add(out))
                {
                    delete spl_seg;
                }
            }
        }

        inline void horizontal_bottomIntegrate(double split_y_coord, poly_spline::spline_piece& IN, spline_list& out)
        {
            if(Y3>Y1)
            {
                horizontal_Y1Integrate(split_y_coord, IN, out);
            }
            else
            {
                horizontal_Y3Integrate(split_y_coord, IN, out);
            }
        }

        inline void horizontal_topIntegrate(double split_y_coord, poly_spline::spline_piece& IN, spline_list& out)
        {
            if(Y3>Y1)
            {
                horizontal_Y3Integrate(split_y_coord, IN, out);
            }
            else
            {
                horizontal_Y1Integrate(split_y_coord, IN, out);
            }
        }

        inline void horizontal_integration(std::shared_ptr<poly_spline> IN, spline_list& out)
        {
            size_t lower_spline_index=search(IN->x_vals, std::fmin(Y1, Y3));
            size_t upper_spline_index=search(IN->x_vals, std::fmax(Y1, Y3));

            if(upper_spline_index==lower_spline_index) //then the poly_spline does not split this tri
            {
                nosplit_singleIntegrate(IN->splines[lower_spline_index], out);
            }
            else
            {   //get the bottom bit
                horizontal_bottomIntegrate(IN->x_vals[lower_spline_index+1], IN->splines[lower_spline_index], out);

                //middle bits
                for(size_t SP_i=lower_spline_index+1; SP_i!=upper_spline_index; SP_i++)
                {
                    spline_list tmp_lower;
                    spline_list tmp_upper;

                    horizontal_bottomIntegrate(IN->x_vals[SP_i], IN->splines[SP_i], tmp_lower);
                    horizontal_bottomIntegrate(IN->x_vals[SP_i+1], IN->splines[SP_i], tmp_upper);
                    add_splines(tmp_lower, tmp_upper, true); //subtract lower from upper
                    add_splines(tmp_upper, out);
                }

                //top
                horizontal_topIntegrate(IN->x_vals[upper_spline_index], IN->splines[upper_spline_index], out);
            }
        }

        void integrate_along_Y(std::shared_ptr<poly_spline> IN, spline_list& out)
        {
            if(type==0 or type==2)//the vertical types
            {
                vertical_integration(IN, out);
                //if(section_A)
                //{
                    //section_A->vertical_integration(IN, out);
                    //section_B->vertical_integration(IN, out);
                //}
                //else
                //{
                    //vertical_integration(IN, out);
                //}
            }
            else if(type==1 or type==3)//the horizontal types
            {
                horizontal_integration(IN, out);
                //if(section_A)
                //{
                    //section_A->horizontal_integration(IN, out);
                    //section_B->horizontal_integration(IN, out);
                //}
                //else
                //{
                    //horizontal_integration(IN, out);
                //}
            }
        }
    };


    class quad_section
    {
        public:
        int level;

        int state; //0 is before refinment. 1 is refined to triangles. 3 means refined to rectangles. 2 means can not longer refine,and should be deleted.
        //5 means that that one of the childern shapes is (or was) a 2, and this quad is now refined to triangles. 4 means that one of the childern is a 3 and this quad is refined to rectangles

        double X1;
        double Y1;
        double X2;
        double Y2;
        double F1;
        double F2;
        double F3;
        double F4;

        double middle_X;
        double middle_Y;

        //do not own these pointers
        quad_section* left_neighbor;
        quad_section* upper_neighbor;
        quad_section* right_neighbor;
        quad_section* lower_neighbor;

        //DO own these pointers
        quad_section* quadrent_A; //upper left
        quad_section* quadrent_B;//upper right
        quad_section* quadrent_C;//lower left
        quad_section* quadrent_D;//lower right

        //triangles
        tri_section* upper_tri;
        tri_section* left_tri;
        tri_section* lower_tri;
        tri_section* right_tri;

        quad_section()
        {
            //owner is expected to set all the values correctly. constructor is simple.
            left_neighbor=0;
            upper_neighbor=0;
            right_neighbor=0;
            lower_neighbor=0;

            //DO own these pointers
            quadrent_A=0;
            quadrent_B=0;
            quadrent_C=0;
            quadrent_D=0;

            upper_tri=0;
            left_tri=0;
            lower_tri=0;
            right_tri=0;

            state=0;
        }

        ~quad_section()
        {
            //DO own these pointers
            if(quadrent_A) delete quadrent_A;
            if(quadrent_B) delete quadrent_B;
            if(quadrent_C) delete quadrent_C;
            if(quadrent_D) delete quadrent_D;

            if(upper_tri) delete upper_tri;
            if(left_tri) delete left_tri;
            if(lower_tri) delete lower_tri;
            if(right_tri) delete right_tri;
        }

        //for a functor
        template<typename functor_T>
        void refine(functor_T& func, double precision_factor )
        {
            //expect that the neighbors are set. That is, they are null if there is no neighbor.
            //assume that x1, y1, x2, y2, f1, f2, f3, f4 are all set

            middle_X=(X1+X2)*0.5;
            middle_Y=(Y1+Y2)*0.5;

            if(middle_X != middle_X)
            {
                state=2;
                return;
            }

            if(middle_Y != middle_Y)
            {
                state=2;
                return;
            }


            //check if we are too small
            if(float( X1+( X1-middle_X))==float( X1))
            {
                state=2;
                return;
            }
            if(float( X2+( X2-middle_X))==float( X2))
            {
                state=2;
                return;
            }
            if(float( Y1+( Y1-middle_Y))==float( Y1))
            {
                state=2;
                return;
            }
            if(float( Y2+( Y2-middle_Y))==float( Y2))
            {
                state=2;
                return;
            }

            //check percent error
            double middle_value=func(middle_X, middle_Y);
            //double middle_guess=(F1+F2+F3+F4)*0.25;
//print("P.append( [", middle_X, ",", middle_Y, ",", middle_value, "])");


            //if( abs(middle_value-middle_guess) < abs(middle_value*percent_error) )
            if( float(middle_value*precision_factor + ( middle_value - (F1+F2+F3+F4)*0.25 ) ) == float(middle_value*precision_factor) )
            {
                //then we are withen precision, we will do triangles
                state=1;
            }
            else
            {
                // then we are outside precision. Do deeper quads
                state=3;

                //first, make the quads
                quadrent_A=new quad_section();//upper left
                quadrent_B=new quad_section();//upper right
                quadrent_C=new quad_section();//lower left
                quadrent_D=new quad_section();//lower right

                //set the easy neighbors
                quadrent_A->right_neighbor=quadrent_B;
                quadrent_A->lower_neighbor=quadrent_D;

                quadrent_B->left_neighbor=quadrent_A;
                quadrent_B->lower_neighbor=quadrent_C;

                quadrent_C->left_neighbor=quadrent_D;
                quadrent_C->upper_neighbor=quadrent_B;

                quadrent_D->right_neighbor=quadrent_C;
                quadrent_D->upper_neighbor=quadrent_A;

                //check the other neighbors, and find other function values
                double F5, F6, F7, F8;
                if(upper_neighbor and (upper_neighbor->state==3 or upper_neighbor->state==4))
                {
                    F6=upper_neighbor->quadrent_D->F3;
                    quadrent_A->upper_neighbor=upper_neighbor->quadrent_D;
                    quadrent_B->upper_neighbor=upper_neighbor->quadrent_C;
                }
                else
                {
                    F6=func(middle_X, Y1);

//print("P.append( [", middle_X, ",", Y1, ",", F6, "])");
                }

                if(right_neighbor and (right_neighbor->state==3 or right_neighbor->state==4))
                {
                    F7=right_neighbor->quadrent_A->F4;
                    quadrent_B->right_neighbor=right_neighbor->quadrent_A;
                    quadrent_C->right_neighbor=right_neighbor->quadrent_D;
                }
                else
                {
                    F7=func(X2, middle_Y);

//print("P.append( [", X2, ",", middle_Y, ",", F7, "])");
                }

                if(lower_neighbor and (lower_neighbor->state==3 or lower_neighbor->state==4))
                {
                    F8=lower_neighbor->quadrent_A->F2;
                    quadrent_C->lower_neighbor=lower_neighbor->quadrent_B;
                    quadrent_D->lower_neighbor=lower_neighbor->quadrent_A;
                }
                else
                {
                    F8=func(middle_X, Y2);

//print("P.append( [", middle_X, ",", Y2, ",", F8, "])");
                }

                if(left_neighbor and (left_neighbor->state==3 or left_neighbor->state==4))
                {
                    F5=left_neighbor->quadrent_B->F3;
                    quadrent_D->left_neighbor=left_neighbor->quadrent_C;
                    quadrent_A->left_neighbor=left_neighbor->quadrent_B;
                }
                else
                {
                    F5=func(X1, middle_Y);

//print("P.append( [", X1, ",", middle_Y, ",", F5, "])");
                }

                //set function values and locations
                quadrent_A->X1=X1;
                quadrent_A->X2=middle_X;
                quadrent_A->Y1=Y1;
                quadrent_A->Y2=middle_Y;
                quadrent_A->F1=F1;
                quadrent_A->F2=F6;
                quadrent_A->F3=middle_value;
                quadrent_A->F4=F5;

                quadrent_B->X1=middle_X;
                quadrent_B->X2=X2;
                quadrent_B->Y1=Y1;
                quadrent_B->Y2=middle_Y;
                quadrent_B->F1=F6;
                quadrent_B->F2=F2;
                quadrent_B->F3=F7;
                quadrent_B->F4=middle_value;

                quadrent_C->X1=middle_X;
                quadrent_C->X2=X2;
                quadrent_C->Y1=middle_Y;
                quadrent_C->Y2=Y2;
                quadrent_C->F1=middle_value;
                quadrent_C->F2=F7;
                quadrent_C->F3=F3;
                quadrent_C->F4=F8;

                quadrent_D->X1=X1;
                quadrent_D->X2=middle_X;
                quadrent_D->Y1=middle_Y;
                quadrent_D->Y2=Y2;
                quadrent_D->F1=F5;
                quadrent_D->F2=middle_value;
                quadrent_D->F3=F8;
                quadrent_D->F4=F4;

                //refine the quadrents
                quadrent_A->level=level+1;
                quadrent_B->level=level+1;
                quadrent_C->level=level+1;
                quadrent_D->level=level+1;

                quadrent_A->refine(func, precision_factor);
                if(quadrent_A->state==2)
                {
                    delete quadrent_A;
                    delete quadrent_B;
                    delete quadrent_C;
                    delete quadrent_D;
                    quadrent_A=0;
                    quadrent_B=0;
                    quadrent_C=0;
                    quadrent_D=0;

                    state=5;
                }
                if(state==3)
                {
                    quadrent_B->refine(func, precision_factor);
                    if(quadrent_B->state==2)
                    {
                        delete quadrent_A;
                        delete quadrent_B;
                        delete quadrent_C;
                        delete quadrent_D;
                        quadrent_A=0;
                        quadrent_B=0;
                        quadrent_C=0;
                        quadrent_D=0;

                        state=5;
                    }
                }
                if(state==3)
                {
                    quadrent_C->refine(func, precision_factor);
                    if(quadrent_C->state==2)
                    {
                        delete quadrent_A;
                        delete quadrent_B;
                        delete quadrent_C;
                        delete quadrent_D;
                        quadrent_A=0;
                        quadrent_B=0;
                        quadrent_C=0;
                        quadrent_D=0;

                        state=5;
                    }
                }
                if(state==3)
                {
                    quadrent_D->refine(func, precision_factor);
                    if(quadrent_D->state==2)
                    {
                        delete quadrent_A;
                        delete quadrent_B;
                        delete quadrent_C;
                        delete quadrent_D;
                        quadrent_A=0;
                        quadrent_B=0;
                        quadrent_C=0;
                        quadrent_D=0;

                        state=5;
                    }
                }

                //check final state
                if(state==3)
                {
                if(quadrent_D->state==5 or quadrent_D->state==4 or quadrent_C->state==5 or quadrent_C->state==4 or quadrent_B->state==5 or quadrent_B->state==4 or quadrent_A->state==5 or quadrent_A->state==4)
                {
                    state=4;
                }
                }
            }

            if(state==5 or state==1) //doing triangles
            {
                //print("tri at level:", level);

                ////make
                upper_tri= new tri_section();
                left_tri= new tri_section();
                lower_tri= new tri_section();
                right_tri= new tri_section();

                //set
                upper_tri->type=1; //0 is border is to left, 1 is up, 2 is down 3 is to right
                upper_tri->bordering_quad = upper_neighbor; //may be null

                upper_tri->X1=X1;
                upper_tri->Y1=Y1;
                upper_tri->F1=F1;

                upper_tri->X2=X2;
                upper_tri->Y2=Y1;
                upper_tri->F2=F2;

                upper_tri->X3=middle_X;
                upper_tri->Y3=middle_Y;
                upper_tri->F3=middle_value;


                left_tri->type=0; //0 is border is to left, 1 is up, 2 is down 3 is to right
                left_tri->bordering_quad = left_neighbor; //may be null

                left_tri->X1=X1;//lower or left
                left_tri->Y1=Y2;
                left_tri->F1=F4;

                left_tri->X2=X1;
                left_tri->Y2=Y1;
                left_tri->F2=F1;

                left_tri->X3=middle_X;
                left_tri->Y3=middle_Y;
                left_tri->F3=middle_value;


                lower_tri->type=2; //0 is border is to left, 1 is up, 2 is down 3 is to right
                lower_tri->bordering_quad = lower_neighbor; //may be null

                lower_tri->X1=X1;//lower or left
                lower_tri->Y1=Y2;
                lower_tri->F1=F4;

                lower_tri->X2=X2;
                lower_tri->Y2=Y2;
                lower_tri->F2=F3;

                lower_tri->X3=middle_X;
                lower_tri->Y3=middle_Y;
                lower_tri->F3=middle_value;


                right_tri->type=3; //0 is border is to left, 1 is up, 2 is down 3 is to right
                right_tri->bordering_quad = right_neighbor; //may be null

                right_tri->X1=X2;//lower or left
                right_tri->Y1=Y2;
                right_tri->F1=F3;

                right_tri->X2=X2;
                right_tri->Y2=Y1;
                right_tri->F2=F2;

                right_tri->X3=middle_X;
                right_tri->Y3=middle_Y;
                right_tri->F3=middle_value;

                //set weights
                upper_tri->set_weights();
                left_tri->set_weights();
                lower_tri->set_weights();
                right_tri->set_weights();
            }
            //////DONE!!!!
        }

        void triangularize()
        //void triangularize()
        {
            if(state==3 or state==4)//have quads
            {
                //double check neighbors

                if(upper_neighbor and (upper_neighbor->state==3 or upper_neighbor->state==4))
                {
                    quadrent_A->upper_neighbor=upper_neighbor->quadrent_D;
                    quadrent_B->upper_neighbor=upper_neighbor->quadrent_C;
                }

                if(right_neighbor and (right_neighbor->state==3 or right_neighbor->state==4))
                {
                    quadrent_B->right_neighbor=right_neighbor->quadrent_A;
                    quadrent_C->right_neighbor=right_neighbor->quadrent_D;
                }

                if(lower_neighbor and (lower_neighbor->state==3 or lower_neighbor->state==4))
                {
                    quadrent_C->lower_neighbor=lower_neighbor->quadrent_B;
                    quadrent_D->lower_neighbor=lower_neighbor->quadrent_A;
                }

                if(left_neighbor and (left_neighbor->state==3 or left_neighbor->state==4))
                {
                    quadrent_D->left_neighbor=left_neighbor->quadrent_C;
                    quadrent_A->left_neighbor=left_neighbor->quadrent_B;
                }

                quadrent_A->triangularize();
                quadrent_B->triangularize();
                quadrent_C->triangularize();
                quadrent_D->triangularize();
            }
            else if(state==5 or state==1)//have triangles
            {
                upper_tri->bordering_quad=upper_neighbor;
                upper_tri->triangularize();

                left_tri->bordering_quad=left_neighbor;
                left_tri->triangularize();

                lower_tri->bordering_quad=lower_neighbor;
                lower_tri->triangularize();

                right_tri->bordering_quad=right_neighbor;
                right_tri->triangularize();
            }
        }

        inline double call(double X, double Y)
        {

            if(state==3 or state==4)//have quads
            {
                if(Y>=middle_Y)
                {
                    if(X>=middle_X)
                    {
                        return quadrent_B->call(X,Y);
                    }
                    else
                    {
                        return quadrent_A->call(X,Y);
                    }
                }
                else
                {
                    if(X>=middle_X)
                    {
                        return quadrent_C->call(X,Y);
                    }
                    else
                    {
                        return quadrent_D->call(X,Y);
                    }
                }
            }

            else if(state==1 or state==5)//have tris
            {

                if(Y>=middle_Y)
                {
                    if(X>=middle_X)//in quad B. could be upper tri or right tri
                    {
                        if(upper_tri->intersection(X,Y))
                        {
                            return upper_tri->call(X,Y);
                        }
                        else
                        {
                            return right_tri->call(X,Y);
                        }
                    }
                    else //quad A. upper tri or left tri
                    {
                        if(upper_tri->intersection(X,Y))
                        {
                            return upper_tri->call(X,Y);
                        }
                        else
                        {
                            return left_tri->call(X,Y);
                        }
                    }
                }
                else
                {
                    if(X>=middle_X) //quad C. lower tri or right tri
                    {
                        if(lower_tri->intersection(X,Y))
                        {
                            return lower_tri->call(X,Y);
                        }
                        else
                        {
                            return right_tri->call(X,Y);
                        }
                    }
                    else //quad D. lower tri or left tri
                    {
                        if(lower_tri->intersection(X,Y))
                        {
                            return lower_tri->call(X,Y);
                        }
                        else
                        {
                            return left_tri->call(X,Y);
                        }
                    }
                }
            }
        }

        void integrate_along_Y(std::shared_ptr<poly_spline>& IN, spline_list& out)
        {
            if(state==3 or state==4)
            {
                quadrent_A->integrate_along_Y(IN, out);
                quadrent_B->integrate_along_Y(IN, out);
                quadrent_C->integrate_along_Y(IN, out);
                quadrent_D->integrate_along_Y(IN, out);
            }
            else if(state==1 or state==5)
            {
                left_tri->integrate_along_Y(IN, out);
                right_tri->integrate_along_Y(IN, out);
                upper_tri->integrate_along_Y(IN, out);
                lower_tri->integrate_along_Y(IN, out);
            }
        }
    };

    quad_section* top_quad;


    template<typename functor_T>
    adaptive_2DSpline(functor_T& func, double precision_factor, double Xlower, double Ylower, double Xupper, double Yupper)
    {
        top_quad=new quad_section();

        top_quad->X1=Xlower;
        top_quad->Y1=Yupper;
        top_quad->X2=Xupper;
        top_quad->Y2=Ylower;

        top_quad->F1=func(Xlower, Yupper);
        top_quad->F2=func(Xupper, Yupper);
        top_quad->F3=func(Xupper, Ylower);
        top_quad->F4=func(Xlower, Ylower);


//print("P.append( [", top_quad->X1, ",", top_quad->Y1, ",", top_quad->F1, "])");

//print("P.append( [", top_quad->X2, ",", top_quad->Y1, ",", top_quad->F2, "])");

//print("P.append( [", top_quad->X2, ",", top_quad->Y2, ",", top_quad->F3, "])");

//print("P.append( [", top_quad->X1, ",", top_quad->Y2, ",", top_quad->F4, "])");

        top_quad->level=1;

        top_quad->refine(func, precision_factor);
        top_quad->triangularize();
    }

    ~adaptive_2DSpline()
    {
        delete top_quad;
    }

    double call(double X, double Y)
    {
        //bounds checking
        //if(X<top_quad->X1 or X>top_quad->X2 or Y>top_quad->Y1 or Y<top_quad->Y2)
        //{
            //throw gen_exception("X Y point is out of bounds of 2D spline. X:", X, "( ",top_quad->X1 , ", ", top_quad->X2, ") Y:" , Y, "( " , top_quad->Y2 , ", ", top_quad->Y1, ")");
        //}

        //recursivly find the lowest quad
        quad_section* current_quad=top_quad;
        while(current_quad->state==3 or current_quad->state==4)//while current quad has lower quads
        {
            if(Y>=current_quad->middle_Y)
            {
                if(X>=current_quad->middle_X)
                {
                    current_quad=current_quad->quadrent_B;
                }
                else
                {
                    current_quad=current_quad->quadrent_A;
                }
            }
            else
            {
                if(X>=current_quad->middle_X)
                {
                    current_quad=current_quad->quadrent_C;
                }
                else
                {
                    current_quad=current_quad->quadrent_D;
                }
            }
        }

        //now current quad has tri. Find the right one
        tri_section* current_tri;
        if(Y>=current_quad->middle_Y)
        {
            if(X>=current_quad->middle_X)//in quad B. could be upper tri or right tri
            {
                if(current_quad->upper_tri->intersection(X,Y))
                {
                    current_tri=current_quad->upper_tri;
                }
                else
                {
                    current_tri=current_quad->right_tri;
                }
            }
            else //quad A. upper tri or left tri
            {
                if(current_quad->upper_tri->intersection(X,Y))
                {
                    current_tri=current_quad->upper_tri;
                }
                else
                {
                    current_tri=current_quad->left_tri;
                }
            }
        }
        else
        {
            if(X>=current_quad->middle_X) //quad C. lower tri or right tri
            {
                if(current_quad->lower_tri->intersection(X,Y))
                {
                    current_tri=current_quad->lower_tri;
                }
                else
                {
                    current_tri=current_quad->right_tri;
                }
            }
            else //quad D. lower tri or left tri
            {
                if(current_quad->lower_tri->intersection(X,Y))
                {
                    current_tri=current_quad->lower_tri;
                }
                else
                {
                    current_tri=current_quad->left_tri;
                }
            }
        }

        //now we recursivly try to find the lowest try
        while(current_tri->section_A)
        {
            if(current_tri->section_A->intersection(X,Y))
            {
                current_tri=current_tri->section_A;
            }
            else
            {
                current_tri=current_tri->section_B;
            }
        }

        //finally we have the lowest tri!
        return current_tri->rough_call(X,Y);
    }

    std::shared_ptr<poly_spline> integrate_along_Y(std::shared_ptr<poly_spline> IN)
    {
        spline_list sp_list;
        top_quad->integrate_along_Y(IN, sp_list);
        return convert(sp_list);
    }
};

#endif
