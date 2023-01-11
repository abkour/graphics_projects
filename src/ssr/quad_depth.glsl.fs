#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec2 uv;

uniform sampler2D tex0;

void main() {
    float r = texture(tex0, uv).r;
    FragmentColor = vec4(r, r, r, 1.f);
}