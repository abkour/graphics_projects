#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec2 uv;

uniform sampler2D frame_texture;

void main() {
    FragmentColor = texture(frame_texture, uv);
}