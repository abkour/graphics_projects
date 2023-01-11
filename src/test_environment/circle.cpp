// Standard library dependencies
#include <iostream>
#include <random>

// Third party External dependencies
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <shaderdirect.hpp>

// Application headers
#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
	return os << vec.x << ", " << vec.y;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << vec.x << ", " << vec.y << ", " << vec.z;
}

int main() {
    glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Windowed);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Quad texture setup
	const float quad_attributes[] = 
	{
		-1.f, -1.f,	0.f, 0.f,
		1.f, -1.f, 	1.f, 0.f,
		1.f, 1.f, 	1.f, 1.f,

		-1.f, -1.f, 0.f, 0.f,
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

    ShaderWrapper circle_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/test_environment/circle.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/test_environment/circle.glsl.fs"))
	);
    circle_shader.bind();
    circle_shader.upload2fv(&screen_resolution.x, "sres");

	ShaderWrapper rorate_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/test_environment/rotate.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/test_environment/rotate.glsl.fs"))
	);
	rorate_shader.bind();

	static const float quad[] = 
	{
		-0.25f, -0.25f,
		0.25f, -0.25f,
		0.25f, 0.25f,
		-0.25f, -0.25f,
		0.25f, 0.25f,
		-0.25f, 0.25f
	};

    GLuint rotate_vao, rotate_vbo;
	glGenVertexArrays(1, &rotate_vao);
	glGenBuffers(1, &rotate_vbo);
	glBindVertexArray(rotate_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rotate_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	glm::mat4 model_transform = glm::mat4(1.f);
	model_transform = glm::rotate(model_transform, glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));


    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

		rorate_shader.upload44fm(glm::value_ptr(model_transform), "model");

		glBindVertexArray(rotate_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
}