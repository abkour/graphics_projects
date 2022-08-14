#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 0) out vec3 normal;

uniform mat4 perspective_projection;
uniform mat4 view_transform;

void main() {
    normal = aNormal;
    gl_Position = perspective_projection * view_transform * vec4(aPos, 1.f);
}