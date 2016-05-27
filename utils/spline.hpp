
#ifndef SPLINE_HPP
#define SPLINE_HPP

#include <memory>
#include <list>
#include <vector>
//#include <gsl/gsl_spline.h>

#include "GSL_utils.hpp"
#include "vector.hpp"
#include "functor.hpp"
#include "gen_ex.hpp"

class poly_spline : public functor_1D //we should exted this to work for first order as well
{
public:
	class spline_piece
	{
	public:
		gsl::vector weights;

		spline_piece(const poly_spline::spline_piece &SP)
		{
            weights=SP.weights.clone();
		}

        spline_piece(const gsl::vector &weights_)
		{
            weights=weights_;
		}

		spline_piece(double left_point, double middle_point, double right_point, double left_value, double middle_value, double right_value)
		{
            //second order spline
            weights=gsl::vector(3);

			double weight_three_num=(middle_point-left_point)*(right_value-left_value) - (right_point-left_point)*(middle_value-left_value);
			double weight_three_denom=(middle_point-left_point)*(right_point*right_point - left_point*left_point) - (right_point-left_point)*(middle_point*middle_point-left_point*left_point);
			weights[2]=weight_three_num/weight_three_denom;
			weights[1]=(middle_value-left_value)/(middle_point-left_point) - weights[2]*(middle_point*middle_point-left_point*left_point)/(middle_point-left_point);
			weights[0]=left_value - weights[2]*left_point*left_point - weights[1]*left_point;

			if(weights[2]!=weights[2] or weights[1]!=weights[1] or weights[0]!=weights[0] or std::isinf(weights[2]) or std::isinf(weights[1]) or std::isinf(weights[0]))
			{
                print("second order spline");
                print(left_point, middle_point, right_point);
			    throw gen_exception("function cannot be represented by a spline");
			}
		}

		spline_piece(double left_point, double right_point, double left_value, double right_value)
		{
            //first order spline
            weights=gsl::vector(2);

			weights[1]=(right_value-left_value)/(right_point-left_point);
			weights[0]=left_value - weights[1]*left_point;

			if(weights[1]!=weights[1] or weights[0]!=weights[0] or std::isinf(weights[1]) or std::isinf(weights[0]))
			{
                print("first order spline");
                print(left_point, right_point);
			    throw gen_exception("function cannot be represented by a spline");
			}
		}

		double y(double x)
		{
            double ret=weights[0];
            double F=x;
            for(unsigned int i=1; i<weights.size(); i++)
            {
                ret=weights[i]*F;
                F*=x;
            }
            return ret;
		}
	};

	std::vector<spline_piece> splines;
	gsl::vector x_vals; //length is one greater than splines

	double lower_fill;
	double upper_fill;

    poly_spline(){}

	poly_spline(gsl::vector X, gsl::vector Y)
	{
		size_t num_points=Y.size();
		if( num_points != X.size())
		{
			throw gen_exception("X array and Y array must have the same size");
		}
		if( num_points < 2)
		{
			throw gen_exception("array sizes must be greater than 1");
		}

		size_t num_quad_splines=size_t((num_points-1)/2);
		bool do_linear_spline=(num_points-1) % 2;

		x_vals=gsl::vector(num_quad_splines+1+int(do_linear_spline));
		splines.reserve(num_quad_splines+int(do_linear_spline));

		for(size_t pi=0, si=0; num_points>2 and pi<(num_points-2-int(do_linear_spline)); pi+=2)
		{
            x_vals[si]=X[pi];
			splines.emplace_back(X[pi], X[pi+1], X[pi+2], Y[pi], Y[pi+1], Y[pi+2]);
			si++;
		}
		if(do_linear_spline)
		{
            size_t pi=X.size()-2;
            x_vals[num_quad_splines]=X[pi];
			splines.emplace_back(X[pi], X[pi+1], Y[pi], Y[pi+1]);
		}
		x_vals[num_quad_splines+int(do_linear_spline)]=X[X.size()-1];

		lower_fill=std::nan("");
        upper_fill=std::nan("");
	}

	void set_lower_fill(double v)
	{
        lower_fill=v;
	}

	void set_upper_fill(double v)
	{
        upper_fill=v;
	}

	void set_lower_fill()
	{
        lower_fill=call(x_vals[0]);
	}

	void set_upper_fill()
	{
        upper_fill=call( x_vals[ x_vals.size()-1]);
	}

