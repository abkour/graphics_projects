cmake_minimum_required(VERSION 3.13)

project(graphics_projects)

add_executable( ssao 
                src/ssao/main.cpp 
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)

include_directories("ext//glad//include//")
include_directories("ext//glfw//include//GLFW//")
include_directories("ext//shaderdirect//")
include_directories("ext//glm//glm//")

target_link_libraries(ssao debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(ssao optimized "ext//glfw//src//Release//glfw3")
target_sources(ssao PUBLIC "ext/glad/src/glad.c")