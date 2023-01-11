#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

uniform mat4 proj;
uniform mat4 view;

out vec3 color;

void main() {
    color = aColor;
    gl_Position = proj * view * vec4(aPos, 1.f);
}