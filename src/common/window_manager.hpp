#ifndef _WINDOW_MANAGER_HPP_
#define _WINDOW_MANAGER_HPP_

#include <glad/glad.h>
#include <glfw3.h>
#include <glm.hpp>

#include <memory>

#include "movement_direction.hpp"

enum class WindowMode : uint8_t {
    None,
    Windowed, Borderless, Fullscreen
};

class Cursor;

class WindowManager {

public:

    WindowManager(const glm::vec2& screen_resolution, const WindowMode& window_mode, bool DEBUG_MODE = false);
    ~WindowManager();

    GLFWwindow* get_window_pointer() {
        return window;
    }

    bool get_lmb_pressed() const;
    bool get_rmb_pressed() const;

    float get_mouse_zoom() const;
    glm::vec2 get_mouse_delta() const;
    glm::vec2 get_mouse_position() const;

    MovementDirection get_movement_direction() const;

    glm::vec2 const get_window_resolution() const {
        return screen_resolution;
    }

    void reset_mouse_delta();
    void reset_mouse_zoom();

    void set_title(const char* title);

private:

    GLFWwindow* window = nullptr;
    std::unique_ptr<Cursor> cursor;
    glm::vec2 screen_resolution;
};

#endif 