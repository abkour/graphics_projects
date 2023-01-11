#version 450 core 
layout(location = 0) out vec4 FragmentColor;

uniform sampler2D FontTexture;

in vec2 TexCoord;

void main() {
    FragmentColor = vec4(1.f, 1.f, 1.f, texture(FontTexture, TexCoord).r);
}