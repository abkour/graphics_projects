#ifndef _WINDOW_MANAGER_HPP_
#define _WINDOW_MANAGER_HPP_

#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>

#include <memory>

#include "movement_direction.hpp"

class Cursor;

class WindowManager {

public:

    WindowManager(const glm::vec2& screen_resolution);
    ~WindowManager();

    GLFWwindow* get_window_pointer() {
        return window;
    }

    glm::vec2 get_mouse_delta() const;
    glm::vec2 get_mouse_position() const;

    MovementDirection get_movement_direction() const;

    void reset_mouse_delta();

private:

    GLFWwindow* window = nullptr;
    std::unique_ptr<Cursor> cursor;
};

#endif 