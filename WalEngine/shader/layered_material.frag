#version 430 core

out vec4 outColor;

in vec2 Tex;
in vec3 WorldPos;
in vec3 Normal;

#include "light_definition.inc"
#include "light.inc"
#include "sampling.inc"

#define NB_NORMALS 2
#define COAT_NB_LOBES 1
#define COAT_LOBE_IDX 0
#define BASE_LOBEA_IDX (COAT_LOBE_IDX+1)
//#define BASE_LOBEB_IDX (BASE_LOBEA_IDX+1)
#define BASE_NB_LOBES 1 
#define TOTAL_NB_LOBES (BASE_NB_LOBES+COAT_NB_LOBES)
#define NB_VLAYERS 3
#define TOP_VLAYER_IDX 0
#define BOTTOM_VLAYER_IDX 2
#define COAT_NORMAL_IDX 0
#define BASE_NORMAL_IDX (NB_NORMALS-1)
//#define ZERO_INITIALIZE(type, name) name = (type)0;
#define VLAYERED_DUAL_NORMALS_TOP_FIX_FLIP
#define NB_LV_DIR 1
#define DNLV_COAT_IDX 0
#define DNLV_BASE_IDX 1
#define NDOTLV_SIZE NB_NORMALS
#define TOP_DIR_IDX 0
#define BOTTOM_DIR_IDX (NB_LV_DIR-1)

struct PreLightData
{
	float NdotV[NB_NORMALS];                  // Could be negative due to normal mapping, use ClampNdotV()
	float geomNdotV;
	float bottomAngleFGD;                     // Only used when dual normal maps are enabled, set by ComputeAdding()
	float baseLayerNdotV;                     // Already had ClampNdotV() applied; for GetSpecularDominantDir (see EvaluateBSDF_Env)
											  // (should be merged with the above, compiler should deal with that)
	float TdotV;                              
	float BdotV;

	vec3 iblR[TOTAL_NB_LOBES];              
	float  iblPerceptualRoughness[TOTAL_NB_LOBES];

	vec3 specularFGD[TOTAL_NB_LOBES];      

	float  diffuseFGD;

	vec3 fresnelIridforCalculatingFGD;

	// For clarity, we will dump the base layer lobes roughnesses used by analytical lights
	// here, to avoid confusion with the per-vlayer (vs per lobe) vLayerPerceptualRoughness
	// (also, those don't need to be anisotropic for all lobes but the non-separated
	// original roughnesses are still useful for all lobes because of the IBL hack)
	//
	// Again layered* are used by analytical dirac (non area, ie point and directional) lights
	// and are thus clamped.
	// We don't reuse the BSDFData roughnessAT/AB/BT/BB because we might need these original
	// values per light (ie not only once at GetPreLightData time) to recompute new roughnesses
	// if we use VLAYERED_RECOMPUTE_PERLIGHT.
	float  layeredRoughnessT[BASE_NB_LOBES];
	float  layeredRoughnessB[BASE_NB_LOBES];
	float  layeredCoatRoughness;
	// For consistency with these nonperceptual anisotropic and clamped roughnessAT/AB/BT/BB
	// coatRoughness for analytical dirac lights will also be stored here;
	// These should always be:
	// preLightData.iblPerceptualRoughness[COAT_LOBE_IDX] = bsdfData.coatPerceptualRoughness;
	// preLightData.layeredCoatRoughness = ClampRoughnessForAnalyticalLights(bsdfData.coatRoughness);

	float  iblAnisotropy[BASE_NB_LOBES];

	// GGX
	float partLambdaV[TOTAL_NB_LOBES];        // Depends on N, V, roughness
	// If we use VLAYERED_RECOMPUTE_PERLIGHT, we will recalculate those also.
	// (ComputeAdding changing roughness per light is what will make them change).
	//
	// This used to be strictly done in GetPreLightData, but since this is NOT useful
	// for IBLs, if vlayering is enabled and we want the vlayer stats recomputation
	// per analytical light, we must NOT do it in GetPreLightData (will be wasted) and
	// (in effect can't be precalculated for all analytical lights).
	//
	// In short: only valid and precalculated at GetPreLightData time if vlayering is disabled.
	//

