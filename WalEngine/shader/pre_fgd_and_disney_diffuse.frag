#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

#define REAL_EPS	4.8828125e-4
#define PI          3.14159265358979323846
#define TWO_PI      6.28318530717958647693

#include "bsdf.inc"
#include "sampling.inc"

// Ref: Listing 18 in "Moving Frostbite to PBR" p65 + https://knarkowicz.wordpress.com/2014/12/27/analytical-dfg-term-for-ibl/
vec4 IntegrateGGXAndDisneyDiffuseFGD(float NdotV, float roughness, uint sampleCount = 4096)
{
    // Note that our LUT covers the full [0, 1] range.
    // Therefore, we don't really want to clamp NdotV here (else the mix slope is wrong).
    // However, if NdotV is 0, the integral is 0, so that's not what we want, either.
    // Our runtime NdotV bias is quite large, so we use a smaller one here instead.
    NdotV     = max(NdotV, REAL_EPS);
    vec3 V   = vec3(sqrt(1 - NdotV * NdotV), 0, NdotV);
    vec4 acc = vec4(0.0, 0.0, 0.0, 0.0);

    mat3 localToWorld = mat3(1.0, 0.0, 0.0, 
						     0.0, 1.0, 0.0, 
						     0.0, 1.0, 1.0);

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
            // (1 - F0) * x + F0 * y = mix(x, y, F0)

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