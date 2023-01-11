#include <algorithm>
#include <fstream>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>
#include <numeric>
#include <shaderdirect.hpp>
#include <string.h>
#include <vector>

#include "../common/font_engine.hpp"
#include "../common/window_manager.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << v.x << ", " << v.y;
}

void render_graph(const char* filename);

int main(int argc, const char** argv) {
	try {
		if(argc != 2) {
			std::cout << "Specify obj file!\n";
			return -1;
		}
		render_graph(argv[1]);
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

struct FileInformation {
    int nLabels;
    std::vector<std::string> labels;
    std::vector<std::vector<float>> data;
    std::size_t filesize;
};

FileInformation parse_data(const char* filename) {
    FileInformation f_info;
    std::ifstream ifs(filename, std::ios::binary);
    
    std::string labels;
    std::getline(ifs, labels);
    f_info.nLabels = std::count_if(labels.begin(), labels.end(), [](char ch){ return ch == ' '; }) + 1;

    std::size_t label_entry_size = 0;
    std::istringstream istr(labels);
    for(int i = 0; i < f_info.nLabels; ++i) {
        istr >> labels;
        f_info.labels.push_back(labels);
        label_entry_size += labels.size() + 1;
    }

    int nEntries = 0;
    ifs.seekg(label_entry_size, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(&nEntries), sizeof(int));
    ifs.seekg(sizeof(int), std::ios::cur);

    for(auto& label : f_info.labels) {
        std::cout << label << '\n';
    }

    f_info.data.resize(f_info.nLabels);
    for(int i = 0; i < f_info.nLabels; ++i) {
        f_info.data[i].resize(nEntries, 42.f);
    }

    for(int i = 0; i < f_info.nLabels; ++i) {
        ifs.read(reinterpret_cast<char*>(f_info.data[i].data()), sizeof(float) * nEntries);
        ifs.seekg(nEntries * sizeof(float), std::ios::cur);
    }

    ifs.close();
    return f_info;
}

void render_graph(const char* filename) {
	glm::vec2 screen_resolution(1280, 720);
	WindowManager window_manager(screen_resolution, WindowMode::Windowed);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);

    FileInformation fd = parse_data(filename);
    const std::size_t nEntries = fd.data[0].size();
    std::vector<float> indices(nEntries);
    std::iota(indices.begin(), indices.end(), 0);

    // Find the minimum and maximum value in the data
    float min_v = std::numeric_limits<float>::max();
    float max_v = std::numeric_limits<float>::min();
    for(const float v : fd.data[1]) {
        if(v < min_v) min_v = v;
        if(v > max_v) max_v = v;
    }
    glm::vec2 min_pair(0, min_v);
    glm::vec2 max_pair(fd.data[1].size() - 1, max_v);

    glm::vec2 origin(-0.75f, -0.75f);
    glm::vec2 range(1.5f, 1.5f);

    float step_x = range.x / 10.f;
    float step_y = range.y / 10.f;
    float off_x = step_x;
    float off_y = step_y;
    std::vector<float> minor_lines;

    // Axis lines
    minor_lines.push_back(origin.x);
    minor_lines.push_back(origin.y);
    minor_lines.push_back(origin.x + range.x);
    minor_lines.push_back(origin.y);

    minor_lines.push_back(origin.x);
    minor_lines.push_back(origin.y);
    minor_lines.push_back(origin.x);
    minor_lines.push_back(origin.y + range.y);

    // Horizontal lines
    for(int i = 0; i < 10; ++i) {
        minor_lines.push_back(origin.x);
        minor_lines.push_back(origin.y + off_y);
        minor_lines.push_back(origin.x + range.x);
        minor_lines.push_back(origin.y + off_y);
        off_y += step_y;
    }

    // Vertical lines
    for(int i = 0; i < 10; ++i) {
        minor_lines.push_back(origin.x + off_x);
        minor_lines.push_back(origin.y);
        minor_lines.push_back(origin.x + off_x);
        minor_lines.push_back(origin.y + range.y);
        off_x += step_x;
    }

    minor_lines.push_back(0.f);
    minor_lines.push_back(-1.f);
    minor_lines.push_back(0.f);
    minor_lines.push_back(1.f);

    GLuint graph_vao, graph_vbo;
    glCreateVertexArrays(1, &graph_vao);
    glCreateBuffers(1, &graph_vbo);
    glBindVertexArray(graph_vao);
    glBindBuffer(GL_ARRAY_BUFFER, graph_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nEntries * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nEntries * sizeof(float), indices.data());
    glBufferSubData(GL_ARRAY_BUFFER, nEntries * sizeof(float), nEntries * sizeof(float), fd.data[1].data());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), reinterpret_cast<void*>(nEntries * sizeof(float)));

    GLuint lines_vao, lines_vbo;
    glCreateVertexArrays(1, &lines_vao);
    glCreateBuffers(1, &lines_vbo);
    glBindVertexArray(lines_vao);
    glBindBuffer(GL_ARRAY_BUFFER, lines_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * minor_lines.size(), minor_lines.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    std::string font_path = ROOT_DIRECTORY + std::string("/fonts/consola.ttf");
    FontEngine font_engine(screen_resolution);
    font_engine.InitializeFace(font_path.c_str(), 24);
    font_engine.MakeAtlas(256, 256);

    const char* n_90 = "90"; const char* n_100 = "100";
    const char* xlabel = "Frame";
    const char* ylabel = "Frame time (milliseconds)";
    unsigned int xlabel_string = font_engine.MakeString(xlabel, glm::vec2(200, 900), glm::vec2(200, 0), glm::radians(0.f));
    unsigned int ylabel_string = font_engine.MakeString(ylabel, glm::vec2(200, 0), glm::vec2(200, 0), glm::radians(90.f));
    unsigned int n_90_string = font_engine.MakeString(n_90, glm::vec2(200, 0), glm::vec2(200, 0), glm::radians(0.f));
    unsigned int n_100_string = font_engine.MakeString(n_100, glm::vec2(200, 0), glm::vec2(200, 0), glm::radians(0.f));

    ShaderWrapper graph_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/graph_plotter/graph.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/graph_plotter/graph.glsl.fs"))
    );
    graph_shader.bind();
    graph_shader.upload2fv(&origin.x, "origin");
    graph_shader.upload2fv(&range.x, "range");
    graph_shader.upload2fv(&min_pair.x, "min_value");
    graph_shader.upload2fv(&max_pair.x, "max_value");

    ShaderWrapper lines_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/graph_plotter/lines.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/graph_plotter/lines.glsl.fs"))
    );

    glm::vec2 cursor_pos(0.f);

    float zoom = 1.f;
    const float zoom_factor = 0.08f;
    
    glm::vec3 minor_lines_color(0.6f);
    glm::vec3 axis_lines_color(1.f);
    glm::vec3 center_color(0.f, 1.f, 0.f);

    // EXPERIMENT
    GLuint result = 0;
    GLuint fquery;
    glGenQueries(1, &fquery);

    bool first_query_recall = true;

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(!first_query_recall) {
            glGetQueryObjectuiv(fquery, GL_QUERY_RESULT, &result);
            //std::cout << "result: " << static_cast<float>(result) / 100.f << "micros\n";
        } else {
            first_query_recall = false;
        }

        if(window_manager.get_lmb_pressed()) {
            cursor_pos += window_manager.get_mouse_delta() / screen_resolution;
            window_manager.reset_mouse_delta();
        } 

        zoom += window_manager.get_mouse_zoom() * zoom_factor;
        zoom = std::max(zoom, 1.f);
        window_manager.reset_mouse_zoom();

        
        lines_shader.bind();
        glBindVertexArray(lines_vao);
        lines_shader.upload3fv(&axis_lines_color.x, "color");
        glDrawArrays(GL_LINES, 0, 4);
        lines_shader.upload3fv(&minor_lines_color.x, "color");
        glDrawArrays(GL_LINES, 4, 40);
        lines_shader.upload3fv(&center_color.x, "color");
        glDrawArrays(GL_LINES, 44, 4);

        /*
        graph_shader.bind();
        graph_shader.upload1fv(&zoom, "zoom");
        graph_shader.upload2fv(&cursor_pos.x, "cursor_pos");
        glBindVertexArray(graph_vao);
        glBeginQuery(GL_TIME_ELAPSED, fquery);
        glDrawArrays(GL_LINE_STRIP, 0, fd.data[1].size());
        glEndQuery(GL_TIME_ELAPSED);
        */

        font_engine.Render(xlabel_string, glm::vec2(screen_resolution.x / 2.f, 50.f), true);
        font_engine.Render(ylabel_string, glm::vec2(250, 200), true);
        font_engine.Render(n_100_string, glm::vec2(110, 620));
        font_engine.Render(n_90_string, glm::vec2(125, 570));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &lines_vao);
    glDeleteVertexArrays(1, &graph_vao);
    glDeleteBuffers(1, &lines_vbo);
    glDeleteBuffers(1, &graph_vbo);
    glDeleteQueries(1, &fquery);
}