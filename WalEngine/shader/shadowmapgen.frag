#version 430 core

out vec4 FragColor;

void main()
{
	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);

	float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

	FragColor = vec4(depth, moment2, 0.0, 0.0);
}