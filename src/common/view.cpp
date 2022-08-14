#include "view.hpp"
#include <gtc/matrix_transform.hpp>

ViewTransform::ViewTransform(const glm::vec3& origin, const glm::vec3& target) 
    : pos(origin)
    , dir(target)
    , pitch(0.f)
    , yaw(-89.f)
{
    view_transform = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));
}

void ViewTransform::rotate(glm::vec2 screen_off) {
    yaw += screen_off.x * 0.14f;
    pitch += screen_off.y * 0.14f;

    if(pitch > 89.f) {
        pitch = 89.f;
    } else if(pitch < -89.f) {
        pitch = -89.f;
    }

    dir.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    dir.y = std::sin(glm::radians(pitch));
    dir.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    dir = normalize(dir);

    view_transform = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));
}

void ViewTransform::translate(MovementDirection movement_direction, float delta_time) {
	static const float movementSpeed = 5.f;
    switch (movement_direction) {
	case MovementDirection::Forward:
		pos += dir * delta_time * movementSpeed;
		break;
	case MovementDirection::Backward:
		pos -= dir * delta_time * movementSpeed;
		break;
	case MovementDirection::Left:
		pos -= glm::cross(dir, glm::vec3(0.f, 1.f, 0.f)) * delta_time * movementSpeed;
		break;
	case MovementDirection::Right:
		pos += glm::cross(dir, glm::vec3(0.f, 1.f, 0.f)) * delta_time * movementSpeed;
		break;
	default:
		break;
	}

    view_transform = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));
}
