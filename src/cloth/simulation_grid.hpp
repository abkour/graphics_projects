#ifndef _SIMULATION_GRID_HPP_
#define _SIMULATION_GRID_HPP_

#include "link.hpp"
#include "point_mass.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

class SimulationGrid {

public:

    SimulationGrid(const uint32_t gridx, const uint32_t gridy);

    // Attach a link *from* point mass p0 *to* point mass p1.
    // Directionally matters, since we only store one link per point mass pair.
    void attach_to(const uint32_t from_idx, const uint32_t to_idx);

    void pin_point_mass(const uint32_t idx);

    // Call this function once you have finished attaching links to point masses.
    void finish_attachment();

    glm::ivec2 get_dimensions() const {
        return grid_dimensions;
    }

    std::size_t get_link_count() const {
        return links.size();
    }

    std::size_t get_point_mass_count() const {
        return point_masses.size();
    }

    PointMass& get_point_mass(std::size_t idx) {
        return point_masses[idx];
    }

    Link& get_link(std::size_t idx) {
        return links[idx];
    }

    // Implementation details
    void solve_constraints(const uint32_t grid_idx);
    void solve_link_constraint(const Link& link);

protected:

    glm::ivec2 grid_dimensions;
    std::vector<PointMass> point_masses;

    std::vector<Link> links;
    // For a regular grid, each point mass has exactly two links attached to it.
    // However, since we support general fabrics, we are not limited to regular grids and 
    // point masses can have any number of links attached to them.
    std::vector<uint8_t> link_counts;
    std::vector<uint32_t> link_offsets;
};

#endif