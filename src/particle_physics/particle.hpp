#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include <glm.hpp>

class Particle {

public:

    Particle() = default;

    void set_mass(const float mass);
    void set_inverse_mass(const float inverse_mass);

    /**
    * Integrates the particle forward in time by the given amount.
    * This function uses a Newton-Euler integration method, which is a
    * linear approximation to the correct integral. For this reason it
    * may be inaccurate in some cases.
    */
    void integrate(float duration);

    float ak_get_kinetic_energy() const;

protected:

    glm::vec3 position, velocity, acceleration;
    float damping;

    /**
    * Holds the inverse of the mass of the particle. It
    * is more useful to hold the inverse mass because
    * integration is simpler, and because in real-time
    * simulation it is more useful to have objects with
    * infinite mass (immovable) than zero mass
    * (completely unstable in numerical simulation).
    */
    float inverse_mass;
};

#endif