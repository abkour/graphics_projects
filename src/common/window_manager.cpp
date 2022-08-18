#include "window_manager.hpp"
#include <stdexcept>

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

WindowManager::WindowManager(const glm::vec2& screen_resolution, const WindowMode& window_mode) {
    if (!glfwInit()) {
        throw std::runtime_error("[Class::WindowManager] GLFW could not be initialized!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    if(!primary_monitor) {
        throw std::runtime_error("[Class::WindowManager] Couldn't identify primary monitor!");
    }

    GLFWvidmode* video_mode = const_cast<GLFWvidmode*>(glfwGetVideoMode(primary_monitor)); 
    if(!video_mode) {
        throw std::runtime_error("[Class::WindowManager] Couldn't identify video mode of primary monitor!");
    }

    switch(window_mode) {
    case WindowMode::Windowed:
        window = glfwCreateWindow(screen_resolution.x, screen_resolution.y, "Title", NULL, NULL);
        break;
    case WindowMode::Borderless:
        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
        window = glfwCreateWindow(screen_resolution.x, screen_resolution.y, "Title", primary_monitor, NULL);
        break;
    case WindowMode::Fullscreen:
        window = glfwCreateWindow(screen_resolution.x, screen_resolution.y, "Title", primary_monitor, NULL);
        break;
    default:
        throw std::runtime_error("[Class::WindowManager] No window mode specified!");
        break;
    }

	if (!window) {
        throw std::runtime_error("[Class::WindowManager] Window could not be created!");
	}

	glfwMakeContextCurrent(window);
	if (glfwGetError(NULL)) {
        throw std::runtime_error("[Class::WindowManager] Window could not be made current!");
	}
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("[Class::WindowManager] GLAD could not be loaded!");
	}

    cursor = std::make_unique<Cursor>(screen_resolution.x / 2, screen_resolution.y / 2);
    glfwSetWindowUserPointer(window, cursor.get());

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
}

WindowManager::~WindowManager() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Cursor definition
class Cursor {

public:

    Cursor(float xpos, float ypos) 
        : pos(xpos, ypos)
        , delta(0.f, 0.f)
        , initialEntry(true)
    {}

    glm::vec2 pos;
    glm::vec2 delta;
    bool initialEntry;
};

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Cursor* cursor = 
        reinterpret_cast<Cursor*>(glfwGetWindowUserPointer(window));

    if(cursor->initialEntry) {
        cursor->pos.x = xpos;
        cursor->pos.y = ypos;
        cursor->initialEntry = false;
    }

    cursor->delta.x = xpos - cursor->pos.x;
    cursor->delta.y = cursor->pos.y - ypos;
    cursor->pos.x = xpos;
    cursor->pos.y = ypos;
}

glm::vec2 WindowManager::get_mouse_delta() const {
    return cursor->delta;
}

glm::vec2 WindowManager::get_mouse_position() const {
    return cursor->pos;
}

void WindowManager::reset_mouse_delta() { 
    cursor->delta = glm::vec2(0.f);
}

// Prototype definitions
MovementDirection WindowManager::get_movement_direction() const {
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        return MovementDirection::Forward;
    } else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        return MovementDirection::Backward;
    } else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        return MovementDirection::Left;
    } else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        return MovementDirection::Right;
    }
    return MovementDirection::None;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
