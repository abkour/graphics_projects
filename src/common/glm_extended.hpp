#ifndef _GLM_EXTENDED_HPP_
#define _GLM_EXTENDED_HPP_

#include <glm.hpp>

bool operator==(const glm::vec3& v0, const glm::vec3& v1) {
    return v0.x == v1.x && v0.y == v1.y && v0.z == v1.z;
}

bool operator!=(const glm::vec3& v0, const glm::vec3& v1) {
    return !(v0 == v1);
}

bool operator<(const glm::vec3& v0, const glm::vec3& v1) {
    return v0.x < v1.x && v0.y < v1.y && v0.z < v1.z;
}

bool operator<=(const glm::vec3& v0, const glm::vec3& v1) {
    return v0.x <= v1.x && v0.y <= v1.y && v0.z <= v1.z; 
}

bool operator>(const glm::vec3& v0, const glm::vec3& v1) {
    return !(v0 < v1);
}

bool operator>=(const glm::vec3& v0, const glm::vec3& v1) {
    return !(v0 <= v1);
}

// Make orthonormal base. Returns false, if a and b are parallel
bool make_orthonormal_base(glm::vec3& a, glm::vec3& b, glm::vec3& c) {
    a = glm::normalize(a);
    c = glm::normalize(glm::cross(a, b));
    if(c.length() == 0) {
        return false;
    }
    b = glm::cross(c, a);
    return true;
}

#endif