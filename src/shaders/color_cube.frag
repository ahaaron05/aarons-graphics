#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);    // use max to ensure never a negative diffuse component
    vec3 diffuse = light.diffuse * (diff *  material.diffuse);

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);   // reflect light dir
    float spec = pow(max(dot(viewDir, reflectDir), 0.0) , material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Phong
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0f);
}