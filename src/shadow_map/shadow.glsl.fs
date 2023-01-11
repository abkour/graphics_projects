#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec3 color;

uniform vec2 screen_resolution;
uniform sampler2D shadow_texture;

void main() {
    vec2 uv = gl_FragCoord.xy / screen_resolution;
    float r = texture(shadow_texture, uv).r;
    FragmentColor = vec4(vec3(r), 1.f);
}