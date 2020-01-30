#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMat;
uniform mat4 light1SpaceMatrix;
uniform mat4 light2SpaceMatrix;

out vec3 normal;
out vec3 fragPos;
out vec2 uv;
out vec4 fragPosLight1Space;
out vec4 fragPosLight2Space;

void main() {
    uv = aUv;
    normal = normalMat * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragPosLight2Space = light1SpaceMatrix * vec4(fragPos, 1.0);
    fragPosLight2Space = light2SpaceMatrix * vec4(fragPos, 1.0);
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}