	double call(double X)
	{
		if(X<x_vals[0])
		{
            if(lower_fill!=lower_fill)
            {
                throw gen_exception("value: ",X," is below range");
            }
            else
            {
                return lower_fill;
            }
        }

		if(X>x_vals[x_vals.size()-1])
		{
            if(upper_fill!=upper_fill)
            {
                throw gen_exception("value: ",X," is above range");
            }
            else
            {
                return upper_fill;
            }
        }

		size_t spline_index=search_sorted_d(x_vals, X);
		if(X==x_vals[x_vals.size()-1])
        {
            spline_index=splines.size()-1;
        }

		double Y= splines[spline_index].y(X);

		return Y;
	}

	void multiply(double V)
	{
        for(auto &sp : splines)
        {
            for(unsigned int i=0; i<sp.weights.size(); i++)
            {
                sp.weights[i]*=V;
            }
        }
	}

    void add(double V)
	{
        for(auto &sp : splines)
        {
            sp.weights[0]+=V;
        }
	}

	std::shared_ptr<poly_spline> integrate()
	{
        auto out=std::make_shared<poly_spline>();
		out->lower_fill=std::nan("");
        out->upper_fill=std::nan("");
        out->x_vals=x_vals.clone();
        out->splines.reserve(x_vals.size()-1);
        double total_integral=0;
        for(unsigned int sp_i=0; sp_i<(x_vals.size()-1); sp_i++ )
        {
            gsl::vector new_weights( splines[sp_i].weights.size() + 1 );
            new_weights[0]=total_integral;
            double lower_X=x_vals[sp_i];
            double upper_X=x_vals[sp_i+1];

            for(unsigned int w_i=0; w_i<splines[sp_i].weights.size(); w_i++)
            {
                new_weights[w_i+1]=splines[sp_i].weights[w_i]/(w_i+1);
                total_integral+=new_weights[w_i+1]*(upper_X - lower_X);
                lower_X*=x_vals[sp_i];
                upper_X*=x_vals[sp_i+1];
            }

            out->splines.emplace_back(new_weights);
        }
        return out;
	}

	double integrate(double X)
	{

		if(X<x_vals[0])
		{
            return 0;
        }

		else if(X>=x_vals[x_vals.size()-1])
		{
            double integrand_value=0;
            for(unsigned int i=0; i<splines.size(); i++)
            {
                double lower_X=x_vals[i];
                double upper_X=x_vals[i+1];

                for(unsigned int weight_i=0; weight_i< splines[i].weights.size(); weight_i++)
                {
                    integrand_value+= splines[i].weights[weight_i]*(upper_X-lower_X)/(weight_i+1);
                    lower_X*=x_vals[i];
                    upper_X*=x_vals[i+1];
                }
            }

            return integrand_value;
        }

        else
        {
            size_t spline_index=search_sorted_d(x_vals, X);
            double integrand_value=0;
            for(unsigned int i=0; i<spline_index; i++)
            {
                double lower_X=x_vals[i];
                double upper_X=x_vals[i+1];

                for(unsigned int weight_i=0; weight_i< splines[i].weights.size(); weight_i++)
                {
                    integrand_value+= splines[i].weights[weight_i]*(upper_X-lower_X)/(weight_i+1);
                    lower_X*=x_vals[i];
                    upper_X*=x_vals[i+1];
                }
            }

            double lower_X=x_vals[spline_index];
            double upper_X=X;

            for(unsigned int weight_i=0; weight_i< splines[spline_index].weights.size(); weight_i++)
            {
                integrand_value+= splines[spline_index].weights[weight_i]*(upper_X-lower_X)/(weight_i+1);
                lower_X*=x_vals[spline_index];
                upper_X*=X;
            }

            return integrand_value;
        }
	}


};

void make_fix_spline(gsl::vector X, gsl::vector Y, gsl::vector &X_new, gsl::vector &Y_new)
//takes two vectors, X adn Y, and removes points where the X values are equal, and stores results in X_new and Y_new
{
    size_t num_points=Y.size();
    if( num_points != X.size())
    {
        throw gen_exception("X array and Y array must have the same size");
    }
    if( num_points < 2)
    {
        throw gen_exception("array sizes must be greater than 2");
    }

    std::list<double> new_x;
    std::list<double> new_y;
    new_x.push_back(X[0]);
    new_y.push_back(Y[0]);

    double last_X=X[0];
    for(size_t pi=1; pi<num_points; pi++)
    {
        if( float(last_X+(last_X-X[pi])) != float(last_X) )
        {
            new_x.push_back(X[pi]);
            new_y.push_back(Y[pi]);
            last_X=X[pi];
        }
        //else
        //{
          //  print("rejected:", last_X, X[pi]);
        //}
    }

    X_new=make_vector(new_x);
    Y_new=make_vector(new_y);
}


