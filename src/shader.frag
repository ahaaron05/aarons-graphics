#version 330 core

in vec3 someColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(someColor, 1);
}