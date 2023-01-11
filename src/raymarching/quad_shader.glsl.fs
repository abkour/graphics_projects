#version 450 core 
layout(location = 0) out vec4 FragmentColor;

uniform float time;

uniform vec2 sres;

float distance_from_sphere(in vec3 p, in vec3 c, in float r) {
    return length(p - c) - r;
}

float map_the_world(in vec3 p) {
    float displacement = sin(5.f * p.x) * sin(5.f * p.y) * sin(5.f * p.z) * 0.2f * time;
    float sphere_0 = distance_from_sphere(p, vec3(-2.f, 0.f, 0.f), 1.f);
    float sphere_1 = distance_from_sphere(p, vec3(2.f, 0.f, 0.f), 1.f);
    return min(sphere_0, sphere_1) + displacement;
}

vec3 calculate_normal(in vec3 p) {
    const vec3 small_step = vec3(0.001, 0.f, 0.f);

    float gradient_x = map_the_world(p + small_step.xyy) - map_the_world(p - small_step.xyy);
    float gradient_y = map_the_world(p + small_step.yxy) - map_the_world(p - small_step.yxy);
    float gradient_z = map_the_world(p + small_step.yyx) - map_the_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);
    
    return normalize(normal);
}

vec3 ray_march(in vec3 ro, in vec3 rd) {
    float total_distance_traveled = 0.f;
    const int number_of_steps = 32;
    const float minimum_hit_distance = 0.001f;
    const float maximum_hit_distance = 1000.f;

    for(int i = 0; i < number_of_steps; ++i) {
        vec3 current_position = ro + total_distance_traveled * rd;

        float distance_to_closest = map_the_world(current_position);

        if(distance_to_closest < minimum_hit_distance) {
            vec3 normal = calculate_normal(current_position);
            
            vec3 light_position = vec3(2.f, -5.f, 5.f);

            vec3 direction_to_light = normalize(current_position - light_position);

            float diffuse_intensity = max(0.f, dot(normal, direction_to_light));
            
            return vec3(1.f, 0.f, 0.f) * diffuse_intensity;
        }

        if(distance_to_closest > maximum_hit_distance) {
            break;
        }

        total_distance_traveled += distance_to_closest;
    }
    
    return vec3(0.f);
}

void main() {
    const float aspect_ratio = sres.x / sres.y;
    const vec2 unscaled_uv = (gl_FragCoord.xy / sres) * 2.f - 1.f;
    const vec2 uv = vec2(unscaled_uv.x * aspect_ratio, unscaled_uv.y);
    const vec3 camera_position = vec3(0.f, 0.f, -5.f);
    const vec3 ro = camera_position;
    const vec3 rd = vec3(uv, 1.f);
    
    const vec3 shaded_color = ray_march(ro, rd);

    FragmentColor = vec4(shaded_color, 1.f);
}