#version 450 core
layout(location = 0) in vec2 aPos;

uniform float zoom;
uniform vec2 cursor_pos;

uniform vec2 origin;
uniform vec2 range;

void main() {
    vec2 xy_coord = (cursor_pos + aPos) * max(0.f, zoom);
    gl_Position = vec4(xy_coord, 0.f, 1.f);
}