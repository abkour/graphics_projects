#include "sphere_mesh.hpp"

#include <gtc/type_ptr.hpp>
#include <shaderdirect.hpp>


SphereMesh::SphereMesh(const glm::vec3& center, const glm::vec3& normal, const float radius)
    : center(center)
    , normal(normal)
    , radius(radius)
{
    const float interleaved_attributes[] = 
    {
        -1.f, -1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,

        -1.f, -1.f, 0.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 1.f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved_attributes), interleaved_attributes, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(sizeof(float) * 2));

    ShaderWrapper shader_tmp(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/shapes/sphere_mesh.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/shapes/sphere_mesh.glsl.fs"))
    );
    shader = std::move(shader_tmp);
}

SphereMesh::~SphereMesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void SphereMesh::render() {
    shader.bind();
    shader.upload3fv(glm::value_ptr(center), "center");
    shader.upload3fv(glm::value_ptr(normal), "normal");
    shader.upload1fv(&radius, "radius");
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}