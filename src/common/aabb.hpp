#ifndef _AABB_HPP_
#define _AABB_HPP_

#include <glm.hpp>

class AABB {

public:

    // Constructors

    AABB() = default;
    
    AABB(const glm::vec3& bmin, const glm::vec3& bmax);

    AABB(const AABB& other);

    AABB(AABB&& other);

    AABB& operator=(const AABB& other);

    AABB& operator=(AABB&& other);

    // Queries

    bool operator==(const AABB& other);

    glm::vec3 center() const;

    bool containsExclusive(const glm::vec3& point) const;

    bool containsInclusive(const glm::vec3& point) const;

    bool isCollapsed() const;

    bool isEmpty() const;

    bool isValid() const;

    bool overlapsExclusive(const AABB& other) const;

    bool overlapsInclusive(const AABB& other) const;

private:

    glm::vec3 bmin, bmax;
};

#endif