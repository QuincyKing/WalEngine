#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in vec3 aNormal;

uniform mat4 T_VP;
uniform mat4 T_model;

out vec2 Tex;
out vec3 Normal;
out vec3 Pos;

void main()
{
	Tex = aTex * vec2(7, 7);
	Pos = (T_model * vec4(aPos, 1.0)).xyz; 
	Normal = aNormal;
	gl_Position = T_VP * T_model * vec4(aPos, 1.0);
}