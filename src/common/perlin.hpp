#ifndef _PERLIN_HPP_
#define _PERLIN_HPP_

#include <glm.hpp>
#include <vector>

void perlin_noise_2d(std::vector<float>& output, const glm::vec2 grid_dimensions, const glm::vec2 grid_vertices);

#endif