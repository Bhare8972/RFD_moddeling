
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

class poly_spline : public functor_1D //we should extend this to work for first order as well
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
                ret+=weights[i]*F;
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
		reset(X, Y);
	}

	void reset(gsl::vector X, gsl::vector Y)
	{
		size_t num_points=Y.size();
		if( num_points != X.size())
		{
			throw gen_exception("X array and Y array must have the same size");
		}
		if( num_points < 3)
		{
			throw gen_exception("array sizes must be greater than 2");
		}

		size_t num_quad_splines=size_t((num_points-1)/2);
		bool do_linear_spline=(num_points-1) % 2;

		x_vals=gsl::vector(num_quad_splines+1+int(do_linear_spline));
		splines.clear();
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

            //pad so that all splines have same power
            //fix this so that this spline is also a quadratic. Perhaps match slope of previous spline
			auto tmp=splines.back().weights;
			splines.back().weights=gsl::vector(3);
			splines.back().weights[0]=tmp[0];
			splines.back().weights[1]=tmp[1];
			splines.back().weights[2]=0;
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

		size_t spline_index;
		if(X==x_vals[x_vals.size()-1])
        {
            spline_index=splines.size()-1;
        }
        else
        {
            spline_index=search_sorted_d(x_vals, X);
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
//takes two vectors, X and Y, and removes points where the X values are equal, and stores results in X_new and Y_new
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

    void refine_AbsComp(double abs_compare)
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
        if( float( abs_compare + float( left_data->middle_value - this->rough_call(left_data->middle_point)))  != float(abs_compare) )
        {
            left_data->refine_AbsComp(abs_compare);

            if(left_data->stage==3 or left_data->stage==4)
            {
                stage=4;
            }
        }

        //check if we need to refine the right
        if( float( abs_compare + float( right_data->middle_value - this->rough_call(right_data->middle_point)))  != float(abs_compare) )
        {
            right_data->refine_AbsComp(abs_compare);

            if(right_data->stage==3 or right_data->stage==4)
            {
                stage=4;
            }
        }
    }

    void refine_RelComp(double rel_compare)
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
        if( float( left_data->middle_value*rel_compare + float( left_data->middle_value - this->rough_call(left_data->middle_point)))  != float(left_data->middle_value*rel_compare) )
        {
            left_data->refine_RelComp(rel_compare);

            if(left_data->stage==3 or left_data->stage==4)
            {
                stage=4;
            }
        }

        //check if we need to refine the right
        if( float( right_data->middle_value*rel_compare + float( right_data->middle_value - this->rough_call(right_data->middle_point)))  != float(right_data->middle_value*rel_compare) )
        {
            right_data->refine_RelComp(rel_compare);

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

gsl::vector adaptive_sample(functor_1D* F, double precision, double start, double stop, gsl::vector &points, int precision_type=0 )
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    if(precision_type==0)
    {
        sampler.refine(precision); //type 0 is where precision is just relative error. (default, but error prone)
    }
    else if(precision_type==1)
    {
        sampler.refine_AbsComp(precision); //type 1 is where the differnce is zero comparted to precision
    }
    else if(precision_type==2)
    {
        sampler.refine_RelComp(precision); //same method as adaptive integration
    }



    size_t num_points=sampler.size();
    points=gsl::vector(num_points);
    gsl::vector values=gsl::vector(num_points);
    sampler.get_points(points.ptr(0));
    sampler.get_values(values.ptr(0));

    points[points.size()-1]=stop;
    values[values.size()-1]=right_value;

    return values;
}

std::shared_ptr<poly_spline> adaptive_sample_retSpline(functor_1D* F, double precision, double start, double stop, int precision_type=0  )
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    if(precision_type==0)
    {
        sampler.refine(precision); //type 0 is where precision is just relative error. (default, but error prone)
    }
    else if(precision_type==1)
    {
        sampler.refine_AbsComp(precision); //type 1 is where the differnce is zero comparted to precision
    }
    else if(precision_type==2)
    {
        sampler.refine_RelComp(precision); //same method as adaptive integration
    }



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

std::shared_ptr<poly_spline> adaptive_sample_all(functor_1D* F, double precision, double start, double stop, gsl::vector &points, gsl::vector &values, int precision_type=0)
{
    double left_value=F->call(start);
    double right_value=F->call(stop);

    adaptive_sampler_data sampler(F, start, stop, left_value, right_value);
    if(precision_type==0)
    {
        sampler.refine(precision); //type 0 is where precision is just relative error. (default, but error prone)
    }
    else if(precision_type==1)
    {
        sampler.refine_AbsComp(precision); //type 1 is where the differnce is zero comparted to precision
    }
    else if(precision_type==2)
    {
        sampler.refine_RelComp(precision); //same method as adaptive integration
    }



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


void test_spline(std::shared_ptr<poly_spline> input_spline, size_t points_per_spline, gsl::vector &points, gsl::vector &values )
//sample a spline at a number of points that should be representive of the shape of the spline
{
    size_t num_points=points_per_spline*input_spline->splines.size();
    points=gsl::vector(num_points);
    values=gsl::vector(num_points);

    for(size_t si=0; si<input_spline->splines.size(); si++)
    {
        double X=input_spline->x_vals[si];
        double X_delta=(input_spline->x_vals[si+1]-input_spline->x_vals[si])/points_per_spline;
        for(size_t pi=0; pi<points_per_spline; pi++)
        {
            points[si*points_per_spline+pi]=X;
            values[si*points_per_spline+pi]=input_spline->splines[si].y(X);
            X+=X_delta;
        }
    }
}

double multiply_integrate(std::shared_ptr<poly_spline> splineA, std::shared_ptr<poly_spline> splineB, double start, double stop)
//find the integral of splineA * splineB from start to stop
//notice the ability to multiply splines!! which can, should, be extended
{
    if(start< splineA->x_vals[0] )
    {
        throw gen_exception("invalid lower bound of: ", start, " first spline has lowest X of ", splineA->x_vals[0] );
    }
    if(start< splineB->x_vals[0] )
    {
        throw gen_exception("invalid lower bound of: ", start, " second spline has lowest X of ", splineB->x_vals[0] );
    }

    if(stop > splineB->x_vals[ splineB->x_vals.size()-1 ] )
    {
        throw gen_exception("invalid upper bound of: ", stop, " second spline has highest X of ", splineB->x_vals[ splineB->x_vals.size()-1 ] );
    }

    if(stop > splineA->x_vals[ splineA->x_vals.size()-1 ] )
    {
        throw gen_exception("invalid upper bound of: ", stop, " first spline has highest X of ", splineA->x_vals[ splineA->x_vals.size()-1 ] );
    }

    ////first iteration
    double low_x=start;
    size_t A_index=search_sorted_d(splineA->x_vals, start);
    size_t B_index=search_sorted_d(splineB->x_vals, start);

    bool X_limited_by_A=true;
    double high_x=splineA->x_vals[A_index+1];

    if(splineB->x_vals[B_index+1] < high_x)
    {
        X_limited_by_A=false;
        high_x=splineB->x_vals[B_index+1];
    }

    bool completed=false;
    if(stop<high_x)
    {
        high_x=stop;
        completed=true;
    }

    double ret=0;
    double low_X_power=low_x;
    double high_X_power=high_x;

    int SA_power=splineA->splines[A_index].weights.size()-1;
    int SB_power=splineB->splines[B_index].weights.size()-1;
    int max_power=SA_power*SB_power;
    for(int power_index=0; power_index<=max_power; power_index++)
    {
        double weight=0;
        for(int SA_power_index=std::min(power_index, SA_power); SA_power_index>=0; SA_power_index--)
        {
            int SB_power_index=power_index-SA_power_index;
            if(SB_power_index>SB_power) break;

            weight+=splineA->splines[A_index].weights[SA_power_index]*splineB->splines[B_index].weights[SB_power_index];
        }

        weight/=(power_index+1); //since we are integrated. Otherwords this would be the weight for the resultant spline
        ret+=weight*(high_X_power-low_X_power);
        low_X_power*=low_x;
        high_X_power*=high_x;
    }

    ////middle iterations
    while(not completed)
    {

        if(X_limited_by_A)
        {
            A_index++;
        }
        else
        {
            B_index++;
        }

        low_x=high_x;

        X_limited_by_A=true;
        high_x=splineA->x_vals[A_index+1];
        if(splineB->x_vals[B_index+1] < high_x)
        {
            X_limited_by_A=false;
            high_x=splineB->x_vals[B_index+1];
        }
        if(stop<high_x)
        {
            high_x=stop;
            completed=true;
        }
        low_X_power=low_x;
        high_X_power=high_x;

        SA_power=splineA->splines[A_index].weights.size()-1;
        SB_power=splineB->splines[B_index].weights.size()-1;
        max_power=SA_power*SB_power;

        for(int power_index=0; power_index<=max_power; power_index++)
        {
            double weight=0;
            for(int SA_power_index=std::min(power_index, SA_power); SA_power_index>=0; SA_power_index--)
            {
                int SB_power_index=power_index-SA_power_index;
                if(SB_power_index>SB_power) break;

                weight+=splineA->splines[A_index].weights[SA_power_index]*splineB->splines[B_index].weights[SB_power_index];
            }

            weight/=(power_index+1); //since we are integrated. Otherwords this would be the weight for the resultant spline
            ret+=weight*(high_X_power-low_X_power);

            low_X_power*=low_x;
            high_X_power*=high_x;
        }
    }
    return ret;
}

/*
double weighted_addition(std::shared_ptr<poly_spline> splineA, std::shared_ptr<poly_spline> splineB, double weightA, double weightB)
//add two splines splineA*weightA + splineB*weightB
{
    class tmp_splines
    {
        public:
        double lower_bound;
        double upper_bound;
        gsl::vector weights;

        tmp_splines(double lower_bound_, double upper_bound_, int size)
        {
            lower_bound=lower_bound_;
            upper_bound=upper_bound_;
            weights=make_vector(size);
        }

        void add_weights(gsl::vector new_spline, double weight)
        {
            for(int i=0; i<new_spline.size(); i++)
            {
                AM HERE
            }
        }
    }

    ////first iteration
    double low_x=start;
    size_t A_index=search_sorted_d(splineA->x_vals, start);
    size_t B_index=search_sorted_d(splineB->x_vals, start);

    bool X_limited_by_A=true;
    double high_x=splineA->x_vals[A_index+1];

    if(splineB->x_vals[B_index+1] < high_x)
    {
        X_limited_by_A=false;
        high_x=splineB->x_vals[B_index+1];
    }

    bool completed=false;
    if(stop<high_x)
    {
        high_x=stop;
        completed=true;
    }

    double ret=0;
    double low_X_power=low_x;
    double high_X_power=high_x;

    int SA_power=splineA->splines[A_index].weights.size()-1;
    int SB_power=splineB->splines[B_index].weights.size()-1;
    int max_power=SA_power*SB_power;
    for(int power_index=0; power_index<=max_power; power_index++)
    {
        double weight=0;
        for(int SA_power_index=std::min(power_index, SA_power); SA_power_index>=0; SA_power_index--)
        {
            int SB_power_index=power_index-SA_power_index;
            if(SB_power_index>SB_power) break;

            weight+=splineA->splines[A_index].weights[SA_power_index]*splineB->splines[B_index].weights[SB_power_index];
        }

        weight/=(power_index+1); //since we are integrated. Otherwords this would be the weight for the resultant spline
        ret+=weight*(high_X_power-low_X_power);
        low_X_power*=low_x;
        high_X_power*=high_x;
    }

    ////middle iterations
    while(not completed)
    {

        if(X_limited_by_A)
        {
            A_index++;
        }
        else
        {
            B_index++;
        }

        low_x=high_x;

        X_limited_by_A=true;
        high_x=splineA->x_vals[A_index+1];
        if(splineB->x_vals[B_index+1] < high_x)
        {
            X_limited_by_A=false;
            high_x=splineB->x_vals[B_index+1];
        }
        if(stop<high_x)
        {
            high_x=stop;
            completed=true;
        }
        low_X_power=low_x;
        high_X_power=high_x;

        SA_power=splineA->splines[A_index].weights.size()-1;
        SB_power=splineB->splines[B_index].weights.size()-1;
        max_power=SA_power*SB_power;

        for(int power_index=0; power_index<=max_power; power_index++)
        {
            double weight=0;
            for(int SA_power_index=std::min(power_index, SA_power); SA_power_index>=0; SA_power_index--)
            {
                int SB_power_index=power_index-SA_power_index;
                if(SB_power_index>SB_power) break;

                weight+=splineA->splines[A_index].weights[SA_power_index]*splineB->splines[B_index].weights[SB_power_index];
            }

            weight/=(power_index+1); //since we are integrated. Otherwords this would be the weight for the resultant spline
            ret+=weight*(high_X_power-low_X_power);

            low_X_power*=low_x;
            high_X_power*=high_x;
        }
    }
    return ret;
}*/

#endif
