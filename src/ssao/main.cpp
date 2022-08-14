// Standard library dependencies
#include <iostream>

// Third party External dependencies
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

// Application headers
#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
	return os << vec.x << ", " << vec.y;
}

#include <set>

int main(int argc, const char** argv) {
	if(argc != 2) {
		std::cerr << "No obj file specified. Returning." << std::flush;
	}

	glm::vec2 screen_resolution(640, 480);
	WindowManager window_manager(screen_resolution);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glEnable(GL_DEPTH_TEST);

	// Enable wireframe mode
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Model model;
	model.parse(argv[1]);
	
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
	
	// EBO
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned), vertex_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));

	ShaderWrapper shader(
		false,
		shader_p(GL_VERTEX_SHADER, "../../src/ssao/ssao_shader.glsl.vs"),
		shader_p(GL_FRAGMENT_SHADER, "../../src/ssao/ssao_shader.glsl.fs")
	);
	shader.bind();
	
	glm::vec3 triangle_color(0.f, 1.f, 0.5f);
	shader.upload3fv(&triangle_color.x, "color");

	glm::vec3 pos(0.f, 0.f, -1.f);
	glm::vec3 target(0.f);
	ViewTransform view(pos, target);
	glm::mat4 projection = 
				glm::perspective(	glm::radians(45.f), 
									screen_resolution.x / screen_resolution.y, 
									0.1f, 
									1000.f);

	float delta_time = 0.f;
	float last_time = 0.f;
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		delta_time = glfwGetTime() - last_time;
		last_time += delta_time;

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

		shader.upload44fm(view.get_pointer(), "view_transform");
		shader.upload44fm(glm::value_ptr(projection), "perspective_projection");
		
		glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
}