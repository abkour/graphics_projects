#version 450 core 
out vec4 FragmentColor;

in float height;
in float width;

void main() {
    FragmentColor = vec4(abs(sin(height)), abs(cos(height)), 1.f - width, 1.f);
}