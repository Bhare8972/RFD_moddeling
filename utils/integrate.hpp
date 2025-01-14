﻿
#ifndef INTEGRATE_HPP
#define INTEGRATE_HPP

#include <vector>
#include <memory>
#include <list>

#include "GSL_utils.hpp"
#include "vector.hpp"
#include "vector_int.hpp"
#include "functor.hpp"
#include "gen_ex.hpp"
#include "spline.hpp"

//this is out of date
//consider using chebyshev instead

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
	double left_integrand;

	std::shared_ptr<cum_adap_simps_data> left_data;
	std::shared_ptr<cum_adap_simps_data> right_data;

public:

    int stage;//0 means good, but does not have subsections, 1 has subsections. 2 is in error, 3 tried subsections, but lower subsections had error, 4 one of the recursivly lower subsections is a 3

	cum_adap_simps_data(functor_1D* functor, double left_point_, double right_point_, double left_value_, double right_value_)
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

		left_integrand=0;
		left_integrand=rough_quad(left_point);

        stage=0;
	}

	inline double rough_quad()
	{
        double A=right_point*right_point;
        double B=A*right_point;
		return weight_one*right_point + weight_two*A*0.5 + weight_three*B/3.0 - left_integrand;
	}

	double rough_quad(double point)
	{
		if(point<left_point) return 0.0;
		else if(point>right_point) return rough_quad();

		return weight_one*point + weight_two*point*point/2.0 + weight_three*point*point*point/3.0 - left_integrand;
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

	void refine(double factor)
	{
		// step one, make the next lower two simps rules
		left_data=std::make_shared<cum_adap_simps_data>(function, left_point, middle_point, left_value, middle_value);
		right_data=std::make_shared<cum_adap_simps_data>(function, middle_point, right_point, middle_value, right_value);

	    if(left_data->stage==2 or right_data->stage==2)
	    {
	        left_data.reset();
	        right_data.reset();
	        stage=3;

	        return;
	    }
        stage=1;
        //print("ENTER");
        //print(rough_quad(), left_data->rough_quad(), right_data->rough_quad(), (left_data->rough_quad()+right_data->rough_quad()));
        //print(float((rough_quad()-left_data->rough_quad()) + rough_quad()) == float(rough_quad()),  float((rough_quad()-right_data->rough_quad()) + rough_quad()) == float(rough_quad()) );

		//step two, if necisary, refine the lower two sub sections
		//if( std::abs( rough_quad() - (left_data->rough_quad()+right_data->rough_quad())) > 15*rel_tol )
		double R=rough_quad();
		if( float(R*factor + float( R - (left_data->rough_quad()+right_data->rough_quad()))) != float(R*factor) )
		{
		    //print("RECURE!");
			left_data->refine(factor);
			right_data->refine(factor);

            if(left_data->stage==3 or left_data->stage==4 or right_data->stage==3 or right_data->stage==4)
            {
                stage=4;
            }
		}
		//else
		//{
		    //print("NO RECURE:", std::abs( rough_quad() - (left_data->rough_quad()+right_data->rough_quad())), 15*rel_tol );
		//}
		//print("EXIT", stage);
        //print();
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

	double point(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->size();
			if(index<left_num)
			{
				return left_data->point(index);
			}
			else
			{
				return right_data->point(index-left_num+1);
			}
		}

        if(index==0) return left_point;
        if(index==1) return middle_point;
        if(index==2) return right_point;
        return 0; //keep from warning;
	}

	double value(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->size();
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
		return 0; //prevent warning
	}

	double cum_quad(size_t index)
	{
		if(left_data) //if this exists, then we have made the two sections
		{
			size_t left_num=left_data->size();
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
		return 0; //prevent warning
	}
};

class cum_adap_simps
{
private:
	std::vector<cum_adap_simps_data> sections;

public:

	cum_adap_simps(functor_1D* function, double lower_bound, double upper_bound, double tol_factor=1, unsigned int init_samples=5)
	{
		gsl::vector test_points=linspace(lower_bound, upper_bound, init_samples);
		gsl::vector test_values=function->callv(test_points);

		sections.reserve(init_samples-1);
		for(size_t i=0; i<init_samples-1; i++)
		{
			sections.emplace_back(function, test_points[i], test_points[i+1], test_values[i], test_values[i+1]);
			sections.back().refine(tol_factor);
		}
	}

	double quad()
	{
	/*
		gsl::vector quads(sections.size());
		for(size_t i=0; i<quads.size(); i++)
		{
			quads[i]=sections[i].quad();
		}
		return quads.sum();//partially using kahan summation.....hopefully*/

        double ret=0;
        for(size_t section_i=0; section_i<sections.size(); section_i++)
        {
            ret+=sections[section_i].quad();
        }
        return ret;

	}

	gsl::vector_int sizes()
	{
        gsl::vector_int sizes_(sections.size());
        for(size_t i=0; i<sections.size(); i++)
		{
			sizes_[i]=sections[i].size();
		}
		return sizes_;

	}

    size_t size()
	{
        size_t size_=1;
        for(size_t i=0; i<sections.size(); i++)
		{
			size_+=sections[i].size()-1;
		}
		return size_;

	}

	gsl::vector points()
	{
	    gsl::vector_int sizes_=sizes();
	    size_t current_index=0;
	    gsl::vector points_(size());
	    size_t start=0;

	    for(size_t section_i=0; section_i<sections.size(); section_i++)
	    {
            for(size_t section_point_j=start; section_point_j<size_t(sizes_[section_i]); section_point_j++)
            {
                points_[current_index]=sections[section_i].point(section_point_j);
                current_index++;
            }
            start=1;
	    }

        return points_;
	}

	gsl::vector values()
	{
	    gsl::vector_int sizes_=sizes();
	    size_t current_index=0;
	    gsl::vector values_(size());
	    size_t start=0;

	    for(size_t section_i=0; section_i<sections.size(); section_i++)
	    {
            for(size_t section_point_j=start; section_point_j<size_t(sizes_[section_i]); section_point_j++)
            {
                values_[current_index]=sections[section_i].value(section_point_j);
                current_index++;
            }
            start=1;
	    }

        return values_;
	}

	gsl::vector cum_quads()
	{
	    gsl::vector_int sizes_=sizes();
	    size_t current_index=0;
	    double last_section=0;
	    gsl::vector out(size());
	    size_t start=0;

	    for(size_t section_i=0; section_i<sections.size(); section_i++)
	    {
            for(size_t section_point_j=start; section_point_j<size_t(sizes_[section_i]); section_point_j++)
            {
                out[current_index]=sections[section_i].cum_quad(section_point_j)+last_section;
                current_index++;
            }
            last_section=out[current_index-1];
            start=1;
	    }

        return out;
	}

	int info()
	//0 means succsesfull, 1 means error tollerance not met
	{
	    for(const auto& sect : sections)
	    {
	        if(sect.stage==3 or sect.stage==4)
	        {
	            return 1;
	        }
	    }
	    return 0;
	}

	//gsl::vector_int infos() //return status at each sample point, to be implemented
};


#endif