	float coatIeta;

	vec3 vLayerEnergyCoeff[NB_VLAYERS];
	// TODOENERGY
	// For now since FGD fetches aren't used in compute adding (instead we do non integrated
	// Fresnel( ) evaluations and 1 - Fresnel( ) which is wrong, the former only ok for analytical
	// lights for the top interface for R12), we will use these for FGD fetches but keep them
	// for BSDF( ) eval for analytical lights since the later don't use FGD terms.


	// TODOENERGY:
	// For the vlayered case, fold compensation into FGD terms during ComputeAdding
	// (ie FGD becomes FGDinf) (but the approximation depends on f0, our FGD is scalar,
	// not rgb, see GetEnergyCompensationFactor.)

	// (see ApplyEnergyCompensationToSpecularLighting)
	// We will compute vec3 energy factors per lobe.
	// We will duplicate one entry to simplify the IBL loop (In general it's either that or
	// we add branches (if lobe from bottom interface or top inteface)
	// (All our loops for lobes are static so either way the compiler should unroll and remove
	// either duplicated storage or the branch.)
	vec3 energyCompensationFactor[TOTAL_NB_LOBES];


	//See VLAYERED_DIFFUSE_ENERGY_HACKED_TERM
	vec3 diffuseEnergy; // We don't fold into diffuseFGD because of analytical lights that require it separately.

	mat3 ltcTransformDiffuse;                    // Inverse transformation for Lambertian or Disney Diffuse
	mat3 ltcTransformSpecular[TOTAL_NB_LOBES];   // Inverse transformation for GGX
};

uniform vec4 _Color;
uniform sampler2D _MainTex;
uniform sampler2D _MetallicGlossMap;
uniform sampler2D _RoughnessMap;
uniform sampler2D _BumpMap;
uniform sampler2D _OcclusionMap;
uniform float _LumiScale;
uniform sampler2D _PreFGDandDisneyDiffuse;
uniform float _DelectricIOR;
uniform sampler2D _BentNormal;
uniform sampler2D _GeomNormal;
uniform sampler2D _CoatNormalMap;
uniform float _CoatPerceptualRoughness;
uniform float _CoatIOR;
uniform float _CoatThickness;
uniform vec4 _CoatExtinction;
uniform float _IBLLDScale;
uniform samplerCube prefilterMap;
uniform vec3 M_CamPos;
uniform DirectionalLight light;

float GetCoatEta(in BSDFData bsdfData)
{
	float eta = bsdfData.coatIor / 1.0;
	return eta;
}

void ComputeAdding_GetVOrthoGeomN(BSDFData bsdfData, vec3 V, bool calledPerLight, out vec3 vOrthoGeomN, out bool useGeomN, bool testSingularity = false)
{
	vOrthoGeomN = vec3(0);
	useGeomN = false;

	if (!calledPerLight)
	{
		vOrthoGeomN = GetOrthogonalComponent(V, bsdfData.geomNormalWS, testSingularity);
		useGeomN = true;
	}
}

