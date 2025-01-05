#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D emissionMap;
    float shininess;
};

struct Light {
    vec3 position; // not necessary when using directional.
    vec3 direction;   
    float cutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    if(theta > light.cutOff) // for flashlight calculations
    {
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuseMap, texCoords).rgb;

        // diffuse
        vec3 norm = normalize(normal);
        //vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(norm, lightDir), 0.0f);    // use max to ensure never a negative diffuse component
        vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, texCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);   // reflect light dir
        float spec = pow(max(dot(viewDir, reflectDir), 0.0) , material.shininess);
        vec3 specular = light.specular * spec * texture(material.specularMap, texCoords).rgb;

        // emmission
        //vec3 emission = texture(material.emissionMap, texCoords).rgb;

        // attenuation calculation (for spot-light)
        float distance = length(light.position - fragPos); // think magnitude
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        // //emmission *= attenuation;

        // Phong
        vec3 result = ambient + diffuse + specular;
        fragColor = vec4(result, 1.0f);
    }
    else // use ambient light so scene is not completely dark outside the spotlight
    {
        fragColor = vec4(light.ambient * vec3(texture(material.diffuseMap, texCoords)), 1.0f);
    }  
}