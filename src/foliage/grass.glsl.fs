#version 450 core 
layout(location = 0) out vec4 FragmentColor;

in vec2 uv;

uniform sampler2D grass_texture;

void main() {
    FragmentColor = texture(grass_texture, uv);
}