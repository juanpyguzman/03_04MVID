#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

//Recibimos la matriz view para transformar la posición de la luz a espacio de vista
uniform mat4 view;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float ambientStrength;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int shininess;
uniform float specularStrength;

void main() {
    //Transformamos la posición de la luz ligthPos a espacio de vista lightPosView
    vec3 lightPosView = vec3(view * vec4(lightPos, 1.0));

    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    // Usamos lightPosView
    vec3 lightDir = normalize(lightPosView - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //La posición de la cámara es (0,0,0)
    vec3 viewDir = normalize(-fragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = spec * specularStrength * lightColor;

    vec3 phong = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(phong, 1.0f);
}