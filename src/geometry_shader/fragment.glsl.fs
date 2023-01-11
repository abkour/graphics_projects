#version 450 core 
layout(location = 0) out vec4 FragmentColor;

in vec3 fColor;

void main() {
    FragmentColor = vec4(fColor, 1.f);
}