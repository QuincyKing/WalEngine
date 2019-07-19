#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#include "math.inc"

float ClampNdotV(float NdotV)
{
	return max(NdotV, 0.0001); 
}

float PerceptualRoughnessToRough(float perceptualRoughness)
{
	return perceptualRoughness * perceptualRoughness;
}

float RoughnessToPerceptualRoughness(float roughness)
{
	return sqrt(roughness);
}

float RoughnessToLinearVariance(float a)
{
	a = clamp(a, 0.0, 0.9999);
	float a3 = pow(a, 1.1);
	return a3 / (1.0f - a3);
}

float PerceptualRoughnessToLinearVariance(float a)
{
	a = PerceptualRoughnessToRough(a);
	return RoughnessToLinearVariance(a);
}

float LinearVarianceToRoughness(float v)
{
	v = max(v, 0.0);
	float a = pow(v / (1.0 + v), 1.0 / 1.1);
	return a;
}

float LinearVarianceToPerceptualRoughness(float v)
{
	return RoughnessToPerceptualRoughness(LinearVarianceToRoughness(v));
}

// Ref: http://marc-b-reynolds.github.io/quaternions/2016/07/06/Orthonormal.html
mat3 GetLocalFrame(vec3 localZ)
{
	float x = localZ.x;
	float y = localZ.y;
	float z = localZ.z;
	float sz = sign(z);
	float a = 1 / (sz + z);
	float ya = y * a;
	float b = x * ya;
	float c = x * sz;

	vec3 localX = vec3(c * x * a - 1, sz * b, c);
	vec3 localY = vec3(b, y * ya - sz, y);

	return mat3(localX, localY, localZ);
}

//V和N向量根据Gram-Schmidt方法构建正交向量组
vec3 GetOrthogonalComponent(vec3 V, vec3 N, bool testSingularity = false)
{
	float VdotN = dot(V, N);
	vec3 unitVOrtho;

	if (testSingularity && (abs(1.0 - VdotN) <= FLT_EPS))
	{
		mat3 orthoBasis = GetLocalFrame(N);
		unitVOrtho = orthoBasis[0];
	}
	else
	{
		vec3 VOrtho = V - VdotN * N;
		unitVOrtho = VOrtho * (1.0 / sqrt(max(1.0 - Sq(VdotN), FLT_EPS)));
	}
	//N 和 uniyVOrtho 是正交向量组
	return unitVOrtho;
}

vec3 GetDirFromAngleAndOrthoFrame(vec3 V, vec3 N, float newVdotN)
{
	float sintheta = sqrt(1.0 - Sq(newVdotN));
	vec3 newV = newVdotN * N + sintheta * V;
	return newV;
}

float Remap01(float x, float rcpLength, float startTimesRcpLength)
{
	return clamp(x * rcpLength - startTimesRcpLength, 0.0, 1.0);
}

vec2 Remap01(vec2 x, vec2 rcpLength, vec2 startTimesRcpLength)
{
	return clamp(x * rcpLength - startTimesRcpLength, 0.0, 1.0);
}

vec3 Remap01(vec3 x, vec3 rcpLength, vec3 startTimesRcpLength)
{
	return clamp(x * rcpLength - startTimesRcpLength, 0.0, 1.0);
}

vec2 RemapHalfTexelCoordTo01(half2 coord, vec2 size)
{
	const vec2 rcpLen = size * rcp(size - 1);
	const vec2 startTimesRcpLength = 0.5 * rcp(size - 1);

	return Remap01(coord, rcpLen, startTimesRcpLength);
}

vec2 Remap01ToHalfTexelCoord(vec2 coord, vec2 size)
{
	const vec2 start = 0.5 * rcp(size);
	const vec2 len = 1 - rcp(size);

	return coord * len + start;
}

float SafeDiv(float numer, float denom)
{
	return (numer != denom) ? numer / denom : 1;
}

// Ref: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; 
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 GetSpecularDominantDir(vec3 N, vec3 R, float perceptualRoughness, float NdotV)
{
	float p = perceptualRoughness;
	float a = 1.0 - p * p;
	float s = sqrt(a);

	float lerpFactor = (s + p * p) * a;
	return mix(N, R, lerpFactor);
}

float PerceptualRoughnessToMipmapLevel(float perceptualRoughness)
{
	perceptualRoughness = perceptualRoughness * (1.7 - 0.7 * perceptualRoughness);

	return perceptualRoughness * 6 /*UNITY_SPECCUBE_LOD_STEPS*/;
}

void UpdateLightingHierarchyWeights(inout float hierarchyWeight, inout float weight)
{
	float accumulatedWeight = hierarchyWeight + weight;
	hierarchyWeight = clamp(accumulatedWeight, 0.0, 1.0);
	weight -= clamp(accumulatedWeight - hierarchyWeight, 0.0, 1.0);
}

float PerceptualSmoothnessToPerceptualRoughness(float perceptualSmoothness)
{
	return (1.0 - perceptualSmoothness);
}

void ConvertValueAnisotropyToValueTB(float value, float anisotropy, out float valueT, out float valueB)
{
	// Use the parametrization of Sony Imageworks.
	// Ref: Revisiting Physically Based Shading at Imageworks, p. 15.
	valueT = value * (1 + anisotropy);
	valueB = value * (1 - anisotropy);
}

void ConvertAnisotropyToRoughness(float perceptualRoughness, float anisotropy, out float roughnessT, out float roughnessB)
{
	float roughness = PerceptualRoughnessToRough(perceptualRoughness);
	ConvertValueAnisotropyToValueTB(roughness, anisotropy, roughnessT, roughnessB);
}

void ConvertAnisotropyToClampRoughness(float perceptualRoughness, float anisotropy, out float roughnessT, out float roughnessB)
{
	ConvertAnisotropyToRoughness(perceptualRoughness, anisotropy, roughnessT, roughnessB);

	roughnessT = max(0.0001, roughnessT);
	roughnessB = max(0.0001, roughnessB);
}

#endif
