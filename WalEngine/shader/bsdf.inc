#ifndef BSDF_INCLUDED
#define BSDF_INCLUDED

#include "common.inc"

#define FGDTEXTURE_RESOLUTION 64
#define FLT_MIN  1.175494351e-38 

float Lambert()
{
	return INV_PI;
}

//-------------------------------------------
// F
//-------------------------------------------

float FresnelUnpolarized(float ct1, float n1, float n2)
{
	float cti = ct1;
	float st2 = (1.0 - Sq(cti));
	float nr = n2 / n1;
	if (nr == 1.0) { return 0.0; }

	if (Sq(nr)*st2 <= 1.0) 
	{
		float ctt = sqrt(1.0 - Sq(nr)*st2);
		float tpp = (nr*cti - ctt) / (nr*cti + ctt);
		float tps = (cti - nr * ctt) / (nr*ctt + cti);
		return 0.5 * (tpp*tpp + tps * tps);
	}
	else
	{
		return 0.0;
	}
}

float F_Schlick(float f0, float f90, float u)
{
	float x = 1.0 - u;
	float x2 = x * x;
	float x5 = x * x2 * x2;
	return (f90 - f0) * x5 + f0;                
}

float F_Schlick(float f0, float u)
{
	return F_Schlick(f0, 1.0, u);               
}

vec3 F_Schlick(vec3 f0, float f90, float u)
{
	float x = 1.0 - u;
	float x2 = x * x;
	float x5 = x * x2 * x2;
	return f0 * (1.0 - x5) + (f90 * x5);        
}

vec3 F_Schlick(vec3 f0, float u)
{	
	return F_Schlick(f0, 1.0, u);               
}

//---------------------------------------------
// G
// --------------------------------------------
float ComputeSmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
	float ag = (roughness + 1.0) * (roughness + 1.0) / 8.0f ;
	float lambdaV = NdotV / (NdotV * (1 - ag) + ag);
	float lambdaL = NdotL / (NdotL * (1 - ag) + ag);

	return lambdaV * lambdaL;
}

// Note: V = G / (4 * NdotL * NdotV)
// Ref: http://jcgt.org/published/0003/02/03/paper.pdf
float V_SmithJointGGX(float NdotL, float NdotV, float roughness, float partLambdaV)
{
	float a2 = roughness * roughness;

	// Reorder code to be more optimal:
	float lambdaV = NdotL * partLambdaV;
	float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

	return 0.5 / (lambdaV + lambdaL);
}

float GetSmithJointGGXPartLambdaV(float NdotV, float roughness)
{
	float a2 = roughness * roughness;
	return sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
}

float V_SmithJointGGX(float NdotL, float NdotV, float roughness)
{
	float partLambdaV = GetSmithJointGGXPartLambdaV(NdotV, roughness);
	return V_SmithJointGGX(NdotL, NdotV, roughness, partLambdaV);
}

//-----------------------------------------
// D
//-----------------------------------------
float ComputeNDFGGXTerm(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = (a2 - 1.0f) * NdotH * NdotH + 1.0f;
	return a2 * INV_PI / (d * d + 1e-5f);
}

float DV_SmithJointGGX(float NdotH, float NdotL, float NdotV, float roughness, float partLambdaV)
{
	float a2 = Sq(roughness);
	float s = (NdotH * a2 - NdotH) * NdotH + 1.0;

	float lambdaV = NdotL * partLambdaV;
	float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

	vec2 D = vec2(a2, s * s);           
	vec2 G = vec2(1, lambdaV + lambdaL);

	return INV_PI * 0.5 * (D.x * G.x) / max(D.y * G.y, FLT_MIN);
}

// ref: http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
float DisneyDiffuseNoPI(float NdotV, float NdotL, float LdotV, float perceptualRoughness)
{
	float fd90 = 0.5 + (perceptualRoughness + perceptualRoughness * LdotV);

	float lightScatter = F_Schlick(1.0, fd90, NdotL);
	float viewScatter = F_Schlick(1.0, fd90, NdotV);
	return (1.0 / 1.03571) * (lightScatter * viewScatter);
}

//计算漫反射项
vec3 ComputeDisneyDiffuseTerm(float NdotV, float NdotL, float LdotH, float roughness, vec3 baseColor)
{
	float Fd90 = 0.5f + 2.0 * roughness * LdotH * LdotH;
	return baseColor * INV_PI * (1.0 + (Fd90 - 1) * pow(1 - NdotL, 5)) * (1 + (Fd90 - 1) * pow(1 - NdotV, 5));
}

//计算间接光镜面反射菲涅尔项
vec3 ComputeFresnelLerp(vec3 c0, vec3 c1, float cosA)
{
	float t = pow(1 - cosA, 5);
	return mix(c0, c1, t);
}

void GetPreIntegratedFGDGGXAndDisneyDiffuse(sampler2D _PreIntegratedFGD, float NdotV, float perceptualRoughness, vec3 fresnel0, out vec3 specularFGD, out float diffuseFGD, out float reflectivity)
{
	vec2 coordLUT = Remap01ToHalfTexelCoord(vec2(sqrt(NdotV), perceptualRoughness), vec2(FGDTEXTURE_RESOLUTION));

	vec3 preFGD = texture(_PreIntegratedFGD, coordLUT).xyz;

	specularFGD = mix(preFGD.xxx, preFGD.yyy, fresnel0);
	diffuseFGD = preFGD.z + 0.5;
	reflectivity = preFGD.y;
}

float F02Ior(float fresnel0)
{
	return ((1.0 + sqrt(fresnel0)) / (1.0 - sqrt(fresnel0)));
}

vec2 F02Ior(vec2 fresnel0)
{
	return ((1.0 + sqrt(fresnel0)) / (1.0 - sqrt(fresnel0)));
}

vec3 F02Ior(vec3 fresnel0)
{
	return ((1.0 + sqrt(fresnel0)) / (1.0 - sqrt(fresnel0)));
}

float Ior2F0(float transmittedIor, float incidentIor)
{
	return Sq((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
}

vec2 Ior2F0(vec2 transmittedIor, vec2 incidentIor)
{
	return Sq((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
}

vec3 Ior2F0(vec3 transmittedIor, vec3 incidentIor)
{
	return Sq((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
}

vec3 Ior2F0(vec3 transmittedIor)
{
	return Ior2F0(transmittedIor, vec3(1.0f));
}

vec3 ComputeDiffuseColor(vec3 baseColor, float metallic)
{
	return baseColor * (1.0 - metallic);
}

vec3 ComputeFresnel0(vec3 baseColor, float metallic, vec3 dielectricF0)
{
	return mix(dielectricF0.xxx, baseColor, metallic);
}

#endif
