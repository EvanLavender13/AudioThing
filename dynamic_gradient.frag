#version 330 core

uniform float time;
uniform vec2 resolution;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 p = uv * 2.0 - 1.0;
    p.x *= resolution.x / resolution.y;

    // Create a dynamic gradient background
    float gradient = 0.5 + 0.5 * sin(time + p.y * 10.0);
    vec3 color = mix(vec3(0.1, 0.2, 0.5), vec3(0.8, 0.3, 0.5), gradient);

    gl_FragColor = vec4(color, 1.0);
}
