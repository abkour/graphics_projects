#include "perlin.hpp"

#include <glad/glad.h>
#include <iostream>
#include <random>
#include <shaderdirect.hpp>

glm::vec2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> w-s;
    b *= 1911520717; a ^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    glm::vec2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    glm::vec2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

float perlin(float x, float y) {
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = glm::mix(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = glm::mix(n0, n1, sx);

    value = glm::mix(ix0, ix1, sy);
    return value;
}

void perlin_noise_2d(   std::vector<float>& output, 
                        const glm::vec2 grid_dimensions, 
                        const glm::vec2 grid_vertices)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-1.f, 1.f);

    std::vector<glm::vec2> gradients_buffer((grid_vertices.x + 1) * (grid_vertices.y + 1));
    for(int i = 0; i < gradients_buffer.size(); ++i) {
        gradients_buffer[i] = glm::normalize(glm::vec2(dist(mt), dist(mt)));
    }

    float sx = 0.f;
    float sy = 0.f;
    float step_x = grid_vertices.x / grid_dimensions.x;
    float step_y = grid_vertices.y / grid_dimensions.y;
    for(int i = 0; i < grid_dimensions.x; ++i) {
        for(int j = 0; j < grid_dimensions.y; ++j) {
            int idx = i + j * grid_dimensions.x;
            output[idx] = perlin(sx, sy) * 0.5 + 0.5;
            sx += step_x;
        }
        sx = 0.f;
        sy += step_y;
    }

    std::cout << "sx: " << sx << '\n';
    std::cout << "sy: " << sy << '\n';

    return;

    GLuint gradients;
    glCreateBuffers(1, &gradients);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gradients);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * gradients_buffer.size(), gradients_buffer.data(), GL_STATIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gradients);

    GLuint perlin_noise;
    glCreateBuffers(1, &perlin_noise);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, perlin_noise);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * output.size(), NULL, GL_STATIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, perlin_noise);

    const uint32_t nShaderInvocations = output.size();
    const uint32_t nDispatches = std::ceil((float)nShaderInvocations / 256.f);

    ShaderWrapper perlin_noise_shader(
        false,
        shader_p(GL_COMPUTE_SHADER, ROOT_DIRECTORY + std::string("/src/common/perlin_noise_2d.glsl.comp"))
    );
    perlin_noise_shader.bind();
    perlin_noise_shader.upload1uiv(const_cast<uint32_t*>(&nShaderInvocations), "nGlobalShaderInvocations");
    perlin_noise_shader.upload2fv((float*)const_cast<glm::vec2*>(&grid_dimensions), "texture_dim");
    perlin_noise_shader.upload2fv((float*)const_cast<glm::vec2*>(&grid_vertices), "grid_dim");

    glDispatchCompute(nDispatches, 1, 1);
    std::cout << "nShaderInvocations: " << nShaderInvocations << '\n';
    std::cout << "nDispatches: " << nDispatches << '\n';
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, output.size() * sizeof(float), output.data());
}
