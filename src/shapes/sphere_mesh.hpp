#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include <glad/glad.h>
#include <glm.hpp>
#include <shaderdirect.hpp>

#include <memory>

class SphereMesh {

public:

    SphereMesh() = default;
    
    SphereMesh(const glm::vec3& center, const glm::vec3& normal, const float radius);
    
    // I'm going to delete the copy constructor and copy assignment operator for now.
    SphereMesh(const SphereMesh& other) = delete;
    SphereMesh& operator=(const SphereMesh& other) = delete;

    ~SphereMesh();

    void render();

private:

    float radius;
    glm::vec3 center, normal;
    ShaderWrapper shader;

    GLuint vao, vbo;
};

#endif