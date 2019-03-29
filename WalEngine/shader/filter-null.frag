#version 430 core

out vec4 fragColor;

in vec2 texCoord0;

uniform sampler2D filterTexture;

void main()
{
	fragColor = texture(filterTexture, texCoord0);
}