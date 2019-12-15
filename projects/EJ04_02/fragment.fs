#version 330 core

in vec2 UV;
vec2 flippedUV;
out vec4 FragColor;

uniform sampler2D tex_1;

void main() {
    flippedUV = vec2(UV.x,-UV.y);	
    FragColor = texture(tex_1, flippedUV);
}