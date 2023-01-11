// Standard library dependencies
#include <chrono>
#include <iostream>

// Third party External dependencies
#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

// Application headers
#include "../common/histogram.hpp"
#include "../common/logfile.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

#include "simulation.hpp"

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
    return os << v.x << ", " << v.y;
} 
 
void cloth_simulation();

// Call with cornell-refl.scene
int main(int argc) {
	try {
		cloth_simulation();
	} catch(std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::flush;
	} catch(std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::flush;
    } catch(...) {
		std::cerr << "Unexpected error somewhere!" << std::flush;
	}
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << v.x << ", " << v.y << ", " << v.z;
}

void cloth_simulation() {
    glm::vec2 screen_resolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	WindowManager window_manager(screen_resolution, WindowMode::Windowed);
	GLFWwindow* window = window_manager.get_window_pointer();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSwapInterval(0);

    Histogram histogram(5'000);

    ClothSimulation cloth_simulation(GRID_WIDTH, GRID_HEIGHT);
    cloth_simulation.setup();

    float last_time = 0.f;
    float delta_time = 0.f;
    bool initial = true;
    while(!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 1.f, 1.0f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        delta_time = glfwGetTime() - last_time;
        last_time += delta_time;

        histogram.supply_frametime_ms(delta_time);

        auto start = std::chrono::high_resolution_clock::now();        
        cloth_simulation.tick(window_manager, delta_time);
        auto end = std::chrono::high_resolution_clock::now();

        float simulation_time = std::chrono::duration<double>(end - start).count();
        histogram.supply_frametime_ms(simulation_time);

        cloth_simulation.render();

        // Reset delta for the next frame.
        window_manager.reset_mouse_delta();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    auto hist_rec = histogram.get_histogram_record();
    std::cout << hist_rec;
}