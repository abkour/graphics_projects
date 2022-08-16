#version 460 core 
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D frame_texture;

void main() {
    FragmentColor = vec4(1.f) - texture(frame_texture, uv);
}