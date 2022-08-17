// Standard library dependencies
#include <iostream>
#include <random>

// Third party External dependencies
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

// Application headers
#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

struct RandomEngine {
	RandomEngine() = default;
	
	RandomEngine(float rmin, float rmax)
		: dist(rmin, rmax)
	{
		std::random_device rd;
		mt.seed(rd());
	}

	float operator()() {
		return dist(mt);
	}	

	std::mt19937 mt;
	std::uniform_real_distribution<float> dist;
};

std::ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
	return os << vec.x << ", " << vec.y;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << vec.x << ", " << vec.y << ", " << vec.z;
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

	const float quad_attributes2[] = 
	{
		-0.5f, -0.5f,	0.f, 0.f,
		0.5f, -0.5f, 	1.f, 0.f,
		0.5f, 0.5f, 	1.f, 1.f,

		-0.5f, -0.5f, 	0.f, 0.f,
		0.5f, 0.5f, 	1.f, 1.f,
		-0.5f, 0.5f, 	0.f, 1.f
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

	GLuint position_tbo;
	glGenTextures(1, &position_tbo);
	glBindTexture(GL_TEXTURE_2D, position_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint normal_tbo;
	glGenTextures(1, &normal_tbo);
	glBindTexture(GL_TEXTURE_2D, normal_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_resolution.x, screen_resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_2D, position_tbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tbo, 0);
	glBindTexture(GL_TEXTURE_2D, normal_tbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tbo, 0);
	
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glBindTexture(GL_TEXTURE_2D, depth_tbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tbo, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete. Returning!\n";
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// SSAO Setup
	RandomEngine r_device(-1.f, 1.f);
	float kernel_size = 64;
	std::vector<glm::vec4> kernel(static_cast<int>(kernel_size));
	for(int i = 0; i < kernel_size; ++i) {
		kernel[i] = glm::vec4(r_device(), r_device(), (r_device() + 1.f) * 0.5f, 0.f);
		kernel[i] = glm::normalize(kernel[i]);
		float scale = static_cast<float>(i) / static_cast<float>(kernel_size);
		scale = glm::mix(0.1f, 1.f, scale * scale);
		kernel[i] *= scale;
	}

	int noise_size = 16;
	std::vector<glm::vec3> noise(noise_size);
	for(int i = 0; i < noise_size; ++i) {
		noise[i] = glm::normalize(glm::vec3(r_device(), r_device(), 0.f));
	}

	GLuint noise_tbo;
	glGenTextures(1, &noise_tbo);
	glBindTexture(GL_TEXTURE_2D, noise_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, noise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLuint ssao_fbo;
	glGenFramebuffers(1, &ssao_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);

	GLuint ambient_tbo;
	glGenTextures(1, &ambient_tbo);
	glBindTexture(GL_TEXTURE_2D, ambient_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_resolution.x, screen_resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ambient_tbo, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "SSAO Framebuffer not complete. Returning!\n";
	}

	GLuint constant_ssbo;
	glGenBuffers(1, &constant_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, constant_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * kernel.size(), kernel.data(), GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, constant_ssbo);

	if(int error = glGetError()) {
		std::cout << std::hex << error << '\n';
	}

	ShaderWrapper ssao_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.fs"))
	);

	ShaderWrapper g_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/g_shader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/g_shader.glsl.fs"))
	);

	ShaderWrapper quad_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.fs"))
	);
	
	if(int error = glGetError()) {
		std::cout << "shader compile: " << std::hex << error << '\n';
	}

	quad_shader.bind();
	int attachment_ids[] = { 0, 1, 2 };
	quad_shader.upload1iv(&attachment_ids[0], "tPosition");
	quad_shader.upload1iv(&attachment_ids[1], "tNormal");

	ssao_shader.bind();
	ssao_shader.upload1iv(&attachment_ids[0], "tPosition");
	ssao_shader.upload1iv(&attachment_ids[1], "tNormal");
	ssao_shader.upload1iv(&attachment_ids[2], "tNoise");

	glm::vec3 pos(0.f, 0.f, -1.f);
	glm::vec3 target(0.f);
	ViewTransform view(pos, target);
	glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1000.f);

	if(int error = glGetError()) {
		std::cout << std::hex << error << '\n';
	}

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

		//
		// Render to framebuffer
		g_shader.bind();
		g_shader.upload44fm(view.get_pointer(), "view_transform");
		g_shader.upload44fm(glm::value_ptr(projection), "perspective_projection");
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);
		
		if(int error = glGetError()) {
			std::cout << "Prepass stage: " << std::hex << error << '\n';
		}

		//
		// SSAO shader
		ssao_shader.bind();
		ssao_shader.upload44fm(glm::value_ptr(projection), "proj");
		ssao_shader.upload2fv(glm::value_ptr(screen_resolution), "screen_resolution");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, position_tbo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal_tbo);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noise_tbo);

		glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, constant_ssbo);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if(int error = glGetError()) {
			std::cout << "SSAO stage: " << std::hex << error << '\n';
		}

		//
		// Render final quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		quad_shader.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ambient_tbo);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if(int error = glGetError()) {
			std::cout << "Quad stage: " << std::hex << error << '\n';
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}