void ComputeStatistics(float  cti, vec3 V, vec3 vOrthoGeomN, bool useGeomN, int i, BSDFData bsdfData,
	inout PreLightData preLightData,
	out float  ctt,
	out vec3 R12, out vec3 T12, out vec3 R21, out vec3 T21,
	out float  s_r12, out float  s_t12, out float  j12,
	out float  s_r21, out float  s_t21, out float  j21)
{
	if (i == 0)
	{
		float R0, n12;

		n12 = GetCoatEta(bsdfData);
		R0 = FresnelUnpolarized(cti, n12, 1.0);

		if (useGeomN)
		{
			cti = ClampNdotV(dot(bsdfData.geomNormalWS, V));
		}

		R12 = vec3(R0); 
		T12 = 1.0 - R12;
		R21 = R12;
		T21 = T12;

		float sti = sqrt(1.0 - Sq(cti));
		float stt = sti / n12;
		if (stt <= 1.0f)
		{
			const float alpha = bsdfData.coatRoughness;
			const float scale = clamp((1.0 - alpha)*(sqrt(1.0 - alpha) + alpha), 0.0, 1.0);
			//http://www.wolframalpha.com/input/?i=f(alpha)+%3D+(1.0-alpha)*(sqrt(1.0-alpha)+%2B+alpha)+alpha+%3D+0+to+1
			stt = scale * stt + (1.0 - scale)*sti;
			ctt = sqrt(1.0 - stt * stt);
		}
		else
		{
			ctt = -1.0;
		}

		s_r12 = RoughnessToLinearVariance(bsdfData.coatRoughness);
		s_t12 = RoughnessToLinearVariance(bsdfData.coatRoughness * 0.5 * abs((ctt*n12 - cti) / (ctt*n12)));
		j12 = (ctt / cti)*n12;

		s_r21 = s_r12;
		s_t21 = RoughnessToLinearVariance(bsdfData.coatRoughness * 0.5 * abs((cti / n12 - ctt) / (cti / n12)));
		j21 = 1.0 / j12;
	}
	// Case of the media layer
	else if (i == 1)
	{
		R12 = vec3(0.0, 0.0, 0.0);
		T12 = exp(-bsdfData.coatThickness * bsdfData.coatExtinction / cti);
		R21 = R12;
		T21 = T12;

		ctt = cti;

		s_r12 = 0.0;
		s_t12 = 0.0;
		j12 = 1.0;

		s_r21 = 0.0;
		s_t21 = 0.0;
		j21 = 1.0;
	}
	// Case of the dielectric / conductor base
	else
	{
		float ctiForFGD = cti;

		if (useGeomN)
		{
			//TODO ??
			vec3 bottomDir = GetDirFromAngleAndOrthoFrame(vOrthoGeomN, bsdfData.geomNormalWS, cti);
			ctiForFGD = ClampNdotV(dot(bsdfData.normalWS, bottomDir));
		}
		// We will also save this average bottom angle:
		preLightData.bottomAngleFGD = ctiForFGD;

		R12 = F_Schlick(bsdfData.fresnel0, ctiForFGD);
		T12 = 1.0 - R12;
		R21 = R12;
		T21 = T12;

		ctt = cti;
		s_r12 = 0.0;

		s_t12 = 0.0;
		j12 = 1.0;

		s_r21 = s_r12;
		s_t21 = 0.0;
		j21 = 1.0;
	}
}

