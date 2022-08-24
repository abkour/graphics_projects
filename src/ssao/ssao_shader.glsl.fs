#version 450 core 
layout(location = 0) in vec2 uv;
layout(location = 0) out float ambient_color;

layout(std430, binding = 0) buffer constant_objects 
{
    vec4 samples[64];
};

uniform sampler2D tPosition;
uniform sampler2D tNormal;
uniform sampler2D tNoise;

uniform mat4 proj;

uniform vec2 screen_resolution;
const vec2 noise_scale = screen_resolution / vec2(4.f);

const float hemisphere_radius = 0.25f;
const float bias = 0.025f;

void main() {
    vec3 fragment_position = texture(tPosition, uv).xyz;
    vec3 normal = normalize(texture(tNormal, uv).xyz);
    vec3 random_vec = normalize(texture(tNoise, uv * noise_scale).xyz);

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.f;
    for(int i = 0; i < 64; ++i) {
        vec3 sample_pos = tbn * samples[i].xyz;
        sample_pos = fragment_position + sample_pos * hemisphere_radius;

        vec4 offset = vec4(sample_pos, 1.f);
        offset = proj * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sample_depth = texture(tPosition, offset.xy).z;

        float range_check = smoothstep(0.f, 1.f, hemisphere_radius / abs(fragment_position.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.f : 0.f) * range_check;
    }

    occlusion = 1.f - (occlusion / 64);
    ambient_color = occlusion;
}