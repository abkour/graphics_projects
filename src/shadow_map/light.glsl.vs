#version 450 core
layout(location = 0) in vec3 aPos;

uniform mat4 lightMVP;

void main() {
    vec4 pos_vs = lightMVP * vec4(aPos, 1.f);
}