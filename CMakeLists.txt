cmake_minimum_required(VERSION 3.2)

project(graphics_projects)

add_executable( ssao 
                src/ssao/main.cpp 
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("C://users//flora//ext//glad//include//")
include_directories("C://users//flora//ext//glfw-3.3.8//include//GLFW//")
include_directories("C://users//flora//dev//glShader//")
include_directories("C://users//flora//ext//glm//glm//")

target_link_libraries(ssao debug "C://Users//flora//ext//glfw-3.3.8//build//src//Debug//glfw3.lib")
target_link_libraries(ssao optimized "C://Users//flora//ext//glfw-3.3.8//build//src//Release//glfw3.lib")
target_sources(ssao PUBLIC "C://Users//flora//ext//glad//src//glad.c")