//Ref : https://hal.archives-ouvertes.fr/hal-01785457/document
void ComputeAdding(
	float _cti,
	vec3 V, 
	in BSDFData bsdfData,
	inout PreLightData preLightData,
	bool calledPerLight = false, 
	bool testSingularity = false)
{
	bool useGeomN;
	vec3 vOrthoGeomN;
	ComputeAdding_GetVOrthoGeomN(bsdfData, V, calledPerLight, vOrthoGeomN, useGeomN, testSingularity);
	if (useGeomN)
	{
		_cti = max(abs(dot(bsdfData.coatNormalWS, V)), 0.1);
		_cti = min(_cti, ClampNdotV(preLightData.geomNdotV));
	}

	float  cti = _cti;
	vec3 R0i = vec3(0.0, 0.0, 0.0), Ri0 = vec3(0.0, 0.0, 0.0),
		T0i = vec3(1.0, 1.0, 1.0), Ti0 = vec3(1.0, 1.0, 1.0);
	float  s_r0i = 0.0, s_ri0 = 0.0, s_t0i = 0.0, s_ti0 = 0.0;
	float  j0i = 1.0, ji0 = 1.0;

	float _s_r0m, s_r12, m_rr; 
	vec3 m_R0i;
	vec3 localvLayerEnergyCoeff[NB_VLAYERS];

	for (int i = 0; i < NB_VLAYERS; ++i)
	{
		vec3 R12, T12, R21, T21;
		s_r12 = 0.0;
		float s_r21 = 0.0, s_t12 = 0.0, s_t21 = 0.0, j12 = 1.0, j21 = 1.0, ctt;

		ComputeStatistics(cti, V, vOrthoGeomN, useGeomN, i, bsdfData, preLightData, ctt, R12, T12, R21, T21, s_r12, s_t12, j12, s_r21, s_t21, j21);

		vec3 denom = (vec3(1.0, 1.0, 1.0) - Ri0 * R12);
		m_R0i = (Mean(denom) <= 0.0f) ? vec3(0.0, 0.0, 0.0) : (T0i*R12*Ti0) / denom; 
		vec3 m_Ri0 = (Mean(denom) <= 0.0f) ? vec3(0.0, 0.0, 0.0) : (T21*Ri0*T12) / denom; 
		vec3 m_Rr = (Mean(denom) <= 0.0f) ? vec3(0.0, 0.0, 0.0) : (Ri0*R12) / denom;
		float  m_r0i = Mean(m_R0i);
		float  m_ri0 = Mean(m_Ri0);
		m_rr = Mean(m_Rr);

		vec3 e_R0i = R0i + m_R0i; 
		vec3 e_T0i = (T0i*T12) / denom; 
		vec3 e_Ri0 = R21 + (T21*Ri0*T12) / denom; 
		vec3 e_Ti0 = (T21*Ti0) / denom; 

		float r21 = Mean(R21);
		float r0i = Mean(R0i);
		float e_r0i = Mean(e_R0i);
		float e_ri0 = Mean(e_Ri0);

		_s_r0m = s_ti0 + j0i * (s_t0i + s_r12 + m_rr * (s_r12 + s_ri0));
		float _s_r0i = (r0i*s_r0i + m_r0i * _s_r0m) / e_r0i;
		float _s_t0i = j12 * s_t0i + s_t12 + j12 * (s_r12 + s_ri0)*m_rr;
		float _s_rim = s_t12 + j12 * (s_t21 + s_ri0 + m_rr * (s_r12 + s_ri0));
		float _s_ri0 = (r21*s_r21 + m_ri0 * _s_rim) / e_ri0;
		float _s_ti0 = ji0 * s_t21 + s_ti0 + ji0 * (s_r12 + s_ri0)*m_rr;
		_s_r0i = (e_r0i > 0.0) ? _s_r0i / e_r0i : 0.0;
		_s_ri0 = (e_ri0 > 0.0) ? _s_ri0 / e_ri0 : 0.0;

		localvLayerEnergyCoeff[i] = (m_r0i > 0.0) ? m_R0i : vec3(0.0, 0.0, 0.0);

		R0i = e_R0i;
		T0i = e_T0i;
		Ri0 = e_Ri0;
		Ti0 = e_Ti0;

		cti = ctt;

		if (i < (NB_VLAYERS - 1))
		{
			s_r0i = _s_r0i;
			s_t0i = _s_t0i;
			s_ri0 = _s_ri0;
			s_ti0 = _s_ti0;

			j0i *= j12;
			ji0 *= j21;
		}
	}

	preLightData.vLayerEnergyCoeff[0] = localvLayerEnergyCoeff[0];
	preLightData.vLayerEnergyCoeff[1] = localvLayerEnergyCoeff[1];
	preLightData.vLayerEnergyCoeff[2] = localvLayerEnergyCoeff[2];

	if (!calledPerLight)
	{
		preLightData.iblPerceptualRoughness[COAT_LOBE_IDX] = bsdfData.coatPerceptualRoughness;
		preLightData.layeredCoatRoughness = max(0.0001, bsdfData.coatRoughness);
	}

	ConvertAnisotropyToClampRoughness(preLightData.iblPerceptualRoughness[BASE_LOBEA_IDX], preLightData.iblAnisotropy[0],
		preLightData.layeredRoughnessT[0], preLightData.layeredRoughnessB[0]);

	if (!calledPerLight)
	{
		s_r12 = RoughnessToLinearVariance(bsdfData.roughnessAT);
		_s_r0m = s_ti0 + j0i * (s_t0i + s_r12 + m_rr * (s_r12 + s_ri0));
		float varianceLobeA = _s_r0m;
		preLightData.iblPerceptualRoughness[BASE_LOBEA_IDX] = LinearVarianceToPerceptualRoughness(_s_r0m);
		preLightData.layeredRoughnessT[0] = max(0.0001, LinearVarianceToRoughness(varianceLobeA));
	}

	preLightData.diffuseEnergy = Ti0;
}

