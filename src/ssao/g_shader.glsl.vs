#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 0) out vec3 viewPosition;
layout(location = 1) out vec3 normal;

uniform mat4 perspective_projection;
uniform mat4 view_transform;

void main() {
    vec4 viewPos = view_transform * vec4(aPos, 1.f);
    viewPosition = viewPos.xyz;

    mat3 normal_transform = transpose(inverse(mat3(view_transform)));
    normal = normal_transform * aNormal;

    gl_Position = perspective_projection * viewPos;
}