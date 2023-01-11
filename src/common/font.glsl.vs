#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform vec2 offset;

uniform mat4 model;
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    vec4 adj_pos = model * vec4(aPos, 0.f, 0.f);
    gl_Position = projection * vec4(adj_pos.xy + offset, 0.f, 1.f);
    TexCoord = aTexCoord;
}