void PreLightData_SetupNormals(BSDFData bsdfData, inout PreLightData preLightData, vec3 V, out vec3 N[NB_NORMALS], out float NdotV[NB_NORMALS])
{
	N[BASE_NORMAL_IDX] = bsdfData.normalWS;
	preLightData.NdotV[BASE_NORMAL_IDX] = dot(N[BASE_NORMAL_IDX], V);
	NdotV[BASE_NORMAL_IDX] = ClampNdotV(preLightData.NdotV[BASE_NORMAL_IDX]);

	N[COAT_NORMAL_IDX] = bsdfData.coatNormalWS;
	preLightData.NdotV[COAT_NORMAL_IDX] = dot(N[COAT_NORMAL_IDX], V);
	NdotV[COAT_NORMAL_IDX] = ClampNdotV(preLightData.NdotV[COAT_NORMAL_IDX]);

	preLightData.geomNdotV = dot(bsdfData.geomNormalWS, V);
}

float PreLightData_GetBaseNdotVForFGD(BSDFData bsdfData, PreLightData preLightData, float NdotV[NB_NORMALS])
{
	float baseLayerNdotV;
	bool isCoatNM = true;
	if (isCoatNM)
	{
		baseLayerNdotV = preLightData.bottomAngleFGD;
	}
	else
	{
		baseLayerNdotV = sqrt(1 + Sq(preLightData.coatIeta)*(Sq(NdotV[0]) - 1));
	}
	return ClampNdotV(baseLayerNdotV);
}

float CalculateEnergyCompensationFromSpecularReflectivity(float specularReflectivity)
{
	float energyCompensation = 1.0 / specularReflectivity - 1.0;
	return energyCompensation;
}

vec3 ApplyEnergyCompensationToSpecularLighting(vec3 specularLighting, vec3 fresnel0, float energyCompensation)
{
	specularLighting *= 1.0 + fresnel0 * energyCompensation;
	return specularLighting;
}

// Ref: Practical multiple scattering compensation for microfacet models.
vec3 GetEnergyCompensationFactor(float specularReflectivity, vec3 fresnel0)
{
	float ec = CalculateEnergyCompensationFromSpecularReflectivity(specularReflectivity);
	return ApplyEnergyCompensationToSpecularLighting(vec3(1.0, 1.0, 1.0), fresnel0, ec);
}

PreLightData GetPreLightData(vec3 V, inout BSDFData bsdfData, sampler2D _PreFGDandDisneyDiffuse)
{
	PreLightData preLightData;
//	ZERO_INITIALIZE(PreLightData, preLightData);

	vec3 N[NB_NORMALS];
	float NdotV[NB_NORMALS];

	PreLightData_SetupNormals(bsdfData, preLightData, V, N, NdotV);

	preLightData.diffuseEnergy = vec3(1.0, 1.0, 1.0);

	vec3 iblN[TOTAL_NB_LOBES]; 
	vec3 iblR[TOTAL_NB_LOBES];
	float specularReflectivity[TOTAL_NB_LOBES];
	float diffuseFGD[BASE_NB_LOBES];

	vec3 f0forCalculatingFGD;

	preLightData.coatIeta = 1.0 / GetCoatEta(bsdfData);

	ComputeAdding(NdotV[COAT_NORMAL_IDX], V, bsdfData, preLightData, false);

	preLightData.baseLayerNdotV = PreLightData_GetBaseNdotVForFGD(bsdfData, preLightData, NdotV);

	float diffuseFGDTmp;

	GetPreIntegratedFGDGGXAndDisneyDiffuse(_PreFGDandDisneyDiffuse,
		NdotV[COAT_NORMAL_IDX],
		preLightData.iblPerceptualRoughness[COAT_LOBE_IDX],
		preLightData.vLayerEnergyCoeff[TOP_VLAYER_IDX],
		preLightData.specularFGD[COAT_LOBE_IDX],
		diffuseFGDTmp,
		specularReflectivity[COAT_LOBE_IDX]);

	f0forCalculatingFGD = preLightData.vLayerEnergyCoeff[BOTTOM_VLAYER_IDX];

	GetPreIntegratedFGDGGXAndDisneyDiffuse(_PreFGDandDisneyDiffuse,
		preLightData.baseLayerNdotV,
		preLightData.iblPerceptualRoughness[BASE_LOBEA_IDX],
		f0forCalculatingFGD,
		preLightData.specularFGD[BASE_LOBEA_IDX],
		diffuseFGD[0],
		specularReflectivity[BASE_LOBEA_IDX]);

	iblN[COAT_LOBE_IDX] = N[COAT_NORMAL_IDX];
	iblN[BASE_LOBEA_IDX] = N[BASE_NORMAL_IDX];

	for (int i = 0; i < TOTAL_NB_LOBES; i++)
	{
		preLightData.iblR[i] = reflect(-V, iblN[i]);
	}

	preLightData.energyCompensationFactor[COAT_LOBE_IDX] = GetEnergyCompensationFactor(specularReflectivity[COAT_LOBE_IDX], Ior2F0(vec3(bsdfData.coatIor)));
	preLightData.energyCompensationFactor[BASE_LOBEA_IDX] = GetEnergyCompensationFactor(specularReflectivity[BASE_LOBEA_IDX], bsdfData.fresnel0);

	preLightData.diffuseFGD = 1.0;

	return preLightData;
}

