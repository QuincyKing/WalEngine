#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#define FLT_EPS		5.960464478e-8 
#define rcp(x)		1.0 / (x)
#define REAL_EPS	4.8828125e-4
#define INV_PI      0.31830988618
#define TWO_PI      6.28318530717958647693

float Mean(vec3 a)
{ 
	return (a.x + a.y + a.z) / 3.0; 
}

float Sq(float x)
{
	return x * x;
}

vec2 Sq(vec2 x)
{
	return vec2(x.x * x.x, x.y * x.y);
}

vec3 Sq(vec3 x)
{
	return vec3(x.x * x.x, x.y * x.y, x.z * x.z);
}

vec3 SphericalToCartesian(float cosPhi, float sinPhi, float cosTheta)
{
	float sinTheta = sqrt(clamp(1 - cosTheta * cosTheta, 0.0, 1.0));
	return vec3(vec2(cosPhi, sinPhi) * sinTheta, cosTheta);
}

vec3 SphericalToCartesian(float phi, float cosTheta)
{
	float sinPhi = sin(phi);
	float cosPhi = cos(phi);

	return SphericalToCartesian(cosPhi, sinPhi, cosTheta);
}

#endif
