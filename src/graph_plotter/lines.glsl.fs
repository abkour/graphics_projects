#version 450 core 
layout(location = 0) out vec4 FragmentColor;

uniform vec3 color;

void main() {
    FragmentColor = vec4(color, 1.0f);
}