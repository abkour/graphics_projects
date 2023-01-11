#include <iostream>
#include <shaderdirect.hpp>

#include "../common/window_manager.hpp"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

struct Projectile {

    Projectile() = default;

    Projectile(const glm::vec3& pColor, const glm::vec3& pPos, const float pLifetime)
        : color(pColor)
        , pos(pPos)
        , lifetime(pLifetime)
    {}

    void tick(const float deltatime) {
        lifetime -= deltatime;
    }

    float lifetime;
    glm::vec3 pos;
    glm::vec3 color;
};

int main(int argc, char** argv) {
    try {
        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Windowed);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(0);

        const float aspect_ratio = screen_resolution.x / screen_resolution.y;
        const int NUMBER_OF_POINTS_ON_CIRCLE = 39;
        float circle[NUMBER_OF_POINTS_ON_CIRCLE * 2];
        float angle_radians = 0.f;
        circle[0] = 0.f;
        circle[1] = 0.f;
        for(int i = 1; i < NUMBER_OF_POINTS_ON_CIRCLE; ++i) {
            circle[i * 2] = sin(glm::radians(angle_radians)) / aspect_ratio;
            circle[i * 2 + 1] = cos(glm::radians(angle_radians));
            angle_radians += 10.f;
        }

        GLuint circle_vao, circle_vbo;
        glGenVertexArrays(1, &circle_vao);
        glBindVertexArray(circle_vao);
        glGenBuffers(1, &circle_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, circle_vbo);
        glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_POINTS_ON_CIRCLE * 2 * sizeof(float), circle, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);        

        const float interleaved_quad[] = 
        {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f, 1.f, 0.f,
            1.f, 1.f, 1.f, 1.f,

            -1.f, -1.f, 0.f, 0.f,
            1.f, 1.f, 1.f, 1.f,
            -1.f, 1.f, 0.f, 1.f
        };

        GLuint quad_vao, quad_vbo;
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved_quad), interleaved_quad, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

        GLuint frame_texture;
        glGenTextures(1, &frame_texture);
        glBindTexture(GL_TEXTURE_2D, frame_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer incomplete!\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ShaderWrapper quad_shader(
            false, 
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/projectiles/quad.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/projectiles/quad.glsl.fs"))
        );

        ShaderWrapper simple_shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/projectiles/vertex.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/projectiles/fragment.glsl.fs"))
        );
        simple_shader.bind();

        Projectile p1(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.75, 0.f), 2.f);
        Projectile p2(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), 2.f);

        const float size_scale = 0.02f;
        glm::mat4 model(1.f);

        glm::vec3 projectile_dir(0.f, -1.f, 0.f);

        float delta_time = 0.f;
        float last_time = 0.f;
        while(!glfwWindowShouldClose(window)) {
            delta_time = glfwGetTime() - last_time;
            last_time += delta_time;

            std::string frametime_str = std::to_string(static_cast<int>(1.f / delta_time)) + " FPS"; 
            window_manager.set_title(frametime_str.c_str());

            // Compute logic
            glm::vec3 to_pos2 = glm::normalize(p2.pos - p1.pos);
            to_pos2 = glm::normalize((to_pos2) + (projectile_dir / 2.f));
            glm::vec3 step = to_pos2 * 0.3f * delta_time;

            // Render to texture
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
            p1.tick(delta_time);
            if(p1.lifetime > 0.f) {
                simple_shader.bind();
                glBindVertexArray(circle_vao);
                model = glm::mat4(1.f);
                model = glm::translate(model, p1.pos + step);
                p1.pos += step;
                model = glm::scale(model, glm::vec3(size_scale * 0.5f));
                simple_shader.upload44fm(glm::value_ptr(model), "model");
                simple_shader.upload3fv(&p1.color.x, "color");
                glDrawArrays(GL_TRIANGLE_FAN, 0, NUMBER_OF_POINTS_ON_CIRCLE);
            }

            // Compute logic
            simple_shader.bind();
            glBindVertexArray(circle_vao);
            glm::vec3 pos_2_step(-0.00001f, 0.f, 0.f);
            p2.pos += pos_2_step;
            model = glm::mat4(1.f);
            model = glm::translate(model, p2.pos);
            model = glm::scale(model, glm::vec3(size_scale));
            simple_shader.upload44fm(glm::value_ptr(model), "model");
            simple_shader.upload3fv(&p2.color.x, "color");
            glDrawArrays(GL_TRIANGLE_FAN, 0, NUMBER_OF_POINTS_ON_CIRCLE);

            // Render texture onto quad
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.1f, 0.1f, 0.1f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT);

            quad_shader.bind();
            glBindVertexArray(quad_vao);
            glBindTexture(GL_TEXTURE_2D, frame_texture);
            glBindTextureUnit(0, frame_texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &circle_vao);
        glDeleteBuffers(1, &circle_vbo);
        glDeleteTextures(1, &frame_texture);
        glDeleteFramebuffers(1, &fbo);
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}
