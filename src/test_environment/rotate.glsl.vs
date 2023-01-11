#version 450 core 
layout(location = 0) in vec2 aPos;

uniform mat4 model;

void main() {
    gl_Position = model * vec4(aPos, 0.f, 1.f);
}