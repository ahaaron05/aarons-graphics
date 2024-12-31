#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);    // use max to ensure never a negative diffuse component
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5f;
    int shininessFactor = 256;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);   // reflect light dir horizontally
    float spec = pow(max(dot(viewDir, reflectDir), 0.0) , shininessFactor);
    vec3 specular = specularStrength * spec * lightColor;

    // Phong
    vec3 result = (ambient + diff + specular) * objColor;
    fragColor = vec4(result, 1.0f);
}