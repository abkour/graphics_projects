#ifndef _POINT_MASS_HPP_
#define _POINT_MASS_HPP_

#include <algorithm>
#include <iostream>
#include <vector>

#include "simulation_constants.hpp"

struct MouseState {
    float mouse_influence_scalar = 5;
    float mousex, mousey;
    float pmousex, pmousey;
    bool lmb_pressed = false;

    float dist_point_to_segment_squared(float x, float y) const {
        float vx = pmousex - x;
        float vy = pmousey - y;
        float ux = mousex - pmousex;
        float uy = mousey - pmousey;

        float len = ux*ux + uy*uy;
        float det = (-vx * ux) + (-vy * uy);

        if(det < 0 || det > len) {
            ux = mousex - x;
            uy = mousey - y;
            return std::min(vx*vx + vy*vy, ux*ux + uy*uy);
        }

        det = ux*vy - uy*vx;
        return (det * det) / len;
    }
    
    float distance_traveled_x() const {
        return (mousex - pmousex) * mouse_influence_scalar;
    }

    float distance_traveled_y() const {
        return (mousey - pmousey) * mouse_influence_scalar;
    }
};

struct PointMass {

    PointMass() = default;

    PointMass(float xpos, float ypos) {
        x = xpos;
        y = ypos;
        lastx = x;
        lasty = y;
        accx = accy = 0.f;
    }

    void update_physics(float timestep, float gravity = 980) {
        apply_force(0, MASS * gravity);
        float velx = x - lastx;
        float vely = y - lasty;

        // damper velocity
        velx *= 0.99f;
        vely *= 0.99f;

        float timestep_sq = timestep * timestep;

        float nextx = x + velx + 0.5f * accx * timestep_sq; 
        float nexty = y + vely + 0.5f * accy * timestep_sq; 
    
        lastx = x;
        lasty = y;

        x = nextx;
        y = nexty;

        accx = 0.f;
        accy = 0.f;
    }

    void update_interactions(const MouseState& mouse_state) {
        if(mouse_state.lmb_pressed) {
            float distance_squared = mouse_state.dist_point_to_segment_squared(x, y);
            if(distance_squared < MOUSE_INFLUENCE_SIZE) {
                lastx = x - mouse_state.distance_traveled_x();
                lasty = y - mouse_state.distance_traveled_y();
            }
        }
    }

    void apply_force(float fX, float fY) {
        accx += fX/MASS;
        accy += fY/MASS;
    }

    void pin_to(float pinpointx, float pinpointy) {
        pinned = true;
        pinx = pinpointx;
        piny = pinpointy;
    }

    float lastx, lasty;
    float x, y;
    float accx, accy;

    bool pinned = false;
    float pinx, piny;
};

#endif