class adaptive_sampler_data
{
private:
    functor_1D* function;

    double left_point;
    double right_point;
    double left_value;
    double right_value;

    double middle_point;
    double middle_value;

    double weight_one;
    double weight_two;
    double weight_three;

    std::shared_ptr<adaptive_sampler_data> left_data;
    std::shared_ptr<adaptive_sampler_data> right_data;

public:

    int stage;//0 means good, but does not have subsections, 1 has subsections. 2 is in error, 3 tried subsections, but lower subsections had error, 4 one of the recursivly lower subsections is a 3

    adaptive_sampler_data(functor_1D* functor, double left_point_, double right_point_, double left_value_, double right_value_)
    {
        function=functor;
        left_point=left_point_;
        right_point=right_point_;
        left_value=left_value_;
        right_value=right_value_;

        middle_point=(left_point+right_point)/2.0;
        middle_value=functor->call(middle_point);

        if(float(left_point+(left_point-right_point))==float(left_point))
        {
            stage=2;
            return;
        }

        if(float(middle_point+(middle_point-right_point))==float(middle_point))
        {
            stage=2;
            return;
        }

        if(float(middle_point+(middle_point-left_point))==float(middle_point))
        {
            stage=2;
            return;
        }

        double weight_three_num=(middle_point-left_point)*(right_value-left_value) - (right_point-left_point)*(middle_value-left_value);
        double weight_three_denom=(middle_point-left_point)*(right_point*right_point - left_point*left_point) - (right_point-left_point)*(middle_point*middle_point-left_point*left_point);
        weight_three=weight_three_num/weight_three_denom;
        weight_two=(middle_value-left_value)/(middle_point-left_point) - weight_three*(middle_point*middle_point-left_point*left_point)/(middle_point-left_point);
        weight_one=left_value - weight_three*left_point*left_point - weight_two*left_point;

        stage=0;
    }

    double rough_call(double X)
    {
        return weight_one + weight_two*X + weight_three*X*X;
    }

    double call(double X)
    {
        if(X<middle_point and left_data)
        {
            return left_data->call(X);
        }
        else if( X>middle_point and right_data )
        {
            return right_data->call(X);
        }
        else
        {
            return rough_call(X);
        }
    }

    void refine(double percent_error)
    {
        // step one, make the next lower two simps rules
        left_data=std::make_shared<adaptive_sampler_data>(function, left_point, middle_point, left_value, middle_value);
        right_data=std::make_shared<adaptive_sampler_data>(function, middle_point, right_point, middle_value, right_value);

        if(left_data->stage==2 or right_data->stage==2)
        {
            left_data.reset();
            right_data.reset();
            stage=3;

            return;
        }
        stage=1;

        //check if we need to refine the left
        if( std::abs( left_data->middle_value - this->rough_call(left_data->middle_point) ) > percent_error*std::abs(left_data->middle_value) )
        {
            left_data->refine(percent_error);

            if(left_data->stage==3 or left_data->stage==4)
            {
                stage=4;
            }
        }

        //check if we need to refine the right
        if( std::abs( right_data->middle_value - this->rough_call(right_data->middle_point) ) > percent_error*std::abs(right_data->middle_value) )
        {
            right_data->refine(percent_error);

            if(right_data->stage==3 or right_data->stage==4)
            {
                stage=4;
            }
        }
    }

    size_t size()
    {
        if(left_data) //if this exists, then we have made the two sections
        {
            return left_data->size() + right_data->size()-1;//one point is shared
        }
        else
        {
            return 3;
        }
    }

    void get_points(double* points)
    {
        unsigned int offset=1;
        if(left_data)
        {
            left_data->get_points(points);
            offset=left_data->size()-1;
        }
        else
        {
            points[0]=left_point;
        }

        if(right_data)
        {
            right_data->get_points(points+offset);
        }
        else
        {
            points[offset]=middle_point;
        }
        //don't do last point
    }

