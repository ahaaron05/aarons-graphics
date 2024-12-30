#version 330 core

out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 objColor;

void main()
{
    fragColor = vec4(lightColor * objColor, 1.0f);
}