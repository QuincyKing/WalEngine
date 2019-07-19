#ifndef LIGHT_DEFINITION_INCLUDED
#define LIGHT_DEFINITION_INCLUDED

struct BSDFData
{
	vec3 diffuseColor;
	vec3 fresnel0;
	vec3 normalWS;
	vec3 geomNormalWS;
	vec3 coatNormalWS;
	vec3 bentNormalWS;
	float perceptualRoughnessA;
	float roughnessAT;
	float roughnessAB;
	float roughnessBT;
	float coatRoughness;
	float coatPerceptualRoughness;
	float coatMask;
	float coatIor;
	float coatThickness;
	vec3 coatExtinction;
	float anisotropyA;
	//TODO: iridescence
	//float iridescenceIor;
	//float iridescenceThickness;
	//float iridescenceMask;
	//float iridescenceCoatFixupTIR;
	//float iridescenceCoatFixupTIRClamp;
	float thickness;
};

struct IndirectLighting
{
	vec3 specularReflected;
	vec3 specularTransmitted;
};

struct DirectLighting
{
	vec3 diffuse;
	vec3 specular;
};

#endif
