#version 450 core
layout(location = 0) out vec4 FragmentColor;

in float height;

void main() {
    float h = (height + 16.f) / 64.f;
    FragmentColor = vec4(h, h, h, 1.f);
}