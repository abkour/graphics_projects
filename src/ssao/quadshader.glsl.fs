#version 450 core 
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D tAmbient;

void main() {
    float c = texture(tAmbient, uv).x;
    FragmentColor = vec4(c, c, c, 1.f);
}