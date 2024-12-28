#version 330 core

in vec3 someColor;
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, texCoord) * vec4(someColor, 1.0f);
}