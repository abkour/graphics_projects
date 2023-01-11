#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set up patch vertex count
        glPatchParameteri(GL_PATCH_VERTICES, 2);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        static const float vertices[] = 
        {
            -3.f, 0.f, 3.f, 0.f,
        };
        
        const int nVertices = sizeof(vertices) / (sizeof(float) * 2);

        GLuint vao, vbo;
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

        ShaderWrapper shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/curved_surface/vertex.glsl.vs")),
            shader_p(GL_TESS_CONTROL_SHADER, ROOT_DIRECTORY + std::string("/src/curved_surface/tcs.glsl.tcs")),
            shader_p(GL_TESS_EVALUATION_SHADER, ROOT_DIRECTORY + std::string("/src/curved_surface/tes.glsl.tes")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/curved_surface/fragment.glsl.fs"))
        );
        shader.bind();

        glLineWidth(2);

        float delta_time = 0.f;
        float last_time = 0.f;
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.0f, 0.0f, 0.0f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            delta_time = glfwGetTime() - last_time;
            last_time += delta_time;

            std::string frametime_str = std::to_string(static_cast<int>(1.f / delta_time)) + " FPS"; 
            window_manager.set_title(frametime_str.c_str());

            shader.upload1fv(&last_time, "time");

            glDrawArrays(GL_PATCHES, 0, nVertices);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}
