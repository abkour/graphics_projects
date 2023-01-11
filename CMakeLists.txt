cmake_minimum_required(VERSION 3.13)

project(graphics_projects)

add_executable( api 
                src/api/main.cpp 
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( cloth 
                src/cloth/main.cpp 
                src/cloth/simulation.cpp
                src/cloth/simulation_grid.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( ssao 
                src/ssao/main.cpp 
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( circle
                src/test_environment/circle.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( raymarching
                src/raymarching/main
                src/common/model_loader.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( volume_light
                src/volumetric_lighting/main.cpp
                src/common/model_loader.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( foliage
                src/foliage/grass.cpp
                src/common/model_loader.cpp
                src/common/perlin.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( geometry_shader
                src/geometry_shader/main.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( terrain_tessellation
                src/tessellation_shader/main.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( curved_surface
                src/curved_surface/main.cpp
                src/common/view.cpp
                src/common/window_manager.cpp)

add_executable( projectiles
                src/projectiles/main.cpp
                src/common/window_manager.cpp)

add_executable( graph_plotter
                src/graph_plotter/main.cpp
                src/common/font_engine.cpp
                src/common/window_manager.cpp)

add_executable( point_lights
                src/point_lights/main.cpp
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( ssr
                src/ssr/main.cpp
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( mirror
                src/mirror/main.cpp
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( shadow_map
                src/shadow_map/main.cpp
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

add_executable( questions
                src/questions/main.cpp
                src/common/logfile.cpp
                src/common/view.cpp
                src/common/window_manager.cpp
                src/common/model_loader.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(ext/freetype-2.11.0)
add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)

include_directories("ext//glad//include//")
include_directories("ext//glfw//include//GLFW//")
include_directories("ext//shaderdirect//")
include_directories("ext//stb//")
include_directories("ext//glm//glm//")
include_directories("ext//freetype-2.11.0//include//")

target_link_libraries(api debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(api optimized "ext//glfw//src//Release//glfw3")
target_sources(api PUBLIC "ext/glad/src/glad.c")

target_link_libraries(cloth debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(cloth optimized "ext//glfw//src//Release//glfw3")
target_sources(cloth PUBLIC "ext/glad/src/glad.c")

target_link_libraries(ssao debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(ssao optimized "ext//glfw//src//Release//glfw3")
target_sources(ssao PUBLIC "ext/glad/src/glad.c")

target_link_libraries(circle debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(circle optimized "ext//glfw//src//Release//glfw3")
target_sources(circle PUBLIC "ext/glad/src/glad.c")

target_link_libraries(raymarching debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(raymarching optimized "ext//glfw//src//Release//glfw3")
target_sources(raymarching PUBLIC "ext/glad/src/glad.c")

target_link_libraries(volume_light debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(volume_light optimized "ext//glfw//src//Release//glfw3")
target_sources(volume_light PUBLIC "ext/glad/src/glad.c")

target_link_libraries(foliage debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(foliage optimized "ext//glfw//src//Release//glfw3")
target_sources(foliage PUBLIC "ext/glad/src/glad.c")

target_link_libraries(geometry_shader debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(geometry_shader optimized "ext//glfw//src//Release//glfw3")
target_sources(geometry_shader PUBLIC "ext/glad/src/glad.c")

target_link_libraries(terrain_tessellation debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(terrain_tessellation optimized "ext//glfw//src//Release//glfw3")
target_sources(terrain_tessellation PUBLIC "ext/glad/src/glad.c")

target_link_libraries(curved_surface debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(curved_surface optimized "ext//glfw//src//Release//glfw3")
target_sources(curved_surface PUBLIC "ext/glad/src/glad.c")

target_link_libraries(projectiles debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(projectiles optimized "ext//glfw//src//Release//glfw3")
target_sources(projectiles PUBLIC "ext/glad/src/glad.c")

target_link_libraries(graph_plotter debug "ext//freetype-2.11.0//build//Debug//freetyped")
target_link_libraries(graph_plotter optimized "ext//freetype-2.11.0//Release//freetype")
target_link_libraries(graph_plotter debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(graph_plotter optimized "ext//glfw//src//Release//glfw3")
target_sources(graph_plotter PUBLIC "ext/glad/src/glad.c")

target_link_libraries(point_lights debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(point_lights optimized "ext//glfw//src//Release//glfw3")
target_sources(point_lights PUBLIC "ext/glad/src/glad.c")

target_link_libraries(ssr debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(ssr optimized "ext//glfw//src//Release//glfw3")
target_sources(ssr PUBLIC "ext/glad/src/glad.c")

target_link_libraries(mirror debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(mirror optimized "ext//glfw//src//Release//glfw3")
target_sources(mirror PUBLIC "ext/glad/src/glad.c")

target_link_libraries(shadow_map debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(shadow_map optimized "ext//glfw//src//Release//glfw3")
target_sources(shadow_map PUBLIC "ext/glad/src/glad.c")

target_link_libraries(questions debug "ext//glfw//src//Debug//glfw3")
target_link_libraries(questions optimized "ext//glfw//src//Release//glfw3")
target_sources(questions PUBLIC "ext/glad/src/glad.c")

add_compile_definitions(ROOT_DIRECTORY="${CMAKE_SOURCE_DIR}")