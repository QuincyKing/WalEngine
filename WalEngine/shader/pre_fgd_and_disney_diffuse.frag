#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

#define REAL_EPS	4.8828125e-4
#define PI          3.14159265358979323846
#define TWO_PI      6.28318530717958647693

const mat3 k_identity3x3 = mat3(
							1, 0, 0,
                            0, 1, 0,
                            0, 0, 1);

float F_Schlick(float f0, float f90, float u)
{
    float x = 1.0 - u;
    float x2 = x * x;
    float x5 = x * x2 * x2;
    return (f90 - f0) * x5 + f0;  
}

//----------------------------------------
// Diffuse     
//----------------------------------------

// ref: http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
float DisneyDiffuseNoPI(float NdotV, float NdotL, float LdotV, float perceptualRoughness)
{
    float fd90 = 0.5 + (perceptualRoughness + perceptualRoughness * LdotV);

    float lightScatter = F_Schlick(1.0, fd90, NdotL);
    float viewScatter = F_Schlick(1.0, fd90, NdotV);
    return (1.0 / 1.03571) * (lightScatter * viewScatter);
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

vec3 SampleSphereUniform(float u1, float u2)
{
    float phi      = TWO_PI * u2;
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
    L     = SampleHemisphereCosine(u.x, u.y, N);
    NdotL = clamp(dot(N, L), 0.0, 1.0);

    weightOverPdf = 1.0;
}

//-----------------------------------------------
// Specular
//-----------------------------------------------

// Note: V = G / (4 * NdotL * NdotV)
// Ref: http://jcgt.org/published/0003/02/03/paper.pdf
float V_SmithJointGGX(float NdotL, float NdotV, float roughness, float partLambdaV)
{
    float a2 = roughness * roughness;

    // Original formulation:
    // lambda_v = (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5
    // lambda_l = (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5
    // G        = 1 / (1 + lambda_v + lambda_l);

    // Reorder code to be more optimal:
    float lambdaV = NdotL * partLambdaV;
    float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

    // Simplify visibility term: (2.0 * NdotL * NdotV) /  ((4.0 * NdotL * NdotV) * (lambda_v + lambda_l))
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

float SafeDiv(float numer, float denom)
{
    return (numer != denom) ? numer / denom : 1;
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
    float phi      = TWO_PI * u.y;

    vec3 localH = SphericalToCartesian(phi, cosTheta);

    NdotH = cosTheta;

    vec3 localV;

    if (VeqN)
    {
        // localV == localN
        localV = vec3(0.0, 0.0, 1.0);
        VdotH  = NdotH;
    }
    else
    {
        localV = V * transpose(localToWorld);
        VdotH  = clamp(dot(localV, localH), 0.0, 1.0);
    }

    // Compute { localL = reflect(-localV, localH) }
    vec3 localL = -localV + 2.0 * VdotH * localH;
    NdotL = localL.z;

    L = localL * localToWorld;
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

// ref: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

// Ref: Listing 18 in "Moving Frostbite to PBR" + https://knarkowicz.wordpress.com/2014/12/27/analytical-dfg-term-for-ibl/
vec4 IntegrateGGXAndDisneyDiffuseFGD(float NdotV, float roughness, uint sampleCount = 4096)
{
    // Note that our LUT covers the full [0, 1] range.
    // Therefore, we don't really want to clamp NdotV here (else the lerp slope is wrong).
    // However, if NdotV is 0, the integral is 0, so that's not what we want, either.
    // Our runtime NdotV bias is quite large, so we use a smaller one here instead.
    NdotV     = max(NdotV, REAL_EPS);
    vec3 V   = vec3(sqrt(1 - NdotV * NdotV), 0, NdotV);
    vec4 acc = vec4(0.0, 0.0, 0.0, 0.0);

    mat3 localToWorld = k_identity3x3;

    for (uint i = 0; i < sampleCount; ++i)
    {
        vec2 u = Hammersley(i, sampleCount);

        float VdotH;
        float NdotL;
        float weightOverPdf;

        vec3 L; // Unused
        ImportanceSampleGGX(u, V, localToWorld, roughness, NdotV,
                            L, VdotH, NdotL, weightOverPdf);

        if (NdotL > 0.0)
        {
            // Integral{BSDF * <N,L> dw} =
            // Integral{(F0 + (1 - F0) * (1 - <V,H>)^5) * (BSDF / F) * <N,L> dw} =
            // (1 - F0) * Integral{(1 - <V,H>)^5 * (BSDF / F) * <N,L> dw} + F0 * Integral{(BSDF / F) * <N,L> dw}=
            // (1 - F0) * x + F0 * y = lerp(x, y, F0)

            acc.x += weightOverPdf * pow(1 - VdotH, 5);
            acc.y += weightOverPdf;
        }

        // for Disney we still use a Cosine importance sampling, true Disney importance sampling imply a look up table
        ImportanceSampleLambert(u, localToWorld, L, NdotL, weightOverPdf);

        if (NdotL > 0.0)
        {
            float LdotV = dot(L, V);
            float disneyDiffuse = DisneyDiffuseNoPI(NdotV, NdotL, LdotV, sqrt(roughness));

            acc.z += disneyDiffuse * weightOverPdf;
        }
    }

    acc /= sampleCount;

    // Remap from the [0.5, 1.5] to the [0, 1] range.
    acc.z -= 0.5;

    return acc;
}

vec2 Remap01(vec2 x, vec2 rcpLength, vec2 startTimesRcpLength)
{ 
	return clamp(x * rcpLength - startTimesRcpLength, 0.0, 1.0);
}

vec2 RemapHalfTexelCoordTo01(vec2 coord, vec2 size)
{
    const vec2 rcpLen              = size * (vec2(1)/(size - vec2(1)));
    const vec2 startTimesRcpLength = 0.5 * (vec2(1)/(size - vec2(1)));

    return Remap01(coord, rcpLen, startTimesRcpLength);
}

void main()
{
	 vec2 coordLUT = RemapHalfTexelCoordTo01(TexCoords, vec2(64));

    // The FGD texture is parametrized as follows:
    // X = sqrt(dot(N, V))
    // Y = perceptualRoughness
    // These coordinate sampling must match the decoding in GetPreIntegratedDFG in Lit.hlsl,
    // i.e here we use perceptualRoughness, must be the same in shader
    // Note: with this angular parametrization, the LUT is almost perfectly linear,
    // except for the grazing angle when (NdotV -> 0).
    float NdotV = coordLUT.x * coordLUT.x;
    float perceptualRoughness = coordLUT.y;
	FragColor = vec4(IntegrateGGXAndDisneyDiffuseFGD(NdotV, perceptualRoughness * perceptualRoughness).xyz, 1.0);
}