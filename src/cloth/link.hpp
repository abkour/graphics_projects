#ifndef _LINK_HPP_
#define _LINK_HPP_

#include <glm.hpp>

// A link represents a bidirectional physical connection between two point masses.
// When a point mass is affected by a force, the link will sheer and stretch and 
// apply a force on the other point mass.
struct Link {

public:

    Link(const uint32_t grid_idx0, const uint32_t grid_idx1)
    {
        grid_attachment_point_1 = grid_idx0; // when you set one object to another, it's pretty much a reference. 
        grid_attachment_point_2 = grid_idx1; // Anything that'll happen to p1 or p2 in here will happen to the paticles in our ArrayList
    }

    uint32_t grid_attachment_point_1;
    uint32_t grid_attachment_point_2;
};

#endif