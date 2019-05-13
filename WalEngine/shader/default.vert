#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 T_VP;
uniform mat4 T_model;

void main()
{
	gl_Position = T_VP * T_model * vec4(aPos, 1.0);
}