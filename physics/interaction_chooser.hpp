#ifndef INTERACTION_CHOOSER
#define INTERACTION_CHOOSER

#include <array>
#include <cmath>

#include "rand.hpp"
#include "gen_ex.hpp"
#include "root_finding.hpp"

//these are classes to decide when an interaction happens and which interaction happens

//not convinced that interaction_chooser_constant and interaction_chooser_linear are useful and error free

class physical_interaction
{
    public:

    virtual double rate(double energy)=0; //return negative if no interaction
};


template< int num_interaction_types>
class interaction_chooser_constant
//assumes that the rate of interactions doesn't change significant over one iteration
//this approxametly what dwyer does, just with fancier code
// I don't think I will do this.
{
    std::array<physical_interaction*, num_interaction_types> interactions;
    std::array<double, num_interaction_types> interaction_rates;
	rand_threadsafe rand;


    //// template functions to initilize interactions
    template< int num, typename first_T, typename ... args_T>
    inline void init(first_T& interaction, args_T& ... interactions_ )
    {
        interactions[num]=&interaction;
        init<num+1, args_T...>(interactions_...);
    }
    template< int num, typename first_T>
    inline void init(first_T& interaction)
    {
        interactions[num]=&interaction;
    }


    //// the API  ////

    public:

    template< typename ... args_T>
    interaction_chooser_constant(args_T& ... interactions_)
    {
        if( sizeof...(interactions_) !=  num_interaction_types)
        {
            throw gen_exception("number of interactions in interaction_chooser_constant is not equal to template value");
        }
        init<0, args_T...>(interactions_...);
    }

    double sample(double energy, int& interaction_chosen)
    //// sample the interactions for at an energy. Return the time untill the interaction and place
    // the index corresponding to the interaction into interaction_chosen
    {
        //find interaction rates
        double total_rate=0;
        for(int i=0; i<num_interaction_types; i++)
        {
            interaction_rates[i]=interactions[i]->rate(energy);
            total_rate+=interaction_rates[i];
        }

        if(float(1+total_rate)==1) //total rate is about zero
        {
            interaction_chosen=-1;
            return 0; //need to make sure interaction_chosen is not -1 on return
        }

        //choose interaction
        double interaction_sample=rand.uniform()*total_rate;
        for(int i=0; i<num_interaction_types; i++)
        {
            interaction_sample-=interaction_rates[i];
            if(interaction_sample<0)
            {
                interaction_chosen=i;
                break;
            }
        }

        //sampel time and return
        return rand.exponential(1.0/total_rate);
    }
};













