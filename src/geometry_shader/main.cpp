#include <iostream>
#include <shaderdirect.hpp>

#include "../common/view.hpp"
#include "../common/window_manager.hpp"

int main(int argc, char** argv) {
    try {
        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Windowed);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);

        ViewTransform view(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 100.f);

        ShaderWrapper shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/geometry_shader/vertex.glsl.vs")),
            shader_p(GL_GEOMETRY_SHADER, ROOT_DIRECTORY + std::string("/src/geometry_shader/geometry.glsl.gs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/geometry_shader/fragment.glsl.fs"))
        );
        shader.bind();

        float points[] = {
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
            0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
            -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
        }; 

        GLuint vao, vbo;
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

        float delta_time = 0.f;
        float last_time = 0.f;
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.05f, 0.05f, 0.45f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            delta_time = glfwGetTime() - last_time;
            last_time += delta_time;

            //
            // Handle potential user input
            auto mouse_delta = window_manager.get_mouse_delta();
            if(mouse_delta.x != 0.f || mouse_delta.y != 0.f) {
                view.rotate(mouse_delta);
                window_manager.reset_mouse_delta();
            }

            auto movement_direction = window_manager.get_movement_direction();
            if(movement_direction != MovementDirection::None) {
                view.translate(movement_direction, delta_time);
            }

            glDrawArrays(GL_POINTS, 0, 4);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}
