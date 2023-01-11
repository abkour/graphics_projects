#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <shaderdirect.hpp>

#include "../common/model.hpp"
#include "../common/perlin.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec4 vec) {
    return os << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
}

int main(int argc, char** argv) {
    try {
        if(argc != 2) return -1;

        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Windowed);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);

        const glm::vec2 grid_dimensions(512);
        const glm::vec2 grid_vertices(8);
        std::vector<float> perlin_noise(grid_dimensions.x * grid_dimensions.y);

        GLuint err = glGetError();
        if(err != GL_NO_ERROR) {
            std::cout << "Error later: " << std::hex << err << '\n';
        }

        perlin_noise_2d(perlin_noise, grid_dimensions, grid_vertices);
        
        GLuint plane_texture;
        glGenTextures(1, &plane_texture);
        glBindTexture(GL_TEXTURE_2D, plane_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, grid_dimensions.x, grid_dimensions.y, 0, GL_RED, GL_FLOAT, perlin_noise.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Model model;
        model.parse(argv[1]);

        static const float uv_coordinates[] = 
        {
            0.f, 1.f,
            0.f, 0.f,
            1.f, 0.f,

            0.f, 1.f,
            1.f, 0.f,
            1.f, 1.f
        };
        
        std::vector<float> interleaved_attributes(model.GetFaceCount() * 5);
        for(int i = 0; i < model.GetFaceCount(); ++i) {
            interleaved_attributes[i * 5 + 0] = model.GetVertex(model.GetFace(i).p).x;
            interleaved_attributes[i * 5 + 1] = model.GetVertex(model.GetFace(i).p).y;
            interleaved_attributes[i * 5 + 2] = model.GetVertex(model.GetFace(i).p).z;
            
            interleaved_attributes[i * 5 + 3] = uv_coordinates[(i % 6) * 2];
            interleaved_attributes[i * 5 + 4] = uv_coordinates[(i % 6) * 2 + 1];
        }

        std::cout << interleaved_attributes.size() << '\n';

        const float plane_x = 16;
        const float plane_z = 16;
        static const float plane_vertices[] = 
        {
            -plane_x / 2.f, -0.5f, -plane_z / 2.f,
            plane_x / 2.f, -0.5f, -plane_z / 2.f,  
            plane_x / 2.f, -0.5f, plane_z / 2.f,  

            -plane_x / 2.f, -0.5f, -plane_z / 2.f,
            plane_x / 2.f, -0.5f, plane_z / 2.f,  
            -plane_x / 2.f, -0.5f, plane_z / 2.f 
        };

        static const float uv_plane[] = 
        {
            0.f, 0.f,    
            1.f, 0.f,    
            1.f, 1.f,

            0.f, 0.f,    
            1.f, 1.f,    
            0.f, 1.f
        };

        GLuint plane_vao, plane_vbo;
        glGenVertexArrays(1, &plane_vao);
        glGenBuffers(1, &plane_vbo);
        glBindVertexArray(plane_vao);
        glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices) + sizeof(uv_plane), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(plane_vertices), plane_vertices);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(plane_vertices), sizeof(uv_plane), uv_plane);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(sizeof(plane_vertices)));

        GLuint quad_vao, quad_vbo;
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float), interleaved_attributes.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));

        int tex_width, tex_height, nChannels;
        const char* tex_path = "C://Users//flora//dev//graphics_projects//src//foliage//grass_3.png";
        //const char* tex_path = (ROOT_DIRECTORY + std::string("/src/foliage/grass.jpg")).c_str();
        stbi_set_flip_vertically_on_load(true);
        unsigned char* gtexture_buffer = stbi_load(tex_path, &tex_width, &tex_height, &nChannels, 0);
        if (!gtexture_buffer) {
            std::cout << "Error loading file!\n";
        }

        const uint32_t nGrassInstances = 256;

        GLuint offset_buffer;
        glGenBuffers(1, &offset_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, offset_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * nGrassInstances, NULL, GL_STATIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offset_buffer);

        GLuint grass_texture;
        glGenTextures(1, &grass_texture);
        glBindTexture(GL_TEXTURE_2D, grass_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gtexture_buffer);


        glm::vec2 grid_size(16, 16);
        glm::vec2 cell_size(1.f, 1.f);
        glm::vec3 grid_origin(-8.f, 0.f, -8.f);

        ShaderWrapper position_assignment_shader(
            false,
            shader_p(GL_COMPUTE_SHADER, ROOT_DIRECTORY + std::string("/src/foliage/position_assignment.glsl.comp"))
        );
        position_assignment_shader.bind();
        position_assignment_shader.upload2fv(&grid_size.x, "grid_size");
        position_assignment_shader.upload2fv(&cell_size.x, "cell_size");
        position_assignment_shader.upload3fv(&grid_origin.x, "origin");
        glDispatchCompute(nGrassInstances, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        ShaderWrapper plane_shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/foliage/plane.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/foliage/plane.glsl.fs"))
        );

        ShaderWrapper grass_shader(
            false,
            shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/foliage/grass.glsl.vs")),
            shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/foliage/grass.glsl.fs"))
        );
        grass_shader.bind();

        ViewTransform view(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 100.f);
        grass_shader.upload44fm(glm::value_ptr(proj), "proj");
        
        plane_shader.bind();
        plane_shader.upload44fm(glm::value_ptr(proj), "proj");

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

            plane_shader.bind();
            plane_shader.upload44fm(view.get_pointer(), "view");
            glBindTexture(GL_TEXTURE_2D, plane_texture);
            glBindTextureUnit(0, plane_texture);
            glBindVertexArray(plane_vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
/*
            grass_shader.bind();
            grass_shader.upload44fm(view.get_pointer(), "view");
            glBindVertexArray(quad_vao);
            glBindTexture(GL_TEXTURE_2D, grass_texture);
            glBindTextureUnit(0, grass_texture);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 24, nGrassInstances);
*/        
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // clean up
        stbi_image_free(gtexture_buffer);
        glDeleteVertexArrays(1, &quad_vao);
        glDeleteBuffers(1, &quad_vbo);
        glDeleteTextures(1, &grass_texture);

    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cout << "Unknown error!\n";
    }
}