IndirectLighting EvaluateBSDF_Env( PreLightData preLightData, BSDFData bsdfData, inout float hierarchyWeight, float ldScale)
{
	IndirectLighting lighting;
//	ZERO_INITIALIZE(IndirectLighting, lighting);

	vec3 envLighting = vec3(0.0, 0.0, 0.0);
	float weight = 0.0;


	vec3 R[TOTAL_NB_LOBES];
	float tempWeight[TOTAL_NB_LOBES];
	int i;

	for (i = 0; i < TOTAL_NB_LOBES; ++i)
	{
		tempWeight[i] = 1.0;
	}

	for (i = 0; i < TOTAL_NB_LOBES; ++i)
	{
		vec3 L;
		vec3 normal;

		normal = (NB_NORMALS > 1 && i == COAT_NORMAL_IDX) ? bsdfData.coatNormalWS : bsdfData.normalWS;

		R[i] = preLightData.iblR[i];

		float clampedNdotV = (NB_NORMALS > 1 && i == COAT_NORMAL_IDX) ? ClampNdotV(preLightData.NdotV[COAT_NORMAL_IDX]) : preLightData.baseLayerNdotV; // the later is already clamped

		R[i] = GetSpecularDominantDir(normal, preLightData.iblR[i], preLightData.iblPerceptualRoughness[i], clampedNdotV);
		float roughness = PerceptualRoughnessToRough(preLightData.iblPerceptualRoughness[i]);
		R[i] = mix(R[i], preLightData.iblR[i], clamp(smoothstep(0, 1, roughness * roughness) , 0.0, 1.0) );
		float iblMipLevel;

		iblMipLevel = PerceptualRoughnessToMipmapLevel(preLightData.iblPerceptualRoughness[i]);

		vec4 preLD = textureLod(prefilterMap, R[i],  iblMipLevel);

		tempWeight[i] *= preLD.a;
		L = preLD.rgb * ldScale * preLightData.specularFGD[i];
		L *= preLightData.energyCompensationFactor[i];
		envLighting += L;
	}

	for (i = 0; i < TOTAL_NB_LOBES; ++i)
	{
		weight += tempWeight[i];
	}
	weight /= TOTAL_NB_LOBES;
	weight = tempWeight[1];

	UpdateLightingHierarchyWeights(hierarchyWeight, weight);
	envLighting *= weight;

	lighting.specularReflected = envLighting;

	return lighting;
}

