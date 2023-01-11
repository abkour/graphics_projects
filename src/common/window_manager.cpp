#include "window_manager.hpp"
#include <stdexcept>

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam );

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

WindowManager::WindowManager(
                const glm::vec2& screen_resolution, 
                const WindowMode& window_mode,
                bool DEBUG_MODE)
{
    this->screen_resolution = screen_resolution;
    
    if (!glfwInit()) {
        throw std::runtime_error("[Class::WindowManager] GLFW could not be initialized!");
	}

    if(DEBUG_MODE) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

    if(DEBUG_MODE) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    }
    
    cursor = std::make_unique<Cursor>(screen_resolution.x / 2, screen_resolution.y / 2);
    glfwSetWindowUserPointer(window, cursor.get());

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
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
        : lmb_pressed(false)
        , rmb_pressed(false)
        , zoom(0.f)
        , pos(xpos, ypos)
        , delta(0.f, 0.f)
        , initialEntry(true)
    {}

    bool lmb_pressed, rmb_pressed;

    float zoom;
    glm::vec2 pos;
    glm::vec2 delta;
    bool initialEntry;
};


void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    char* source_str = NULL;
    switch(source) {
    case GL_DEBUG_SOURCE_API:
        source_str = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_str = "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_str = "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_str = "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "OTHER";
        break;
    default:
        break;
    }

    char* type_str = NULL;
    switch(type) {
    case GL_DEBUG_TYPE_ERROR:
        type_str = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_str = "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_str = "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_str = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "OTHER";
        break;
    default:
        break;
    }

    char* severity_str = NULL;
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_str = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_str = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_str = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_str = "NOTIFICATION";
        break;
    default:
        break;
    }

  fprintf( stderr, "GL CALLBACK: %s source = %s, type = %s, severity = %s, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            source_str, type_str, severity_str, message );
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Cursor* cursor = 
        reinterpret_cast<Cursor*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cursor->lmb_pressed = true;
    } else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        cursor->lmb_pressed = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        cursor->rmb_pressed = true;
    } else if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        cursor->rmb_pressed = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Cursor* cursor = 
        reinterpret_cast<Cursor*>(glfwGetWindowUserPointer(window));
    
    cursor->zoom = yoffset;
}

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

float WindowManager::get_mouse_zoom() const {
    return cursor->zoom;
}

glm::vec2 WindowManager::get_mouse_delta() const {
    return cursor->delta;
}

glm::vec2 WindowManager::get_mouse_position() const {
    return cursor->pos;
}

bool WindowManager::get_lmb_pressed() const {
    return cursor->lmb_pressed;
}

bool WindowManager::get_rmb_pressed() const {
    return cursor->rmb_pressed;
}

void WindowManager::reset_mouse_zoom() {
    cursor->zoom = 0.f;
}

void WindowManager::reset_mouse_delta() { 
    cursor->delta = glm::vec2(0.f);
}

void WindowManager::set_title(const char* title) {
    glfwSetWindowTitle(window, title);
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
