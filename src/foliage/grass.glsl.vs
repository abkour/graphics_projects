#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(std430, binding = 0) buffer buf0 
{
    vec4 offset_buffer[];
};

out vec2 uv;

uniform vec2 offsets;

uniform mat4 proj;
uniform mat4 view;

void main() {
    uv = aUV;
    gl_Position = proj * view * vec4(offset_buffer[gl_InstanceID].xyz + aPos, 1.f);
}