#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 uv;

in vec4 fragPosLightSpace;

in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    int shininess;
};
uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

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
#define NUMBER_POINT_LIGHTS 1
uniform PointLight pointLight[NUMBER_POINT_LIGHTS];

uniform vec3 viewPos;

uniform sampler2D depthMap;

uniform bool isModel;

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for (int y = -1; y <=1; ++y) {
            float pcf = texture(depthMap, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth -bias > pcf ? 1.0 : 0.0;
        }
	}
    shadow /= 9.0;

    if (projCoords.z > 1.0) {
     shadow = 0.0;
	}

    return shadow;
}

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedoMap) {
    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, uv)) * light.specular;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fragPosLightSpace, bias);
    
    vec3 phong = ambient + ((1.0 - shadow) * (diffuse + specular));

    return phong;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedoMap) {
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
        light.linear * distance +
        light.quadratic * distance * distance);

    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, uv)) * light.specular;

    return (ambient + diffuse + specular) * attenuation;
    }

void main() {
    if (isModel){
    
        vec3 normal = vec3(texture(texture_normal1, uv));
        normal = normalize(normal * 2.0 - 1.0);

        vec3 albedo = vec3(texture(texture_diffuse1, uv));
        vec3 ambient = albedo * dirLight.ambient;

        vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * albedo * dirLight.diffuse;

        vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = spec * vec3(texture(texture_specular1, uv)) * dirLight.specular;

        float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
        float shadow = ShadowCalculation(fragPosLightSpace, bias);
    
        vec3 phong = ambient + ((1.0 - shadow) * (diffuse + specular));
        FragColor = vec4(phong, 1.0f);

	}

    else {
    
        vec3 normal = vec3(texture(material.normal, uv));
        normal = normalize(normal * 2.0 - 1.0);

        vec3 albedo = vec3(texture(material.diffuse, uv));
        vec3 ambient = albedo * dirLight.ambient;

        vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

        vec3 phong = calcDirectionalLight(dirLight, normal, viewDir, albedo);

        
        for (int i = 0; i < NUMBER_POINT_LIGHTS; ++i) {
            phong += calcPointLight(pointLight[i], normal, viewDir, fragPos, albedo);
	    }

        FragColor = vec4(phong, 1.0f);
    }
}