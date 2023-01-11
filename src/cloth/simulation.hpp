#ifndef _SIMULATION_HPP_
#define _SIMULATION_HPP_

#include "../common/window_manager.hpp"

#include "simulation_grid.hpp"

// For rendering the grid
#include <glad/glad.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

class ClothSimulation {

public:

    // Cloth dimensions
    ClothSimulation(float width, float height);
    ~ClothSimulation();

    // Required to setup storage for data structures etc.
    void setup();

    // Updates the simulation. Timestep in milliseconds.
    void tick(const WindowManager& window_manager, float elapsed_time);

    // Renders output of simulation to the screen.
    void render();
    
private:

    SimulationGrid simulation_grid;

private:

    std::vector<glm::vec2> render_storage_buffer;
    GLuint grid_vao, grid_vbo;
    ShaderWrapper grid_shader;

private:

    // Implementation details
    glm::vec2 accumulated_delta_pos;
    float left_over_time = 0.f;

    void construct_fabric();
    void update_mouse_state(const WindowManager& wm, const float mouse_influence_scalar);

    MouseState mouse_state;
};

#endif