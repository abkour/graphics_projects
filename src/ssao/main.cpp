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

int main(int argc, const char** argv) {
	if(argc != 2) {
		std::cerr << "No obj file specified. Returning." << std::flush;
	}

	glm::vec2 screen_resolution(640, 480);
	WindowManager window_manager(screen_resolution);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(0);

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
	
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned), vertex_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));
	
	GLuint depth_tbo;
	glGenTextures(1, &depth_tbo);
	glBindTexture(GL_TEXTURE_2D, depth_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_resolution.x, screen_resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint tbo;
	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tbo, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete. Returning!\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Quad texture setup
	const float quad_attributes[] = 
	{
		-1.f, -1.f,	0.f, 0.f,
		1.f, -1.f, 	1.f, 0.f,
		1.f, 1.f, 	1.f, 1.f,

		-1.f, -1.f, 	0.f, 0.f,
		1.f, 1.f, 	1.f, 1.f,
		-1.f, 1.f, 	0.f, 1.f
	};

	GLuint quad_vao, quad_vbo;
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_attributes), quad_attributes, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	ShaderWrapper quad_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.fs"))
	);

	ShaderWrapper shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.fs"))
	);
	
	glm::vec3 pos(0.f, 0.f, -1.f);
	glm::vec3 target(0.f);
	ViewTransform view(pos, target);
	glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1000.f);

	float delta_time = 0.f;
	float last_time = 0.f;
	while(!glfwWindowShouldClose(window)) {
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

		shader.bind();
		shader.upload44fm(view.get_pointer(), "view_transform");
		shader.upload44fm(glm::value_ptr(projection), "perspective_projection");
		
		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

		// Render to quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		quad_shader.bind();
		glBindTexture(GL_TEXTURE_2D, tbo);
		glBindTextureUnit(0, tbo);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}