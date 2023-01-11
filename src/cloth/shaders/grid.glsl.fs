#version 450 core
layout(location = 0) out vec4 FragmentColor;

const vec2 screen_resolution = vec2(1080, 720);

void main() {
    vec2 uv = gl_FragCoord.xy / screen_resolution;
    FragmentColor = vec4(uv, 1.f, 1.f);
}