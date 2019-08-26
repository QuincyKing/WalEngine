#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexcoord0;

uniform mat4 T_model;
uniform mat4 T_VP;
uniform vec2 uv;

out vec2 tex;

void main()
{
	tex = aTexcoord0 * uv;
	gl_Position = T_VP * T_model * vec4(aPos, 1.0);
}
