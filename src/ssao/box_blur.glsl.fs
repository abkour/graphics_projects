#version 450 core
layout(location = 0) in vec2 uv;
layout(location = 0) out float filtered_ssao;

uniform sampler2D tUnfiltered;

void main() {
    float result = 0.f;
    vec2 texel_size = 1.f / vec2(textureSize(tUnfiltered, 0));
    for(int x = -2; x < 2; ++x) {
        for(int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(tUnfiltered, uv + offset).r;
        }
    }
    filtered_ssao = result / 16.f;
}