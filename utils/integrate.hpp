
#include <vector>
#include <memory>

#include "GSL_utils.hpp"
#include "functor.hpp"

class cum_adap_simps_data
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
	
	std::shared_ptr<cum_adap_simps_data> left_data;
	std::shared_ptr<cum_adap_simps_data> right_data;
	
public:
	cum_adap_simps_data(functor_1D* functor, double left_point_, double, right_point_, double left_value_, double right_value_)
	{
		function=functor;
		left_point=left_point_;
		right_point=right_point_;
		left_value=left_value_;
		right_value=right_value_;
		
		middle_point=(left_value+right_value)/2.0;
		middle_value=functor->call(middle_point);
		
		double weight_three_num=(middle_point-left_point)*(right_value-left_value) - (right_point-left_point)*(middle_value-left_value);
		double weight_three_denom=(middle_point-left_point)*(right_point*right_point - left_point*left_point) - (right_point-left_point)*(middle_point*middle_point-left_point*left_point);
		weight_three=weight_three_num/weight_three_denom;
		weight_two=(middle_value-left_value)/(middle_point-left_point) - weight_three*(middle_point*middle_point-left_point*left_point)/(middle_point-left_point);
		weight_one=left_value - weight_three*left_point*left_point - weight_two*left_point;
	}
	
	double rough_quad()
	{
		return rough_quad(right_point);
	}
	
	double rough_quad(double point)
	{
		if(point<left_point) return 0.0;
		else if(point>right_point) return rough_quad(right_point);
		
		return weight_one*point + weight_two*point*point/2.0 + weight_three*point*point*point/3.0;
	}
	
	double quad()
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			return left_data->quad() + right_data->quad();
		}
		else
		{
			return rough_quad();
		}
	}
	
	double quad(double point)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			if(point<left_point)
			{
				return 0.0;
			}
			else if(point<middle_point)
			{
				return left_data->quad(point);
			}
			else if(point<right_point)
			{
				return left_data->quad() + right_data->quad(point);
			}
			else
			{
				return quad();
			}
		}
		else
		{
			return rough_quad(point);
		}
	}
	
	void refine(double guess)
	{
		// step one, make the next lower two simps rules
		left_data=std::make_shared<cum_adap_simps_data>(function, left_point, middle_point, left_value, middle_value);
		right_data=std::make_shared<cum_adap_simps_data>(function, middle_point, right_point, middle_value, right_value);
		
		//step two, if necisary, refine the lower two sub sections
		if( (guess-(rough_quad() - left_data->rough_quad() -left_data->rough_quad() )) != guess)
		{
			left_data.refine(guess);
			right_data.refine(guess);
		}
	}
	
	size_t num_points()
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			return left_data->num_points() + right_data->num_points();
		}
		else
		{
			return 3;
		}
	}
	
	double point(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->num-points();
			if(index<left_num)
			{
				return left_data->point(index);
			}
			else
			{
				return right_data->point(index-left_num+1);
			}
		}
		else
		{
			if(index==0) return left_point;
			if(index==1) return middle_point;
			if(index==2) return right_point;
		}
	}
	
	double value(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->num-points();
			if(index<left_num)
			{
				return left_data->value(index);
			}
			else
			{
				return right_data->value(index-left_num+1);
			}
		}
		else
		{
			if(index==0) return left_value;
			if(index==1) return middle_value;
			if(index==2) return right_value;
		}
	}
	
	double cum_quad(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->num-points();
			if(index<left_num)
			{
				return left_data->cum_quad(index);
			}
			else
			{
				return right_data->cum_quad(index-left_num+1)+left_data->quad();
			}
		}
		else
		{
			if(index==0) return 0;
			if(index==1) return rough_quad(middle_point);
			if(index==2) return rough_quad(right_point);
		}
	}
};

class cum_adap_simps
{
private:
	std::vector<cum_adap_simps_data> sections;
	
public:
	
	cum_adap_simps(functor_1D* function, double lower_bound, double upper_bound, unsigned int init_samples=5)
	{
		gsl::vector test_points=linspace(lower_bound, upper_bound, init_samples);
		gsl::vector test_values=function->call(test_points);
		
		double guess=(test_values.sum()-0.5*(test_values[0]-test_values[init_samples-2]))*(test_points[1]-test_points[0]); //trapezoidal guess
		
		sections.reserve(init_samples-1);
		for(size_t i=0; i<init_samples-1; i++)
		{
			sections.emplace_back(function, test_points[i], test_points[i+1], test_values[i], test_values[i+1]);
			sections.back().refine(guess);
		}
	}
	
	double quad()
	{
		gsl::vector quads(sections.size());
		for(size_t i=0; i<quads.size(); i++)
		{
			quads[i]=sections[i].quad();
		}
		return quads.sum();//partially using kahan summation.....hopefully
	}
	
	gsl::vector
}
