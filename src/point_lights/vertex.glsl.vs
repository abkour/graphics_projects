#version 450 core 
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 oWorldPosition;
out vec3 oNormal;

uniform mat4 projection;
uniform mat4 view;

void main() {
    oWorldPosition = aPos;
    oNormal = aNormal;
    gl_Position = projection * view * vec4(aPos, 1.f);
}