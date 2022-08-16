#version 460 core 
layout(location = 0) in vec3 normal;
layout(location = 0) out vec4 FragmentColor;

void main() {
    FragmentColor = vec4(abs(normal), 1.f);
}