    void get_values(double* values)
    {
        unsigned int offset=1;
        if(left_data)
        {
            left_data->get_values(values);
            offset=left_data->size()-1;
        }
        else
        {
            values[0]=left_value;
        }

        if(right_data)
        {
            right_data->get_values(values+offset);
        }
        else
        {
            values[offset]=middle_value;
        }
        //don't do last point
    }

    std::list<poly_spline::spline_piece> get_spline()
    {
        if(left_data)//if we have left_data, we have right as well
        {
            auto A=left_data->get_spline();
            auto B=right_data->get_spline();
            A.splice(A.end(), B, B.begin(), B.end());
            return A;
        }
        else
        {
            gsl::vector weights({weight_one, weight_two, weight_three});
            std::list<poly_spline::spline_piece> ret;
            ret.emplace_back(weights);
            return ret;
        }
    }

    std::list<double> get_spline_points()
    {
        if(left_data)//if we have left_data, we have right as well
        {
            auto A=left_data->get_spline_points();
            auto B=right_data->get_spline_points();
            A.splice(A.end(), B, B.begin(), B.end());
            return A;
        }
        else
        {
            std::list<double> ret;
            ret.emplace_back(left_point);
            return ret;
        }
    }
};

gsl::vector adaptive_sample(functor_1D* F, double precision, double start, double stop, gsl::vector &points )
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    sampler.refine(precision);



    size_t num_points=sampler.size();
    points=gsl::vector(num_points);
    gsl::vector values=gsl::vector(num_points);
    sampler.get_points(points.ptr(0));
    sampler.get_values(values.ptr(0));

    points[points.size()-1]=stop;
    values[values.size()-1]=right_value;

    return values;
}

std::shared_ptr<poly_spline> adaptive_sample_retSpline(functor_1D* F, double precision, double start, double stop  )
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    sampler.refine(precision);



    auto out_spline=std::make_shared<poly_spline>();
	out_spline->lower_fill=std::nan("");
	out_spline->upper_fill=std::nan("");

    auto spline_peices=sampler.get_spline();
    out_spline->splines.reserve(spline_peices.size());
    out_spline->splines.insert(out_spline->splines.end(), std::make_move_iterator(spline_peices.begin()),  std::make_move_iterator(spline_peices.end()) );

    auto spline_points=sampler.get_spline_points();
    out_spline->x_vals=gsl::vector(spline_peices.size()+1);
    int i=0;
    for(double v :  spline_points)
    {
        out_spline->x_vals[i]=v;
        i++;
    }
    out_spline->x_vals[i]=stop;

    return out_spline;
}



std::shared_ptr<poly_spline> adaptive_sample_all(functor_1D* F, double precision, double start, double stop, gsl::vector &points, gsl::vector &values  )
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    sampler.refine(precision);



    size_t num_points=sampler.size();
    points=gsl::vector(num_points);
    values=gsl::vector(num_points);
    sampler.get_points(points.ptr(0));
    sampler.get_values(values.ptr(0));

    points[points.size()-1]=stop;
    values[values.size()-1]=right_value;



    auto out_spline=std::make_shared<poly_spline>();
	out_spline->lower_fill=std::nan("");
	out_spline->upper_fill=std::nan("");

    auto spline_peices=sampler.get_spline();
    out_spline->splines.reserve(spline_peices.size());
    out_spline->splines.insert(out_spline->splines.end(), std::make_move_iterator(spline_peices.begin()),  std::make_move_iterator(spline_peices.end()) );

    auto spline_points=sampler.get_spline_points();
    out_spline->x_vals=gsl::vector(spline_peices.size()+1);
    int i=0;
    for(double v :  spline_points)
    {
        out_spline->x_vals[i]=v;
        i++;
    }
    out_spline->x_vals[i]=stop;

    return out_spline;
}

class adative_2DSpline
//not exactly memory effiecent
{
    public:
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

            if(type==0 or type==3)//verticle style tri
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
    };

    quad_section* top_quad;


    template<typename functor_T>
    adative_2DSpline(functor_T& func, double precision_factor, double Xlower, double Ylower, double Xupper, double Yupper)
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

    double call(double X, double Y)
    {
        //bounds checking
        if(X<top_quad->X1 or X>top_quad->X2 or Y<top_quad->Y2 or Y>top_quad->Y1)
        {
            throw gen_exception("X Y point is out of bounds of 2D spline");
        }

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
};

#endif
