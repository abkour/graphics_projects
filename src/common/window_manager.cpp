#include "window_manager.hpp"

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

WindowManager::WindowManager(const glm::vec2& screen_resolution) {
    if (!glfwInit()) {
        return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

    window = glfwCreateWindow(  screen_resolution.x, 
                                screen_resolution.y, 
                                "ssao implementation", 
                                NULL, 
                                NULL);

	if (!window) {
        window = nullptr;
        return;
	}

	glfwMakeContextCurrent(window);
	if (glfwGetError(NULL)) {
        window = nullptr;
        return;
	}
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        window = nullptr;
        return;
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
