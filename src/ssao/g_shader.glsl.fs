#version 460 core 
layout(location = 0) in vec3 viewPosition;
layout(location = 1) in vec3 normal;
layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;

void main() {
    Position = viewPosition;
    Normal = normalize(normal);
}