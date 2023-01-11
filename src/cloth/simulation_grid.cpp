#include "simulation_grid.hpp"

SimulationGrid::SimulationGrid(const uint32_t gridx, const uint32_t gridy) {
    grid_dimensions = glm::ivec2(gridx, gridy);
    const int mid_width = (int) (SCREEN_WIDTH/2 - (GRID_WIDTH * RESTING_DISTANCE)/2);
    const int y_start = 25;

    const uint32_t grid_size = grid_dimensions.x * grid_dimensions.y;    
    point_masses.reserve(grid_size);

    for(int j = 0; j < grid_dimensions.y; ++j) {
        for(int i = 0; i < grid_dimensions.x; ++i) {
            point_masses.emplace_back(mid_width + i * RESTING_DISTANCE, j * RESTING_DISTANCE + y_start);
        }
    }

    // The number of links has a minimum bound of (Number of point masses - 1)
    links.reserve(grid_size - 1);
    link_counts.resize(grid_size, 0);
    link_offsets.resize(grid_size, 0);
}

void SimulationGrid::attach_to( const uint32_t from_idx, const uint32_t to_idx) 
{
    links.emplace_back(from_idx, to_idx);
    link_counts[from_idx]++;
}

void SimulationGrid::pin_point_mass(const uint32_t idx) {
    point_masses[idx].pinned = true;
    point_masses[idx].pinx = point_masses[idx].x;
    point_masses[idx].piny = point_masses[idx].y;
}

void SimulationGrid::solve_constraints(const uint32_t grid_idx) {
        for (int i = 0; i < link_counts[grid_idx]; i++) {
            auto& link = links[link_offsets[grid_idx] + i];
            solve_link_constraint(link);
        }

        PointMass& p = point_masses[grid_idx];
        // Boundary constraints
        if(p.y < 1.f) {
            p.y = 2.f * 1.f - p.y;
        }
        if(p.y > SCREEN_HEIGHT - 1.f) {
            p.y = 2.f * (SCREEN_HEIGHT - 1.f) - p.y;
        }
        
        if(p.x < 1.f) {
            p.x = 2.f * 1.f - p.x;
        }
        if(p.x > SCREEN_WIDTH - 1.f) {
            p.x = 2.f * (SCREEN_WIDTH - 1.f) - p.x;
        }

        if(p.pinned) {
            p.x = p.pinx;
            p.y = p.piny;
        }
}

void SimulationGrid::solve_link_constraint(const Link& link) {
    auto& p1 = point_masses[link.grid_attachment_point_1];
    auto& p2 = point_masses[link.grid_attachment_point_2];
    
    float diffx = p1.x - p2.x;
    float diffy = p1.y - p2.y;
    float d = sqrt(diffx * diffx + diffy * diffy);
    
    float difference = (RESTING_DISTANCE - d) / d;
    
    // Inverse the mass quantities
    float im1 = 1.f / MASS;
    float scalarP1 = (MASS / (MASS + MASS)) * LINK_STIFFNESS;
    float scalarP2 = LINK_STIFFNESS - scalarP1;
    
    // Push/pull based on mass
    // heavier objects will be pushed/pulled less than attached light objects
    p1.x += diffx * scalarP1 * difference;
    p1.y += diffy * scalarP1 * difference;
    
    p2.x -= diffx * scalarP2 * difference;
    p2.y -= diffy * scalarP2 * difference;
}

void SimulationGrid::finish_attachment() {
    uint32_t total_link_count = 0;
    for(int i = 0; i < link_offsets.size(); ++i) {
        link_offsets[i] = total_link_count;
        total_link_count += link_counts[i];
    }
}