void BSDF_SetupNormalsAndAngles(BSDFData bsdfData, inout PreLightData preLightData,
	inout vec3 L[NB_LV_DIR], inout vec3 V[NB_LV_DIR], out vec3 N[NB_NORMALS], out float NdotL[NDOTLV_SIZE],
	out float NdotH[NB_NORMALS], out float NdotV[NDOTLV_SIZE])
{
	N[BASE_NORMAL_IDX] = bsdfData.normalWS;
	N[COAT_NORMAL_IDX] = bsdfData.coatNormalWS;
	float unclampedNdotV[NB_NORMALS] = preLightData.NdotV;

	NdotL[DNLV_COAT_IDX] = dot(N[COAT_NORMAL_IDX], L[TOP_DIR_IDX]);

	float LdotV = dot(L[TOP_DIR_IDX], V[TOP_DIR_IDX]); 
	float invLenLV = 1.0 / sqrt(max(2.0 * LdotV + 2.0, FLT_EPS));
	NdotH[COAT_NORMAL_IDX] = clamp((NdotL[DNLV_COAT_IDX] + unclampedNdotV[COAT_NORMAL_IDX]) * invLenLV, 0.0, 1.0); 
	NdotV[DNLV_COAT_IDX] = ClampNdotV(unclampedNdotV[COAT_NORMAL_IDX]);
	NdotH[BASE_NORMAL_IDX] = clamp((dot(N[BASE_NORMAL_IDX], L[TOP_DIR_IDX]) + unclampedNdotV[BASE_NORMAL_IDX]) * invLenLV, 0.0, 1.0); 

	NdotL[DNLV_BASE_IDX] = dot(N[BASE_NORMAL_IDX], L[BOTTOM_DIR_IDX]);
	NdotV[DNLV_BASE_IDX] = ClampNdotV(unclampedNdotV[BASE_NORMAL_IDX]);

	preLightData.partLambdaV[COAT_LOBE_IDX] = GetSmithJointGGXPartLambdaV(NdotV[DNLV_COAT_IDX], preLightData.layeredCoatRoughness);
	preLightData.partLambdaV[BASE_LOBEA_IDX] = GetSmithJointGGXPartLambdaV(NdotV[DNLV_BASE_IDX], preLightData.layeredRoughnessT[0]);
}

DirectLighting EvaluateBSDF_Directional(vec3 inV, vec3 inL, PreLightData preLightData, BSDFData bsdfData)
{
	DirectLighting lighting;
//	ZERO_INITIALIZE(DirectLighting, lighting);

	float NdotL[NDOTLV_SIZE];
	float NdotV[NDOTLV_SIZE];
	vec3 L[NB_LV_DIR], V[NB_LV_DIR];
	vec3 N[NB_NORMALS];
	float NdotH[NB_NORMALS];
	L[TOP_DIR_IDX] = L[BOTTOM_DIR_IDX] = inL;
	V[TOP_DIR_IDX] = inV;

	BSDF_SetupNormalsAndAngles(bsdfData, preLightData, L, V, N, NdotL, NdotH, NdotV);

	float baseLayerDV = DV_SmithJointGGX(NdotH[BASE_NORMAL_IDX], NdotL[DNLV_BASE_IDX], NdotV[DNLV_BASE_IDX], preLightData.layeredRoughnessT[0], preLightData.partLambdaV[BASE_LOBEA_IDX]);
	float coatLayerDV = DV_SmithJointGGX(NdotH[COAT_NORMAL_IDX], NdotL[DNLV_COAT_IDX], NdotV[DNLV_COAT_IDX], preLightData.layeredCoatRoughness, preLightData.partLambdaV[COAT_LOBE_IDX]);

	vec3 specularLighting = max(vec3(0.0), NdotL[DNLV_BASE_IDX] * preLightData.vLayerEnergyCoeff[BOTTOM_VLAYER_IDX] * baseLayerDV * preLightData.energyCompensationFactor[BASE_LOBEA_IDX]);
	vec3 coatSpecularLighting = max(vec3(0.0), NdotL[DNLV_COAT_IDX] * preLightData.vLayerEnergyCoeff[TOP_VLAYER_IDX] * coatLayerDV * preLightData.energyCompensationFactor[COAT_LOBE_IDX]);
	specularLighting += coatSpecularLighting;

	vec3 diffuseColor = preLightData.diffuseEnergy * bsdfData.diffuseColor;
	vec3 diffuseLighting = diffuseColor * max(0.0f, NdotL[DNLV_BASE_IDX]) * INV_PI;

	lighting.diffuse = diffuseLighting;
	lighting.specular = specularLighting;

	return lighting;
}