template< int num_interaction_types>
class interaction_chooser_linear
//find when and which interaction occurs, assuming that interaction rate changes linearly durring the timestep
//has warnings if the interaction rate changes too quickly
{
private:
    std::array<physical_interaction*, num_interaction_types> interactions;
    std::array<double, num_interaction_types> num_interactions_in_timestep;
	rand_threadsafe rand;

	//double timestep_size;
	//double initial_energy;
	//double final_energy;

    int error_flag; //0 if everything is okay, 1 if next timestep should be smaller, 2 if thies timestep needs to be re-done
    double lower_error_bound; //default to 0.1
    double upper_error_bound; //default to 0.2




    //// template functions to initilize interactions
    template< int num, typename first_T, typename ... args_T>
    inline void init(first_T& interaction, args_T& ... interactions_ )
    {
        interactions[num]=&interaction;
        init<num+1, args_T...>(interactions_...);
    }
    template< int num, typename first_T>
    inline void init(first_T& interaction)
    {
        interactions[num]=&interaction;
    }



    public:

    template< typename ... args_T>
    interaction_chooser_linear(args_T& ... interactions_)
    {
        if( sizeof...(interactions_) !=  num_interaction_types)
        {
            throw gen_exception("number of interactions in interaction_chooser_linear is not equal to template value");
        }

        init<0, args_T...>(interactions_...);

        lower_error_bound=0.1;
        upper_error_bound=0.2;
    }

    void set_error_bound(double lower_error_bound_, double upper_error_bound_)
    {
        lower_error_bound=lower_error_bound_;
        upper_error_bound=upper_error_bound_;
    }

    double sample(double initial_energy, double final_energy, double timestep_size, int& interaction_chosen)
    //// sample the interactions for at an energy. Return the time untill the interaction and place
    // the index corresponding to the interaction into interaction_chosen. if the return value is greater than the timestep size
    // then the interaction doesn't happen
    {
        //set variables
        error_flag=0;

        //sample interactions
        double total_A=0;
        double total_B=0;
        for(int i=0; i<num_interaction_types; i++)
        {
            //find A and B
            double Ai=interactions[i]->rate(initial_energy);
            double Bi=interactions[i]->rate(final_energy);

            if(float(Bi+Ai)==float(Ai))//if the rate dropped to zero
            {
                Ai=0; //then lets say rate was zero everywhere in the setp
            }
            else
            {
                Bi-=Ai;
            }

            //check error
            double percent_change=std::abs(Bi/Ai);
            if(percent_change>lower_error_bound)
            {
                if(percent_change>upper_error_bound)
                {
                    error_flag=2;
                }
                else if(error_flag==0)
                {
                    error_flag=1;
                }
            }

            // set the physics
            total_A+=Ai;
            total_B+=Bi;
            num_interactions_in_timestep[i]=Ai+0.5*Bi*timestep_size;
        }
        double total_num_interactions_in_timestep=total_A + 0.5*total_B*timestep_size;

        if( float(total_num_interactions_in_timestep+1)==float(1)) //approxamently zero interactions
        {
            interaction_chosen=-1;
            return 2.0*timestep_size; //if return value is greater than timestep_size, then interaction doesn't happen
        }

        //choose which interaction happens
        double interaction_sample=rand.uniform()*total_num_interactions_in_timestep;
        for(int i=0; i<num_interaction_types; i++)
        {
            interaction_sample-=num_interactions_in_timestep[i];
            if(interaction_sample<0)
            {
                interaction_chosen=i;
                break;
            }
        }

        //find time untill interaction
        double U=rand.uniform();
        if( float(total_A + total_B) == float(total_A) ) //total_B is about zero
        {
            return -std::log(1-U)/total_A;  //that's it!
        }
        else if(total_B>0)
        {
            double ratio=total_A*timestep_size/total_B;
            return std::sqrt(ratio*ratio - 2*std::log(1-U)*timestep_size/total_B ) - ratio;
        }
        else //B<0
        {
            double ratio=total_A*timestep_size/total_B;
            return -std::sqrt(ratio*ratio - 2*std::log(1-U)*timestep_size/total_B ) - ratio;
        }

    }

    int get_error_flag()
    {
        return error_flag;
    }
};





















