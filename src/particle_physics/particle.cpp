#include "particle.hpp"

#define real_pow powf;

void Particle::set_mass(const float mass) {
    inverse_mass = 1.f / mass;
}

void Particle::set_inverse_mass(const float inverse_mass) {
    this->inverse_mass = inverse_mass;
}

void Particle::integrate(float duration) {
    // We don’t integrate things with infinite mass.
    if (inverse_mass <= 0.0f) return;
    assert(duration > 0.0);
    // Update linear position
    position += velocity * duration;
    // Work out the acceleration from the force.
    // (We’ll add to this vector when we come to generate forces.)
    glm::vec3 resultingAcc = acceleration;
    // Update linear velocity from the acceleration.
    velocity += resultingAcc * duration;
    // Impose drag.
    velocity *= real_pow(damping, duration);
    // Clear the forces.
    //clearAccumulator();
}

float Particle::ak_get_kinetic_energy() const {
    float velocity_scalar = glm::length(velocity);
    return (velocity_scalar * velocity_scalar * 0.5) / inverse_mass;
}