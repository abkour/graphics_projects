#version 450 core 
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in uint aRefl;

out vec3 positionVS;
out vec3 normal;
out vec3 color;
flat out uint bRefl;

uniform mat4 proj;
uniform mat4 view;

void main() {
    normal = aNormal;
    color = aColor;
    bRefl = aRefl;
    positionVS = vec3(view * vec4(aPos, 1.f));
    gl_Position = proj * view * vec4(aPos, 1.f);
}