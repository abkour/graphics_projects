#version 460 core 
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D tPosition;
uniform sampler2D tNormal;

uniform mat4 view;
uniform mat4 proj;

void main() {
    FragmentColor = abs(texture(tNormal, uv));
}