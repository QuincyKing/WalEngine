#version 430 core

layout (location = 0)in vec3 position;
layout (location = 1)in vec2 texCoord;

uniform mat4 T_model;
uniform mat4 T_VP;

out vec2 texCoord0;

void main()
{
    gl_Position = T_VP * T_model * vec4(position, 1.0);
    texCoord0 = texCoord; 
}