#include "../common/view.hpp"
#include "../common/window_manager.hpp"

#include <iostream>
#include <shaderdirect.hpp>
#include <vector>

int main() {
    try {
        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        const int gridx = 128;
        const int gridz = 128;
        const float grid_dimx = 16.f;
        const float grid_dimz = 16.f;
        const float grid_stepx = grid_dimx / gridx;
        const float grid_stepz = grid_dimz / gridz;

        std::vector<glm::vec3> grid(gridx * gridz * 6);

        int step_x = 0;
        int step_z = -1;
        for(int i = 0; i < grid.size() / 6; i++) {
            if(i % gridx == 0) {
                step_x = 0;
                step_z++;
            }
            
            grid[i * 6 + 0] = glm::vec3(grid_stepx * step_x,                0.f, grid_stepz * step_z);
            grid[i * 6 + 1] = glm::vec3(grid_stepx * step_x + grid_stepx,   0.f, grid_stepz * step_z);
            grid[i * 6 + 2] = glm::vec3(grid_stepx * step_x + grid_stepx,   0.f, grid_stepz * step_z + grid_stepz);

            grid[i * 6 + 3] = glm::vec3(grid_stepx * step_x,                0.f, grid_stepz * step_z);
            grid[i * 6 + 4] = glm::vec3(grid_stepx * step_x + grid_stepx,   0.f, grid_stepz * step_z + grid_stepz);
            grid[i * 6 + 5] = glm::vec3(grid_stepx * step_x,                0.f, grid_stepz * step_z + grid_stepz);

            step_x++;
        }

        GLuint grid_vao, grid_vbo;
        glCreateVertexArrays(1, &grid_vao);
        glCreateBuffers(1, &grid_vbo);
        glBindVertexArray(grid_vao);
        glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
        glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(glm::vec3), grid.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        ShaderWrapper grid_shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/fluid_simulation/grid.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/fluid_simulation/grid.glsl.fs"))
        );
        grid_shader.bind();

        glm::vec3 eye_position(0.f, 0.f, -5.f);
        glm::vec3 eye_target(0.f);
        ViewTransform view(eye_position, eye_target);

        glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 100.f);
        grid_shader.upload44fm(glm::value_ptr(proj), "proj");

        // Enable wireframe mode for now.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        float deltatime = 0.f;
        float lasttime  = 0.f;
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.f, 0.2f, 0.3f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            deltatime = glfwGetTime() - lasttime;
            lasttime += deltatime;

            auto movement_dir = window_manager.get_movement_direction();
            if(movement_dir != MovementDirection::None) {
                view.translate(movement_dir, deltatime);
            }

            auto camera_offset = window_manager.get_mouse_delta();
            if(camera_offset != glm::vec2(0.f)) {
                view.rotate(camera_offset);
                window_manager.reset_mouse_delta();
            }

            grid_shader.upload44fm(view.get_pointer(), "view");

            glBindVertexArray(grid_vao);
            glDrawArrays(GL_TRIANGLES, 0, gridx * gridz * 3);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &grid_vao);
        glDeleteBuffers(1, &grid_vbo);
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error somewhere!\n";
    }
}