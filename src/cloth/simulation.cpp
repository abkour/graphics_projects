#include "simulation.hpp"

// Helper functions
std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << v.x << ", " << v.y;
}

namespace helpers {

// Convert grid space to clip space
static glm::vec2 grid_to_clip_conversion(const glm::vec2 grid_resolution, const glm::vec2 sample)
{
    glm::vec2 normalized_result = sample / grid_resolution;
    normalized_result = (normalized_result * glm::vec2(2.f, -2.f)) - glm::vec2(1.f, -1.f);
    return normalized_result;
}

} // namespace helpers

//
//
// ClothSimulation implementation
ClothSimulation::ClothSimulation(float width, float height) 
    : simulation_grid(width, height)
    , accumulated_delta_pos(0.f, 0.f)
{}

void ClothSimulation::setup() {

    construct_fabric();

    ShaderWrapper shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/cloth/shaders/grid.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/cloth/shaders/grid.glsl.fs"))
    );
    grid_shader = std::move(shader);

    const auto link_count = simulation_grid.get_link_count();
    render_storage_buffer.resize(link_count * 2);

    glGenVertexArrays(1, &grid_vao);
    glGenBuffers(1, &grid_vbo);
    glBindVertexArray(grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * render_storage_buffer.size(), NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

ClothSimulation::~ClothSimulation() {
    glDeleteVertexArrays(1, &grid_vao);
    glDeleteBuffers(1, &grid_vbo);
}

void ClothSimulation::tick(const WindowManager& window_manager, float elapsed_time) {
    constexpr int constraint_accuracy = 3;
    constexpr float fixed_delta_time = 1000.f / static_cast<float>(SIMULATION_FRAMERATE);
    constexpr float fixed_delta_time_seconds = fixed_delta_time / 1000.f;

    accumulated_delta_pos += window_manager.get_mouse_delta();

    elapsed_time *= 1000.f;
    int timesteps = static_cast<int>((elapsed_time + left_over_time) / fixed_delta_time);
    timesteps = std::min(timesteps, 2);
    
    accumulated_delta_pos += window_manager.get_mouse_delta();
    float mouse_influence_scalar = 1.f / timesteps;
    update_mouse_state(window_manager, mouse_influence_scalar);

    // Reset spill over time, once a simulation has to occur.
    if(timesteps > 0) {
        left_over_time = (elapsed_time + left_over_time) - timesteps * fixed_delta_time;
        accumulated_delta_pos = glm::vec2(0.f, 0.f);
    } else {
        left_over_time += elapsed_time - timesteps * fixed_delta_time;
    }

    for(int i = 1; i <= timesteps; ++i) {
        for(int x = 0; x < constraint_accuracy; ++x) {
            for(int p = 0; p < simulation_grid.get_point_mass_count(); ++p) {
                simulation_grid.solve_constraints(p);
            }
        }

        for(int p = 0; p < simulation_grid.get_point_mass_count(); ++p) {
            auto& pm = simulation_grid.get_point_mass(p);
            pm.update_interactions(mouse_state);
            pm.update_physics(fixed_delta_time_seconds);
        }
    }
}

void ClothSimulation::render() {
    // batch vertices
    const glm::ivec2 grid_dimensions = simulation_grid.get_dimensions();
    const std::size_t link_count = simulation_grid.get_link_count();
    for(int i = 0; i < link_count; ++i) {
        auto& link = simulation_grid.get_link(i);
        auto& p1 = simulation_grid.get_point_mass(link.grid_attachment_point_1);
        auto& p2 = simulation_grid.get_point_mass(link.grid_attachment_point_2);
        render_storage_buffer[i * 2]        = helpers::grid_to_clip_conversion(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec2(p1.x, p1.y));
        render_storage_buffer[i * 2 + 1]    = helpers::grid_to_clip_conversion(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec2(p2.x, p2.y));
    }
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * render_storage_buffer.size(), render_storage_buffer.data());

    grid_shader.bind();
    glBindVertexArray(grid_vao);
    glDrawArrays(GL_LINES, 0, render_storage_buffer.size());
}

//
//
// Implementaiton details
void ClothSimulation::construct_fabric() {
    const auto& grid_dimensions = simulation_grid.get_dimensions();
    //int midWidth = (int) (width/2 - (GRID_WIDTH * resting_distance)/2);
    // Top left (0, 0) -> Bottom right (x, y)
    for(int y = 0; y < grid_dimensions.y; ++y) {
        for(int x = 0; x < grid_dimensions.x; ++x) {
            int xpos = x + y * grid_dimensions.x;
            if(x != grid_dimensions.x - 1) {
                simulation_grid.attach_to(xpos, xpos + 1);
            }
            if(y != grid_dimensions.y - 1) {
                simulation_grid.attach_to(xpos, xpos + grid_dimensions.x);
            }
            if(y == 0) {
                // Pin the top row to prevent it from falling.
                simulation_grid.pin_point_mass(xpos);
            }
        }
    }

    simulation_grid.finish_attachment();
}

void ClothSimulation::update_mouse_state(const WindowManager& wm, const float mouse_influence_scalar) {
    auto cursor_pos = wm.get_mouse_position();
    auto cursor_delta = accumulated_delta_pos;
    mouse_state.lmb_pressed = wm.get_lmb_pressed();
    cursor_delta.y = -cursor_delta.y;
    mouse_state.mousex = cursor_pos.x;
    mouse_state.mousey = cursor_pos.y;
    mouse_state.pmousex = cursor_pos.x - cursor_delta.x;
    mouse_state.pmousey = cursor_pos.y - cursor_delta.y;
    mouse_state.mouse_influence_scalar = mouse_influence_scalar;
}
