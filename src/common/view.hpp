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

    float* get_pointer() {
        return glm::value_ptr(view_transform);
    }

protected:

    glm::mat4 view_transform;
    glm::vec3 dir, pos;
    float yaw, pitch;
};

#endif