template< int num_interaction_types>
class interaction_chooser_quadratic
//find when and which interaction occurs, assuming that interaction rate changes quadraticly during the timestep
//has warnings if the interaction rate changes too quickly
{
private:
    std::array<physical_interaction*, num_interaction_types> interactions;
	rand_threadsafe rand;

	//double timestep_size;
	//double initial_energy;
	//double final_energy;

    int error_flag; //0 if everything is okay, 1 if next timestep should be smaller, 2 if thies timestep needs to be re-done
    double lower_error_bound; //default to 0.05
    double upper_error_bound; //default to 0.1




    //// template functions to initilize interactions
    template< int num, typename first_T, typename ... args_T>
    inline void init(first_T& interaction, args_T& ... interactions_ )
    {
        interactions[num]=&interaction;
        init<num+1, args_T...>(interactions_...);
    }
    template< int num, typename first_T>
    inline void init(first_T& interaction)
    {
        interactions[num]=&interaction;
    }


    ////class to find interaction time
    class interaction_time_finder : public functor_1D
    {
        public:
        double constant;
        double A;
        double B;
        double C;

        interaction_time_finder(double constant_, double A_, double B_, double C_)
        {
            constant=constant_;
            A=A_;
            B=B_;
            C=C_;
        }

        double call(double t_bar)
        {
            return ((C*t_bar + B)*t_bar + A)*t_bar + constant;
        }

    };



    public:

    template< typename ... args_T>
    interaction_chooser_quadratic(args_T& ... interactions_)
    {
        if( sizeof...(interactions_) !=  num_interaction_types)
        {
            throw gen_exception("number of interactions in interaction_chooser_linear is not equal to template value");
        }

        init<0, args_T...>(interactions_...);

        lower_error_bound=0.00025; //seems to be the right amount
        upper_error_bound=0.0005;
        error_flag=0;
    }

    void set_error_bound(double lower_error_bound_, double upper_error_bound_)
    {
        lower_error_bound=lower_error_bound_;
        upper_error_bound=upper_error_bound_;
    }

    double sample(double initial_energy, double middle_energy, double final_energy, double timestep_size, int& interaction_chosen)
    //// sample the interactions for at an energy. Return the time untill the interaction.
    // the index corresponding to the interaction into interaction_chosen.
    // if the return value is greater than the timestep size then the interaction doesn't happen
    {
        //set variables
        error_flag=0;
        std::array<double, num_interaction_types> num_interactions_in_timestep;


        //sample interactions
        double total_A=0;
        double total_B=0;
        double total_C=0;
        for(int i=0; i<num_interaction_types; i++)
        {
            //find A and B
            double initial_rate=interactions[i]->rate(initial_energy);
            double middle_rate =interactions[i]->rate(middle_energy);
            double final_rate  =interactions[i]->rate(final_energy);

            double Ai=initial_rate;
            double Bi=4.0*middle_rate - final_rate - 3.0*Ai;
            double Ci=final_rate - Ai - Bi;

            //find error
            double percent_error=std::abs( (middle_rate - 0.5*(initial_rate+final_rate)) / middle_rate); //check error of linear assumption using middle sample

            //check for no interaction
            if(initial_rate<0 or middle_rate<0 or final_rate<0) //no interaction
            {
                Ai=0;
                Bi=0;
                Ci=0;
                percent_error=0.0;
            }

            //check error
            if(percent_error>lower_error_bound)
            {
                if(percent_error>upper_error_bound)
                {
                    error_flag=2;
                //print("ERROR 2 sub", timestep_size, Ai, Bi, Ci, percent_error);
                //print("  ", percent_error, initial_rate, middle_rate, final_rate);
                }
                else if(error_flag==0)
                {
                    error_flag=1;
                //print("ERROR 1 sub", timestep_size, Ai, Bi, Ci, percent_error);
                }
            }

            // set the physics
            total_A+=Ai;
            total_B+=Bi;
            total_C+=Ci;
            num_interactions_in_timestep[i]=(Ai + 0.5*Bi + Ci/3.0)*timestep_size;
        }

/*
        //check total error
        double percent_change=std::abs(total_C/(total_A+total_B+total_C));
        if(percent_change>lower_error_bound)
        {
            if(percent_change>upper_error_bound)
            {
                error_flag=2;
                print("ERROR 2", timestep_size, total_A, total_B, total_C);
            }
            else if(error_flag==0)
            {
                error_flag=1;
                print("ERROR 1", timestep_size, total_A, total_B, total_C);
            }
        }
*/


        //find number of interactions in timestep
        double total_expected_num_interactions_in_timestep=(total_A + 0.5*total_B + total_C/3.0)*timestep_size;
        int total_actuall_interactions=rand.poisson(total_expected_num_interactions_in_timestep);

        if(total_actuall_interactions==0)
        {
            //no interactions
            interaction_chosen=-1;
            //print("NO");
            return 2.0*timestep_size;
        }


        //select type of interaction
        double interaction_sample=rand.uniform()*total_expected_num_interactions_in_timestep;
        interaction_chosen=-1;
        for(int i=0; i<num_interaction_types; i++)
        {
            interaction_sample-=num_interactions_in_timestep[i];
            if(interaction_sample<0)
            {
                interaction_chosen=i;
                //print("YES", i);
                break;
            }
            else
            {
                //print("ARG");
            }
        }

        if(interaction_chosen==-1)//then there was an error....  NOt sure how to fix this correctly
        {
            interaction_chosen=num_interaction_types-1; //so solve me maybe?
            //print("got no", interaction_chosen);
        }
        //else
        //{
            //print("got", interaction_chosen);
        //}


        //select time of interaction
        double min_interaction_time_rand=2.0;
        for(int i=0; i<total_actuall_interactions; i++)
        {
            double U=rand.uniform();
            if(U<min_interaction_time_rand)
            {
                min_interaction_time_rand=U;
            }
        }

        double denom=total_A + 0.5*total_B + total_C/3.0;
        interaction_time_finder finder(-min_interaction_time_rand, total_A/denom, 0.5*total_B/denom, total_C/(3.0*denom));

        double interaction_Tbar=root_finder_brent(&finder, 1.0, 0.0, 0.0001, 0.0001, 1000);
        return interaction_Tbar*timestep_size;
    }

    int get_error_flag()
    {
        return error_flag;
    }
};


#endif
