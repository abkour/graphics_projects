#include <iostream>
#include <shaderdirect.hpp>

#include "../common/model.hpp"
#include "../common/view.hpp"
#include "../common/window_manager.hpp"

int main() {
    try {
        glm::vec2 screen_resolution(1920, 1080);
        WindowManager window_manager(screen_resolution, WindowMode::Fullscreen);
        GLFWwindow* window = window_manager.get_window_pointer();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
        while(!glfwWindowShouldClose(window)) {
            glClearColor(0.3f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch(std::runtime_error& e) {
        std::cout << e.what() << '\n';
    } catch(...) {
        std::cerr << "Unknown error!\n";
    }
}