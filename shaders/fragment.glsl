#version 330 core

out vec4 fragColor;

in float ageV;

void main() {
    // make points circular
    vec2 coord = 2.0 * gl_PointCoord - 1.0;
    if (dot(coord, coord) > 1.0) discard;


    float scale = (1.0 - ageV);
    vec3 color = vec3(scale, 0, scale);
    fragColor = vec4(color, 1);
}
