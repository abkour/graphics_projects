#version 450 core 
layout(location = 0) out vec3 oColor;
layout(location = 1) out vec3 oNormal;
layout(location = 2) out float oDepth;
layout(location = 3) out float oReflMask;

uniform float far_clip_distance;

in vec3 positionVS;
in vec3 normal;
in vec3 color;
flat in uint bRefl;

void main() { 
    if(bRefl == 0) {
        oReflMask = 0;
    } else {
        oReflMask = 1;
    }
    oColor = color;
    oNormal = normalize(normal);
    oDepth = positionVS.z;
}