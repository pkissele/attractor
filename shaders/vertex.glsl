#version 330 core

layout(location = 0) in float px;
layout(location = 1) in float py;
layout(location = 2) in float pz;
layout(location = 3) in float age;


// uniform float pointSize;
uniform vec2 displaySize;
uniform vec2 displayOffset;

out float ageV;

void main() {
    vec2 in_pos = vec2(px, py);
    vec2 pos = in_pos - displayOffset;
    vec2 ndc = (pos / displaySize) * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = 3;
    ageV = age;
}
