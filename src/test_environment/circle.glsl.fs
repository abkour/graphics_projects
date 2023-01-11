#version 450 core 
layout(location = 0) out vec4 FragmentColor;

uniform vec2 sres;

const float radius = 0.3f;

void main() {
    const float aspect_ratio = sres.x / sres.y;
    vec2 uv = gl_FragCoord.xy / sres;
    uv = uv * 2.f - 1.f;
    uv.x *= aspect_ratio;

    float length = sqrt(uv.x * uv.x + uv.y * uv.y);
    if(length < radius) {
       FragmentColor = vec4(1.f, 0.5f, 0.2f, 1.f);
    } else {
        FragmentColor = vec4(0.f);
    } 

}