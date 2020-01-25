#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D screenTexture;

const float offset = 1.0 / 600.0;

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0),
        vec2(0.0, 0.0),
        vec2(offset, 0.0),
        vec2(-offset, -offset),
        vec2(0.0, -offset),
        vec2(offset, -offset)
    );

    float kernel_x[9] = float[] (
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    );

    float kernel_y[9] = float[] (
        -1, -2, -1,
         0,  0,  0,
         1, -2,  1
    );

    vec3 color_x = vec3(0.0);
    vec3 color_y = vec3(0.0);
    vec3 color = vec3(0.0);

    for (int i = 0; i < 9; ++i) {
        color_x += vec3(texture(screenTexture, uv.st + offsets[i])) * kernel_x[i];
        color_y += vec3(texture(screenTexture, uv.st + offsets[i])) * kernel_y[i];
    }

    color = sqrt(color_x*color_x + color_y*color_y);

    FragColor = vec4(color, 1.0);
}
