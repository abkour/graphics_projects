#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 lightMVP;

void main() {
    gl_Position = lightMVP * vec4(aPos, 1.f);
}