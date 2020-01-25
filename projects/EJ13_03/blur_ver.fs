#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D screenTexture;

const float offset = 600.0;

void main() {
   //Gaussian blur with linear sampling from http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
   float offsets[3] = float[] (0.0, 1.3846153846, 3.2307692308 );
   float kernel[3] = float[] (0.2270270270, 0.3162162162, 0.0702702703);

   vec3 color = vec3(0.0);

   color = vec3(texture(screenTexture, uv.st/600)) * kernel[0];

    for (int i = 1; i < 3; ++i) {
        color += vec3(texture(screenTexture, (uv.st + vec2(0.0, offsets[i])/offset))) * kernel[i];
        color += vec3(texture(screenTexture, (uv.st - vec2(0.0, offsets[i])/offset))) * kernel[i];
    }

    FragColor = vec4(color, 1.0);
}