#version 430

uniform sampler2D sampler0;
uniform vec4 matAmbient;

in vec2 tex;
out vec4 FragColor;

void main()
{
	vec4 base = texture(sampler0, tex);
	FragColor = base * matAmbient;
}
