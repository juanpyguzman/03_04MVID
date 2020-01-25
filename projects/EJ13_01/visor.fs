#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 uv;

struct Material {
    sampler2D diffuse;
};
uniform Material material;

uniform vec3 viewPos;

void main() {
    vec3 tex = vec3(texture(material.diffuse, uv));
    FragColor = vec4(tex, 1.0f);
}