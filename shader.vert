#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;
out vec3 Color;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * transform * vec4(aPos, 1.0f);
   Normal = mat3(transpose(inverse(transform))) * aNormal;  
   FragPos = vec3(transform * vec4(aPos, 1.0));
   Color = aColor;
}