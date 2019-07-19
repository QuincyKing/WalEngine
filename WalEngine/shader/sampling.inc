#ifndef SAMPLING_INCLUDED
#define SAMPLING_INCLUDED

#include "math.inc"
#include "common.inc"
#include "bsdf.inc"

vec3 SampleSphereUniform(float u1, float u2)
{
	float phi = TWO_PI * u2;
	float cosTheta = 1.0 - 2.0 * u1;

	return SphericalToCartesian(phi, cosTheta);
}

// Ref: http://www.amietia.com/lambertnotangent.html
vec3 SampleHemisphereCosine(float u1, float u2, vec3 normal)
{
	vec3 pointOnSphere = SampleSphereUniform(u1, u2);
	return normalize(normal + pointOnSphere);
}

void ImportanceSampleLambert(vec2 u, mat3 localToWorld, out vec3 L, out float NdotL, out float weightOverPdf)
{
	vec3 N = localToWorld[2];
	L = SampleHemisphereCosine(u.x, u.y, N);
	NdotL = clamp(dot(N, L), 0.0, 1.0);

	weightOverPdf = 1.0;
}

void SampleGGXDir(vec2 u, vec3 V, mat3 localToWorld, float roughness,
	out vec3   L,
	out float    NdotL,
	out float    NdotH,
	out float    VdotH,
	bool    VeqN = false)
{
	// GGX NDF sampling
	float cosTheta = sqrt(SafeDiv(1.0 - u.x, 1.0 + (roughness * roughness - 1.0) * u.x));
	float phi = TWO_PI * u.y;

	vec3 localH = SphericalToCartesian(phi, cosTheta);

	NdotH = cosTheta;

	vec3 localV;

	if (VeqN)
	{
		localV = vec3(0.0, 0.0, 1.0);
		VdotH = NdotH;
	}
	else
	{
		localV = mul(transpose(localToWorld), V);
		VdotH = clamp(dot(localV, localH), 0.0, 1.0);
	}

	vec3 localL = -localV + 2.0 * VdotH * localH;
	NdotL = localL.z;

	L = mul(localToWorld, localL);
}

void ImportanceSampleGGX(vec2 u, vec3 V, mat3 localToWorld, float roughness, float NdotV,
	out vec3   L,
	out float  VdotH,
	out float  NdotL,
	out float  weightOverPdf)
{
	float NdotH;
	SampleGGXDir(u, V, localToWorld, roughness, L, NdotL, NdotH, VdotH);

	float Vis = V_SmithJointGGX(NdotL, NdotV, roughness);
	weightOverPdf = 4.0 * Vis * NdotL * VdotH / NdotH;
}

#endif
