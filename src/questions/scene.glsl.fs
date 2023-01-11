#version 450 core
layout(location = 0) out vec4 FragmentColor;

uniform vec2 screen_resolution;

uniform float far;
uniform float near;

float linear_depth(float depth) {
    float z = depth * 2.f - 1.f;
    return (2.f * near * far) / (far + near - z * (far - near));
}

void main() {
    vec3 depth = vec3(linear_depth(gl_FragCoord.z)) / far;
    FragmentColor = vec4(depth, 1.f);
}