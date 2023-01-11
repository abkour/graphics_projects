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
        glfwSwapInterval(0);

        glEnable(GL_DEPTH_TEST);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        int width, height, hm_channels;
        unsigned char* heightmap = stbi_load("C:/users/flora/dev/graphics_projects/src/tessellation_shader/iceland_heightmap.png", &width, &height, &hm_channels, 0);
        
        GLuint height_texture;
        glGenTextures(1, &height_texture);
        glBindTexture(GL_TEXTURE_2D, height_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, heightmap);

        // free image resource
        stbi_image_free(heightmap);

        // Set up patch vertex count
        glPatchParameteri(GL_PATCH_VERTICES, 4);

        // Set up initial, coarse tessellation of terrain
        std::vector<float> vertices;
        unsigned rez = 20;

        std::cout << "(x,y): " << width << ", " << height << '\n';

        for(int i = 0; i < rez; ++i) {
            for(int j = 0; j < rez; ++j) {
                vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*j/(float)rez); // v.z
                vertices.push_back((i+1) / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-width/2.0f + width*i/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back((j+1) / (float)rez); // v

                vertices.push_back(-width/2.0f + width*(i+1)/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
                vertices.push_back((i+1) / (float)rez); // u
                vertices.push_back((j+1) / (float)rez); // v
            }
        }

        GLuint terrain_vao, terrain_vbo;
        glCreateVertexArrays(1, &terrain_vao);
        glCreateBuffers(1, &terrain_vbo);
        glBindVertexArray(terrain_vao);
        glBindBuffer(GL_ARRAY_BUFFER, terrain_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        ViewTransform view(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1500.f);
        glm::mat4 model = glm::mat4(1.0f);

        view.set_movement_scale(100.f);

        ShaderWrapper shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/tessellation_shader/vertex.glsl.vs")),
            shader_p(GL_TESS_CONTROL_SHADER, ROOT_DIRECTORY + std::string("/src/tessellation_shader/tess_control.glsl.tcs")),
            shader_p(GL_TESS_EVALUATION_SHADER, ROOT_DIRECTORY + std::string("/src/tessellation_shader/tess_eval.glsl.tes")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/tessellation_shader/fragment.glsl.fs"))
        );
        shader.bind();
        shader.upload44fm(glm::value_ptr(proj), "proj");
        shader.upload44fm(glm::value_ptr(model), "model");

        float delta_time = 0.f;
        float last_time = 0.f;
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.0f, 0.0f, 0.0f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            delta_time = glfwGetTime() - last_time;
            last_time += delta_time;

            std::string frametime_str = std::to_string(static_cast<int>(1.f / delta_time)) + " FPS"; 
            window_manager.set_title(frametime_str.c_str());

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

            shader.bind();
            shader.upload44fm(view.get_pointer(), "view");
            glBindVertexArray(terrain_vao);
            glBindTexture(GL_TEXTURE_2D, height_texture);
            glBindTextureUnit(0, height_texture);
            glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        glDeleteVertexArrays(1, &terrain_vao);
        glDeleteBuffers(1, &terrain_vbo);
        glDeleteTextures(1, &height_texture);
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}
