#version 330 core

in vec3 someColor;

out vec4 fragColor;

void main()
{               // grey-tan color
    fragColor = vec4(vec3(0.875, 0.902, 0.831), 1.0f);
}