#include "aabb.hpp"
#include "glm_extended.hpp"

AABB::AABB(const glm::vec3& bmin, const glm::vec3& bmax) 
    : bmin(bmin)
    , bmax(bmax)
{}

AABB::AABB(const AABB& other) 
    : bmin(other.bmin)
    , bmax(other.bmax)
{}

AABB::AABB(AABB&& other) 
    : bmin(other.bmin)
    , bmax(other.bmax)
{}

AABB& AABB::operator=(const AABB& other) {
    bmin = other.bmin;
    bmax = other.bmax;
    return *this;
}

AABB& AABB::operator=(AABB&& other) {
    bmin = other.bmin;
    bmax = other.bmax;
    return *this;
}

// Queries

bool AABB::operator==(const AABB& other) {
    return bmin == other.bmin && bmax == other.bmax;
}

glm::vec3 AABB::center() const {
    return (bmax + bmin) / 2.f;
}

bool AABB::containsExclusive(const glm::vec3& point) const {
    return point > bmin && point < bmax;
}

bool AABB::containsInclusive(const glm::vec3& point) const {
    return point >= bmin && point <= bmax; 
}

bool AABB::isCollapsed() const {
    return bmin == bmax;
}

bool AABB::isEmpty() const {
    return bmin == bmax && bmin == glm::vec3(0.f);
}

bool AABB::isValid() const {
    return bmin < bmax;
}

bool AABB::overlapsExclusive(const AABB& other) const {
    return bmin < other.bmax && bmax > other.bmin;
}

bool AABB::overlapsInclusive(const AABB& other) const {
    return bmin <= other.bmax && bmax >= other.bmin;
}