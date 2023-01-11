// Standard library dependencies
#include <iostream>
#include <random>

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

struct PointLight {
	
	PointLight(float pIntensity, glm::vec3& pPosition, glm::vec3& pColor, glm::vec3& pAttenuation)
		: intensity(pIntensity)
		, position(pPosition)
		, color(pColor)
		, attenuation(pAttenuation)
	{}

	float intensity;
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 attenuation;
	glm::vec2 padding;
};

void render_point_lights(const char* filename);

int main(int argc, const char** argv) {
	try {
		if(argc != 2) {
			std::cout << "Specify obj file!\n";
			return -1;
		}
		render_point_lights(argv[1]);
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

void render_point_lights(const char* filename) {
    glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

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
	
    ShaderWrapper shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/point_lights/vertex.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/point_lights/fragment.glsl.fs"))
    );
    shader.bind();

    ViewTransform view_transform(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f));
    glm::mat4 proj = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1000.f);

    shader.upload44fm(glm::value_ptr(proj), "projection");

	PointLight point_light(2.f, glm::vec3(-0.5f, 1.f, -2.5f), glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 0.5f, 2.f));
	shader.upload1fv(&point_light.intensity, "pointLight.intensity");	
	shader.upload3fv(&point_light.position.x, "pointLight.position");	
	shader.upload3fv(&point_light.color.x, "pointLight.color");	
	shader.upload3fv(&point_light.attenuation.x, "pointLight.attenuation");	
	shader.upload2fv(&point_light.padding.x, "pointLight.padding");	

    float last_time = 0.f;
    float delta_time = 0.f;
	while(!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        shader.bind();
        shader.upload44fm(view_transform.get_pointer(), "view");

        glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}