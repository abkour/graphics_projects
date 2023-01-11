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

    float far_clip_distance = 20.f;
    float inv_far_clip_distance = -20.f;

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
	glBufferData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, interleaved_attributes.size() * sizeof(float), interleaved_attributes.data());
    glBufferSubData(GL_ARRAY_BUFFER, interleaved_attributes.size() * sizeof(float), sizeof(colors), colors);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned), vertex_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(interleaved_attributes.size() * sizeof(float)));

    ShaderWrapper eye_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/mirror/vert.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/mirror/frag.glsl.fs"))
    );
    eye_shader.bind();

    ViewTransform view_transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f));
    glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 1.f, far_clip_distance);
    glm::mat4 inv_proj = glm::inverse(proj);

    eye_shader.upload44fm(glm::value_ptr(proj), "proj");

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

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        eye_shader.upload44fm(view_transform.get_pointer(), "view");
        glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}