#version 450 core 
layout(location = 0) in float aX;
layout(location = 1) in float aY;

uniform float zoom;
uniform vec2 cursor_pos;

uniform vec2 origin;
uniform vec2 range;

uniform vec2 min_value;     
uniform vec2 max_value;     

float convert_range(vec2 old_r, vec2 new_r, float v) {
    return (((v - old_r.x) * (new_r.y - new_r.x)) / (old_r.y - old_r.x)) + new_r.x;
}

vec2 normalize_value(vec2 val) {
    float normalized_x = convert_range(vec2(min_value.x, max_value.x), vec2(origin.x, origin.x + range.x), val.x);
    float normalized_y = convert_range(vec2(min_value.y, max_value.y), vec2(origin.y, origin.y + range.y), val.y);
    return vec2(normalized_x, normalized_y);
}

void main() {
    vec2 xy_coord = (cursor_pos + normalize_value(vec2(aX, aY))) * max(0.f, zoom);
    gl_Position = vec4(xy_coord, 0.f, 1.f);
}