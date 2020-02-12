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
uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;

out vec3 normal;
out vec3 fragPos;
out vec2 uv;

out vec4 fragPosLightSpace;

out vec3 tangentDirLightPos;
out vec3 tangentPointLightPos0;
out vec3 tangentPointLightPos1;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

struct DirLight {
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define NUMBER_POINT_LIGHTS 2
uniform PointLight pointLight[NUMBER_POINT_LIGHTS];

void main() {
    uv = aUv;
    normal = normalMat * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));

    vec3 T = normalize(normalMat * aTangent);
    vec3 N = normalize(normalMat * aNormal);
    T = normalize(T - dot(T,N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    tangentDirLightPos = TBN * dirLight.position;
    tangentPointLightPos0 = TBN * pointLight[0].position;
    tangentPointLightPos1 = TBN * pointLight[1].position;     	
    tangentViewPos = TBN * viewPos;
    tangentFragPos = TBN * fragPos;

    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    gl_Position = proj * view * model * vec4(aPos, 1.0);
}