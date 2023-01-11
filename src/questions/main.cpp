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

void cvv(const char* filename);

// Call with cornell-refl.scene
int main(int argc, const char** argv) {
	try {
		if(argc != 2) {
			std::cout << "Specify obj file!\n";
			return -1;
		}
		cvv(argv[1]);
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << v.x << ", " << v.y << ", " << v.z;
}

void cvv(const char* filename) {
    glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Fullscreen, true);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);

    ShaderWrapper light_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/questions/scene.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/questions/scene.glsl.fs"))
    );
    light_shader.bind();
    light_shader.upload2fv(&screen_resolution.x, "screen_resolution");

    ShaderWrapper shadow_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/questions/quad.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/questions/quad.glsl.fs"))
    );

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
	glBufferData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float), interleaved_attributes.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned), vertex_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));

    GLfloat quad[] =  
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    GLuint depth_texture;
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_resolution.x, screen_resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint quad_texture;
    glGenTextures(1, &quad_texture);
    glBindTexture(GL_TEXTURE_2D, quad_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_resolution.x, screen_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLuint custom_fbo;
    glGenFramebuffers(1, &custom_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, custom_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quad_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    unsigned int buffer_attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffer_attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Incomplete framebuffer!\n";
    }

    ViewTransform light_view(glm::vec3(0.f, 3.f, 0.f), glm::vec3(0.005f, -0.99f, 0.f));
    float light_near = 1.f;
    float light_far = 20.f;
    glm::mat4 light_proj = glm::ortho(-10.f, 10.f, -10.f, 10.f, light_near, light_far);
    glm::mat4 light_MVP = light_proj * light_view.get_view_transform();

    ViewTransform view_transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f, 0.f, -2.f));

    float near = 0.1f;
    float far = 20.f;
    glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, near, far);

    light_shader.bind();
    light_shader.upload44fm(glm::value_ptr(light_MVP), "lightMVP");
    light_shader.upload1fv(&light_near, "near");
    light_shader.upload1fv(&light_far, "far");

    bool key_k = false;

    float delta_time = 0.f;
    float last_time = 0.f;
    while(!glfwWindowShouldClose(window)) {
        delta_time = glfwGetTime() - last_time;
        last_time += delta_time;

		// Handle potential user input
		auto mouse_delta = window_manager.get_mouse_delta();
		if(mouse_delta.x != 0.f || mouse_delta.y != 0.f) {
			light_view.rotate(mouse_delta);
			window_manager.reset_mouse_delta();
		}

		auto movement_direction = window_manager.get_movement_direction();
		if(movement_direction != MovementDirection::None) {
			light_view.translate(movement_direction, delta_time);
		}

        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !key_k) {
            key_k = true;
        }
        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE && key_k) {
            std::cout << "Pos: " << light_view.get_position() << '\n';
            std::cout << "Dir: " << light_view.get_direction() << '\n';
            key_k = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.2f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //light_MVP = light_proj * light_view.get_view_transform();
        light_shader.bind();
        light_shader.upload44fm(glm::value_ptr(light_MVP), "lightMVP");
        glBindTextureUnit(0, quad_texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

/*  
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
        quad_shader.bind();
        glBindTextureUnit(0, depth_texture);
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
*/
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteTextures(1, &quad_texture);
    glDeleteTextures(1, &depth_texture);
    glDeleteFramebuffers(1, &custom_fbo);
}