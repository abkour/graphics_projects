#version 450 core
layout(location = 0) out vec4 FragmentColor;

in vec3 oWorldPosition;
in vec3 oNormal;

struct PointLight {
    float intensity;
    vec3 position;
    vec3 color;
    // We choose vec3 to give the developer more freedom in how the light is attenuated.
    // X = constant factor, Y = linear factor, Z = exponential factor
    vec3 attenuation;
    // To align to the nearest 16-byte boundary
    vec2 padding;
};

uniform PointLight pointLight;
uniform vec3 lightPosition;
uniform vec3 lightDirection;

float attenuate(float distance, PointLight light) {
    float attenuation_factor = light.attenuation.x 
        + light.attenuation.y * distance 
        + light.attenuation.z * distance * distance;
    
    return light.intensity / attenuation_factor;
}

void main() {
    vec3 normal = normalize(oNormal);
    vec3 light_direction = pointLight.position - oWorldPosition;
    float light_distance = length(light_direction);
    // Normalize light direction
    light_direction = light_direction / light_distance;
    float attenuation = attenuate(light_distance, pointLight);
    float diffuse_factor = max(dot(normal, light_direction), 0.f);

    FragmentColor = vec4(pointLight.color * diffuse_factor * attenuation, 1.f);
}