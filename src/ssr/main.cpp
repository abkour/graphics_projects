// Standard library dependencies
#include <iostream>

// Third party External dependencies
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

// Application headers
#include "../common/logfile.hpp"
#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

void ssr(const char* filename);

// Call with cornell-refl.scene
int main(int argc, const char** argv) {
	try {
		if(argc != 2) {
			std::cout << "Specify obj file!\n";
			return -1;
		}
		ssr(argv[1]);
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << v.x << ", " << v.y << ", " << v.z;
}

void ssr(const char* filename) {
    glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Windowed);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);

    static const float interleaved_quad[] = 
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

    const float test_distance = std::numeric_limits<float>::max();

    float far_clip_distance = 20.f;
    float inv_far_clip_distance = -20.f;

    static const unsigned char reflection_mask[] = 
    {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 0, 0, 0, // floor
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    static const float colors[] = 
    {
        1.f, 1.f, 1.f, // Light
        1.f, 1.f, 1.f, // Light
        1.f, 1.f, 1.f, // Light
        1.f, 1.f, 1.f, // Light

        0.93f, 0.63f, 0.13f, // White
        0.93f, 0.63f, 0.13f, // White
        0.93f, 0.63f, 0.13f, // White
        0.93f, 0.63f, 0.13f, // White

        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White

        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White

        0.65f, 0.05f, 0.05f, // Red
        0.65f, 0.05f, 0.05f, // Red
        0.65f, 0.05f, 0.05f, // Red
        0.65f, 0.05f, 0.05f, // Red

        0.12f, 0.45f, 0.12f, // Green
        0.12f, 0.45f, 0.12f, // Green
        0.12f, 0.45f, 0.12f, // Green
        0.12f, 0.45f, 0.12f, // Green

        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White

        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
        0.73f, 0.73f, 0.73f, // White
    };

    Model model;
	model.parse(filename);

    std::vector<unsigned> vertex_indices(model.GetFaceCount());
	for(int i = 0; i < vertex_indices.size(); ++i) {
		vertex_indices[i] = model.GetFace(i).p;
	}

	auto interleaved_attributes = model.GetInterleavedAttributes();

	GLuint ebo, vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float) + sizeof(colors) + sizeof(reflection_mask), 
                    NULL, 
                    GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_attributes.size() * sizeof(float), interleaved_attributes.data());
    glBufferSubData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float), sizeof(colors), colors);
    glBufferSubData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float) + sizeof(colors), sizeof(reflection_mask), reflection_mask);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned), vertex_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(interleaved_attributes.size() * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, 1 * sizeof(unsigned char), reinterpret_cast<void*>(interleaved_attributes.size() * sizeof(float) + sizeof(colors)));

    const int nDispatches = (screen_resolution.x * screen_resolution.y / 256) + 1;
    std::cout << "Number of dispatches: " << nDispatches << '\n';

    std::vector<glm::vec4> ptr_data(screen_resolution.x * screen_resolution.y, glm::vec4(0.f));
    GLuint ssr_ssbo;
    glGenBuffers(1, &ssr_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssr_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, screen_resolution.x * screen_resolution.y * sizeof(glm::vec4), NULL, GL_STATIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssr_ssbo);

    GLuint ssr_texture;
    glGenTextures(1, &ssr_texture);
    glBindTexture(GL_TEXTURE_2D, ssr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_resolution.x, screen_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    ShaderWrapper quad_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/quad.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/quad.glsl.fs"))
    );

    ShaderWrapper depth_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/quad.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/quad_depth.glsl.fs"))
    );

    ShaderWrapper ssr_shader(
        false,
        shader_p(GL_COMPUTE_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/ssr.glsl.comp"))
    );
    ssr_shader.bind();
    ssr_shader.upload2fv(&screen_resolution.x, "screen_resolution");

    int tex_ids[] = { 0, 1, 2, 3 };
    ssr_shader.upload1iv(&tex_ids[0], "tColor");
    ssr_shader.upload1iv(&tex_ids[1], "tNormal");
    ssr_shader.upload1iv(&tex_ids[2], "tDepth");
    ssr_shader.upload1iv(&tex_ids[3], "tRefl");

    ShaderWrapper g_pass_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/g_pass.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssr/g_pass.glsl.fs"))
    );
    g_pass_shader.bind();

    GLuint g_pass_fbo;
    glGenFramebuffers(1, &g_pass_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, g_pass_fbo);

    GLuint depth_texture;
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_resolution.x, screen_resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint reflection_texture;
    glGenTextures(1, &reflection_texture);
    glBindTexture(GL_TEXTURE_2D, reflection_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_resolution.x, screen_resolution.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLfloat test_border_color[] = { test_distance, test_distance, test_distance, test_distance };
    GLuint depthVS_texture;
    glGenTextures(1, &depthVS_texture);
    glBindTexture(GL_TEXTURE_2D, depthVS_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, screen_resolution.x, screen_resolution.y, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, test_border_color);

    //GLfloat border_color[] = { 1.f, 0.f, 1.f, 1.f };
    GLuint color_texture;
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    GLuint normal_texture;
    glGenTextures(1, &normal_texture);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depthVS_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, reflection_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    unsigned int attachments[] = {  GL_COLOR_ATTACHMENT0, 
                                    GL_COLOR_ATTACHMENT1, 
                                    GL_COLOR_ATTACHMENT2,
                                    GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Filter Framebuffer not complete. Returning!\n";
	}

    ViewTransform view_transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f));
    glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 1.f, far_clip_distance);
    glm::mat4 inv_proj = glm::inverse(proj);

    g_pass_shader.upload44fm(glm::value_ptr(proj), "proj");
    g_pass_shader.upload1fv(&far_clip_distance, "far_clip_distance");

    ssr_shader.bind();
    ssr_shader.upload1fv(&far_clip_distance, "far_clip_distance");
    ssr_shader.upload44fm(glm::value_ptr(proj), "proj");
    ssr_shader.upload44fm(glm::value_ptr(inv_proj), "inv_proj");

    glm::ivec2 tex_dimensions(screen_resolution.x, screen_resolution.y);

    float last_time = 0.f;
    float delta_time = 0.f;
    while(!glfwWindowShouldClose(window)) {
        delta_time = glfwGetTime() - last_time;
        last_time += delta_time;

		// Handle potential user input
		auto mouse_delta = window_manager.get_mouse_delta();
		if(mouse_delta.x != 0.f || mouse_delta.y != 0.f) {
			view_transform.rotate(mouse_delta);
			window_manager.reset_mouse_delta();
		}

		auto movement_direction = window_manager.get_movement_direction();
		if(movement_direction != MovementDirection::None) {
			view_transform.translate(movement_direction, delta_time);
		}

        glViewport(0, 0, screen_resolution.x, screen_resolution.y);
        glBindFramebuffer(GL_FRAMEBUFFER, g_pass_fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        g_pass_shader.bind();
        g_pass_shader.upload44fm(view_transform.get_pointer(), "view");
        glBindTextureUnit(0, color_texture);
        glBindTextureUnit(1, normal_texture);
        glBindTextureUnit(2, depthVS_texture);
        glBindTextureUnit(3, reflection_texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);
        
        glViewport(0, screen_resolution.y / 2, screen_resolution.x / 2, screen_resolution.y / 2);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
        quad_shader.bind();
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glBindTextureUnit(0, color_texture);
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Normal texture
        glViewport( screen_resolution.x / 2, 
                    screen_resolution.y / 2, 
                    screen_resolution.x / 2, 
                    screen_resolution.y / 2);
        glBindTexture(GL_TEXTURE_2D, normal_texture);
        glBindTextureUnit(0, normal_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Depth texture
        glViewport( 0, 
                    0, 
                    screen_resolution.x / 2, 
                    screen_resolution.y / 2);
        depth_shader.bind();
        glBindTexture(GL_TEXTURE_2D, depthVS_texture);
        glBindTextureUnit(0, depthVS_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ssr_shader.bind();
        glm::mat4 inv_view = glm::inverse(view_transform.get_view_transform());
        ssr_shader.upload44fm(view_transform.get_pointer(), "view");
        ssr_shader.upload44fm(glm::value_ptr(inv_view), "inv_view");
        ssr_shader.upload2iv(&tex_dimensions.x, "tex_dimension");
        glBindTextureUnit(0, color_texture);
        glBindTextureUnit(1, normal_texture);
        glBindTextureUnit(2, depth_texture);
        glBindTextureUnit(3, reflection_texture);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssr_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssr_ssbo);
        glDispatchCompute(nDispatches, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, screen_resolution.x * screen_resolution.y * sizeof(glm::vec4), ptr_data.data());
        glBindTexture(GL_TEXTURE_2D, ssr_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screen_resolution.x, screen_resolution.y, GL_RGBA, GL_FLOAT, ptr_data.data());

        quad_shader.bind();
        glViewport( screen_resolution.x / 2, 
                    0, 
                    screen_resolution.x / 2, 
                    screen_resolution.y / 2);
        glBindTextureUnit(0, ssr_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &ssr_ssbo);
    glDeleteFramebuffers(1, &g_pass_fbo);
    glDeleteTextures(1, &color_texture);
    glDeleteTextures(1, &normal_texture);
    glDeleteTextures(1, &depth_texture);
    glDeleteTextures(1, &reflection_texture);
    glDeleteTextures(1, &ssr_texture);
    glDeleteTextures(1, &depthVS_texture);
}