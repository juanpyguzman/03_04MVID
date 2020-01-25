#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec3 aTangent;
layout (location=4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMat;

out vec3 fragPos;
out vec2 uv;
out vec3 fNormal;
out vec3 tangent;

void main() {
    uv = aUv;
    fNormal = aNormal;
    tangent = aTangent;

    fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}