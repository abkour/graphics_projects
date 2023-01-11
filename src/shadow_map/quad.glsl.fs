#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec2 uv;

uniform sampler2D tex0;

void main() {
    FragmentColor = texture(tex0, uv);
}