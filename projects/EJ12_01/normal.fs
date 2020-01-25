#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec2 uv;
in vec3 fNormal;
in vec3 tangent;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    int shininess;
};
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform vec3 viewPos;

uniform mat3 normalMat;

void main() {
    //Calculamos matriz TBN
    vec3 T = normalize(normalMat * tangent);
    vec3 N = normalize(normalMat * fNormal);
    T = normalize(T - dot(T,N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);

    //Transformamos las normales a espacio de mundo
    vec3 normal = vec3(texture(material.normal, uv));
    normal = normalize(normal*2.0 - 1.0);
    vec3 norm = normalize(TBN*normal);

    vec3 albedo = vec3(texture(material.diffuse, uv));
    vec3 ambient = albedo * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * albedo * light.diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, uv)) * light.specular;

    vec3 phong = ambient + diffuse + specular;
    FragColor = vec4(phong, 1.0f);
}