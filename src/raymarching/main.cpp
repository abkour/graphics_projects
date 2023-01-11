#include <iostream>
#include <shaderdirect.hpp>

#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    return os << vec.x << ' ' << vec.y << ' ' << vec.z;
}

int main(int argc, char** argv) {
    try {
        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);

        static const float quad_interleaved[] = 
        {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f,  1.f, 0.f,
            1.f, 1.f,   1.f, 1.f,
            -1.f, -1.f, 0.f, 0.f,
            1.f, 1.f,   1.f, 1.f,
            -1.f, 1.f,   0.f, 1.f
        };

        GLuint out_tex;
        glCreateTextures(GL_TEXTURE_2D, 1, &out_tex);
        glBindTexture(GL_TEXTURE_2D, out_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_resolution.x, screen_resolution.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLuint quad_vao, quad_vbo;
        glCreateVertexArrays(1, &quad_vao);
        glCreateBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_interleaved), quad_interleaved, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

        ShaderWrapper quad_shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/raymarching/quad_shader.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/raymarching/quad_shader.glsl.fs"))
        );
        quad_shader.bind();
        quad_shader.upload2fv(&screen_resolution.x, "sres");


        float deltatime = 0.f;
        float lasttime = 0.f;
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.2f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            deltatime = glfwGetTime() - lasttime;
            lasttime += deltatime;

            quad_shader.upload1fv(&lasttime, "time");

            glBindVertexArray(quad_vao);
            glBindTexture(GL_TEXTURE_2D, out_tex);
            glBindTextureUnit(0, out_tex);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}