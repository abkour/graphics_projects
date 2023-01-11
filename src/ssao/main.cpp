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

void render_ssao(const char* filename);
void performance_testbed(const char* filename);

int main(int argc, const char** argv) {
	try {
		if(argc != 2) {
			std::cout << "Specify obj file!\n";
			return -1;
		}
		render_ssao(argv[1]);
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

void render_ssao(const char* filename) {
	glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
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
	
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tbo, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tbo, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tbo, 0);
	
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

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

	GLuint filter_fbo;
	glGenFramebuffers(1, &filter_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo);

	GLuint blurred_ssao_tbo;
	glGenTextures(1, &blurred_ssao_tbo);
	glBindTexture(GL_TEXTURE_2D, blurred_ssao_tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_resolution.x, screen_resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurred_ssao_tbo, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Filter Framebuffer not complete. Returning!\n";
	}

	ShaderWrapper g_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/g_shader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/g_shader.glsl.fs"))
	);

	ShaderWrapper ssao_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/ssao_shader.glsl.fs"))
	);

	ShaderWrapper blur_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/box_blur.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/box_blur.glsl.fs"))
	);

	ShaderWrapper quad_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/quadshader.glsl.fs"))
	);

	int attachment_ids[] = { 0, 1, 2 };

	quad_shader.bind();
	quad_shader.upload1iv(&attachment_ids[0], "tPosition");
	quad_shader.upload1iv(&attachment_ids[1], "tNormal");

	ssao_shader.bind();
	ssao_shader.upload1iv(&attachment_ids[0], "tPosition");
	ssao_shader.upload1iv(&attachment_ids[1], "tNormal");
	ssao_shader.upload1iv(&attachment_ids[2], "tNoise");

	blur_shader.bind();
	blur_shader.upload1iv(&attachment_ids[0], "tUnfiltered");

	glm::vec3 pos(0.f, 0.f, -1.f);
	glm::vec3 target(0.f);
	ViewTransform view(pos, target);
	glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1000.f);

	//
	// Loop invariant setup
	g_shader.bind();
	g_shader.upload44fm(glm::value_ptr(projection), "perspective_projection");
	ssao_shader.bind();
	ssao_shader.upload44fm(glm::value_ptr(projection), "proj");
	ssao_shader.upload2fv(glm::value_ptr(screen_resolution), "screen_resolution");

	float g_shader_time = 0.f;

	float delta_time = 0.f;
	float last_time = 0.f;

	int loop_iter = 0;
	constexpr int nLabels = 6;
	constexpr int expected_number_of_entries = 1000;
	std::vector<std::vector<float>> log_entries(nLabels);
	for(int i = 0; i < nLabels; ++i) {
		log_entries[i].resize(expected_number_of_entries);
	}

	LogFile logfile("logs/ssao_frametime.cvs");
	logfile.write_line("Frame_time Scene_Render SSAO_Pass Blur_Pass Quad_Pass Buffer_Swap");

	std::cout << std::flush;

	while(!glfwWindowShouldClose(window)) {
		delta_time = glfwGetTime() - last_time;
		last_time += delta_time;

		log_entries[0].push_back(delta_time * 1000.f);

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

		//
		// Render to framebuffer

		float start = glfwGetTime();
		g_shader.bind();
		g_shader.upload44fm(view.get_pointer(), "view_transform");
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);
		float end = glfwGetTime() - start;
		log_entries[1].push_back(end * 1000.f);

		//
		// SSAO shader
		start = glfwGetTime();
		ssao_shader.bind();
		glBindTextureUnit(0, position_tbo);
		glBindTextureUnit(1, normal_tbo);
		glBindTextureUnit(2, noise_tbo);
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, constant_ssbo);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		end = glfwGetTime() - start;
		log_entries[2].push_back(end * 1000.f);

		//
		// Blur SSAO output
		start = glfwGetTime();
		blur_shader.bind();
		glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo);
		glBindTextureUnit(0, ambient_tbo);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		end = glfwGetTime() - start;
		log_entries[3].push_back(end * 1000.f);
		
		//
		// Render final quad
		start = glfwGetTime();
		quad_shader.bind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTextureUnit(0, blurred_ssao_tbo);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		end = glfwGetTime() - start;
		log_entries[4].push_back(end * 1000.f);

		start = glfwGetTime();
		glfwSwapBuffers(window);
		glfwPollEvents();
		end = glfwGetTime() - start;
		log_entries[5].push_back(end * 1000.f);

		loop_iter++;
	}

	logfile.write_raw(reinterpret_cast<char*>(&loop_iter), sizeof(int));
	for(auto& log_entry : log_entries) {
		logfile.write_raw(reinterpret_cast<char*>(log_entry.data()), loop_iter * sizeof(float));
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &quad_vbo);
	glDeleteBuffers(1, &constant_ssbo);
	glDeleteTextures(1, &depth_tbo);
	glDeleteTextures(1, &noise_tbo);
	glDeleteTextures(1, &normal_tbo);
	glDeleteTextures(1, &ambient_tbo);
	glDeleteTextures(1, &position_tbo);
	glDeleteTextures(1, &blurred_ssao_tbo);
	glDeleteFramebuffers(1, &fbo);	
	glDeleteFramebuffers(1, &ssao_fbo);	
	glDeleteFramebuffers(1, &filter_fbo);
}

void performance_testbed(const char* filename) {
	glm::vec2 screen_resolution(1920, 1080);
	WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

	ShaderWrapper perf_shader(
		false,
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/perf.glsl.vs")),
		shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/ssao/perf.glsl.fs"))
	);
	perf_shader.bind();

	glm::vec3 pos(0.f, 0.f, -1.f);
	glm::vec3 target(0.f);
	ViewTransform view(pos, target);
	glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_resolution.x / screen_resolution.y, 0.1f, 1000.f);

	perf_shader.upload44fm(glm::value_ptr(projection), "proj");


	float delta_time = 0.f, last_time = 0.f;
	while(glfwWindowShouldClose(window)) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		delta_time = glfwGetTime() - last_time;
		last_time += delta_time;
		printf("frame time: %fms\n", delta_time * 1000.f);

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

		glDrawElements(GL_TRIANGLES, vertex_indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}