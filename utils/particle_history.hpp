
#ifndef PARTICLE_HISTORY
#define PARTICLE_HISTORY

#include<string>
#include<list>
#include "binary_IO.hpp"

class particle_history_out
{
public:
    binary_output out;

    /*
    commands:
        1 : add particles
            int32_t; number of particles
            each particle needs
                int32: ID
                int8: charge
                double: creation time
                3 doubles: position
                3 doubles: momentum

        2: update particles
            int32: number of particles
            each particle needs
                int32 ID
                double timestep
                3 doubles: position
                3 doubles: momentum


        3: remove particles
            int32: number of particles
            each particle needs
                int32 ID
                double timestep
                3 doubles: position
                3 doubles: momentum
    */

    particle_history_out() : out("output")
    {}

    particle_history_out(std::string fname) : out(fname)
    {}

    ////// total template abuse!!!! //////

    template< typename p_t>
    void new_particle(p_t &particle)
    {
        out.out_short(1); //command, new particle
        out.out_int(1); //one particle

        out.out_int(particle.ID);
        out.out_short(particle.charge);
        
        out.out_double(particle.current_time);

        out.out_double(particle.position[0]);
        out.out_double(particle.position[1]);
        out.out_double(particle.position[2]);

        out.out_double(particle.momentum[0]);
        out.out_double(particle.momentum[1]);
        out.out_double(particle.momentum[2]);
    }

    template< typename p_t>
    void new_particles(std::list<p_t> &particles)
    {
        out.out_short(1); //command, new particle
        out.out_int(particles.size()); //number of particles

        for(auto &P :particles)
        {
            out.out_int(P.ID);
            out.out_short(P.charge);
            
            out.out_double(P.current_time);

            out.out_double(P.position[0]);
            out.out_double(P.position[1]);
            out.out_double(P.position[2]);

            out.out_double(P.momentum[0]);
            out.out_double(P.momentum[1]);
            out.out_double(P.momentum[2]);
        }
    }

    template< typename p_t>
    void update_particle(p_t &particle)
    {
        out.out_short(2); //command, new particle
        out.out_int(1); //one particle

        out.out_int(particle.ID);
        out.out_double(particle.timestep);

        out.out_double(particle.position[0]);
        out.out_double(particle.position[1]);
        out.out_double(particle.position[2]);

        out.out_double(particle.momentum[0]);
        out.out_double(particle.momentum[1]);
        out.out_double(particle.momentum[2]);
    }

    template< typename p_t>
    void update_particles(std::list<p_t> &particles)
    {
        out.out_short(2); //command, new particle
        out.out_int(particles.size()); //number of particles

        for(auto &P :particles)
        {
            out.out_int(P.ID);
            out.out_double(P.timestep);

            out.out_double(P.position[0]);
            out.out_double(P.position[1]);
            out.out_double(P.position[2]);

            out.out_double(P.momentum[0]);
            out.out_double(P.momentum[1]);
            out.out_double(P.momentum[2]);
        }
    }

    template< typename p_t>
    void remove_particle(p_t &particle)
    {
        out.out_short(3); //command, new particle
        out.out_int(1); //one particle

        out.out_int(particle.ID);
        out.out_double(particle.timestep);

        out.out_double(particle.position[0]);
        out.out_double(particle.position[1]);
        out.out_double(particle.position[2]);

        out.out_double(particle.momentum[0]);
        out.out_double(particle.momentum[1]);
        out.out_double(particle.momentum[2]);
    }

    template< typename p_t>
    void remove_particles(std::list<p_t> &particles)
    {
        out.out_short(3); //command, new particle
        out.out_int(particles.size()); //number of particles

        for(auto &P :particles)
        {
            out.out_int(P.ID);
            out.out_double(P.timestep);

            out.out_double(P.position[0]);
            out.out_double(P.position[1]);
            out.out_double(P.position[2]);

            out.out_double(P.momentum[0]);
            out.out_double(P.momentum[1]);
            out.out_double(P.momentum[2]);
        }
    }
};


#endif
