#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMat;

out vec2 uv;

void main() {
    uv = aUv;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}