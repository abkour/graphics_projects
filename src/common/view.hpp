#ifndef _VIEW_HPP_
#define _VIEW_HPP_

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "movement_direction.hpp"

class ViewTransform {

public:

    ViewTransform(const glm::vec3& origin, const glm::vec3& target);

    // Rotate the orientation vector
    void rotate(const glm::vec2 screen_offy);

    // Translate the origin vector to a new location.
    void translate(MovementDirection movement_direction, float delta_time);

    glm::vec3 get_position() const { return pos; }
    glm::vec3 get_direction() const { return dir; }

    float* get_pointer() {
        return glm::value_ptr(view_transform);
    }

    glm::mat4 get_view_transform() const {
        return view_transform;
    }

    void set_movement_scale(float scale);

protected:

	float movement_speed = 5.f;

    glm::mat4 view_transform;
    glm::vec3 dir, pos;
    float yaw, pitch;
};

#endif