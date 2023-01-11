#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec2 uv;

uniform float far;
uniform float near;
uniform sampler2D tex0;

void main() {
    float r = texture(tex0, uv).r;
    r = r * 2.f - 1.f;
    r = (2 * near * far) / (far + near - r * (far - near));
    FragmentColor = vec4(r, r, r, 1.f);
}