vec3 ConvertF0ForAirInterfaceToF0ForNewTopIor(vec3 fresnel0, float newTopIor)
{
	vec3 ior = F02Ior(min(fresnel0, vec3(1, 1, 1)*0.999)); 
	return Ior2F0(ior, vec3(newTopIor));
}

BSDFData GetBSDFData(vec3 geomNormalWS,
					 vec3 normalWS, 
					 vec3 bentNormalWS, 
					 float perceptualSmoothnessA, 
					 float metallic, 
					 vec3 baseColor, 
					 float dielectricIor, 
					 vec3 coatNormalWS,
					 float coatPerceptualRoughness,
					 //float coatMask,
					 float coatIor,
					 float coatThickness,
					 vec3 coatExtinction
)
{
	BSDFData bsdfData;
//	ZERO_INITIALIZE(BSDFData, bsdfData);

	bsdfData.geomNormalWS = geomNormalWS;
	
	bsdfData.normalWS = normalWS;
	bsdfData.bentNormalWS = bentNormalWS;
	bsdfData.perceptualRoughnessA = PerceptualSmoothnessToPerceptualRoughness(perceptualSmoothnessA);

	bsdfData.diffuseColor = ComputeDiffuseColor(baseColor, metallic);
	bsdfData.fresnel0 = ComputeFresnel0(baseColor, metallic, Ior2F0(vec3(dielectricIor)));

	bsdfData.coatPerceptualRoughness = PerceptualSmoothnessToPerceptualRoughness(coatPerceptualRoughness);
	//bsdfData.coatMask = coatMask;
	bsdfData.coatIor = coatIor;
	bsdfData.coatThickness = coatThickness;
	bsdfData.coatExtinction = coatExtinction;

	bsdfData.coatNormalWS = coatNormalWS;
	bsdfData.fresnel0 = ConvertF0ForAirInterfaceToF0ForNewTopIor(bsdfData.fresnel0, bsdfData.coatIor);
	bsdfData.coatRoughness = PerceptualRoughnessToRough(bsdfData.coatPerceptualRoughness);

	ConvertAnisotropyToRoughness(bsdfData.perceptualRoughnessA, bsdfData.anisotropyA, bsdfData.roughnessAT, bsdfData.roughnessAB);

	return bsdfData;
}

void main()
{
	vec3 lightDir = normalize(light.direction);
	vec3 viewDir = normalize(M_CamPos - WorldPos);
	vec3 refDir = reflect(-viewDir, Normal);

	vec3 halfDir = normalize(lightDir + viewDir);
	float NdotV = clamp(dot(Normal,viewDir), 0.0, 1.0);
	float NdotL = clamp(dot(Normal,lightDir), 0.0, 1.0);
	float NdotH = clamp(dot(Normal,halfDir), 0.0, 1.0);
	float LdotV = clamp(dot(lightDir,viewDir), 0.0, 1.0);
	float LdotH = clamp(dot(lightDir,halfDir), 0.0, 1.0);
	float VdotH = clamp(dot(viewDir, halfDir), 0.0, 1.0);

	float hierarchyWeight = 0.0;
	BSDFData bsdfData = GetBSDFData(geomNormalWS, worldNormal, bentNormalWS, roughness, metallic, albedo, _DelectricIOR,
		coatNormalWS, _CoatPerceptualRoughness, _CoatIOR, _CoatThickness, _CoatExtinction.xyz);
	PreLightData preLightData = GetPreLightData(viewDir, bsdfData, _PreFGDandDisneyDiffuse);
	IndirectLighting indirLighting = EvaluateBSDF_Env(preLightData, bsdfData, hierarchyWeight, _IBLLDScale);
	vec3 diffuse = vec3(0.0);
	vec3 specular = indirLighting.specularReflected;

	DirectLighting dirLighting = EvaluateBSDF_Directional(viewDir, lightDir, preLightData, bsdfData);
	diffuse += dirLighting.diffuse * _LightColor0.rgb * atten;
	specular += dirLighting.specular * _LightColor0.rgb * atten;

	outColor = vec4(diffuse * occlusion + specular * occlusion, 1.0);

	outColor = color / (color + 1.0);
	outColor = pow(color, 1.0 / 2.2);
}
