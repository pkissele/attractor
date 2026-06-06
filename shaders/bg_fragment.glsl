#version 330 core
in vec2 uv;
out vec4 fragColor;

void main() {
    vec3 bgColor = vec3(0.0, 0.0, 0.0);
    fragColor = vec4(bgColor, 1);
}
