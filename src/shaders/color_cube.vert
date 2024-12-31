#version 330 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0f));
    normal = mat3(normalMat) * aNormal;
    
    gl_Position = projection * view * vec4(fragPos, 1.0);
}