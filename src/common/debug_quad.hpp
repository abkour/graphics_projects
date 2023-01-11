#ifndef _DEBUG_QUAD_HPP_
#define _DEBUG_QUAD_HPP_

#include <glad/glad.h>
#include <shaderdirect.hpp>

struct DebugQuad {

    DebugQuad();
    ~DebugQuad();

    DebugQuad(const DebugQuad& other) = delete;
    DebugQuad(DebugQuad&& other) = delete;

    DebugQuad& operator=(const DebugQuad& other) = delete;
    DebugQuad& operator=(DebugQuad&& other) = delete;

    GLuint vao, vbo;
    ShaderWrapper shader;
};

DebugQuad::DebugQuad() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const float quad_attributes[] = 
    {
        -1.f, -1.f, 0.f, 0.f,
        1.f, -1.f, 1.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        -1.f, -1.f, 0.f, 0.f,
        1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 1.f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_attributes), quad_attributes, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    ShaderWrapper tmp_shader(
        false,
        shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/common/quad.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/common/quad.glsl.fs"))
    );

    shader = std::move(tmp_shader);
}

DebugQuad::~DebugQuad() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}


#endif