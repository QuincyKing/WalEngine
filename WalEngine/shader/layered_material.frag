#version 430 core
out vec4 FragColor;
in vec2 Tex;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D M_albedoMap;
uniform sampler2D M_normalMap;
uniform sampler2D M_metallicMap;
uniform sampler2D M_roughnessMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

#include <light.inc>

uniform PointLight R_dir;

uniform vec3 M_CamPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(M_normalMap, Tex).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(Tex);
    vec2 st2 = dFdy(Tex);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

float m_depths[2] = float[2] ( 0.0, 0.0 );
vec3 m_etas[3] = vec3[3]( vec3(1.0), vec3(1.49), vec3(1.0) );
vec3 m_kappas[3] = vec3[3]( vec3(0.0), vec3(0.0), vec3(1.0, 0.1, 0.1) );
//TODO
float m_alphas[3] = float[3] (0.1, 0.1, 0.1);
int nb_layers = 2;
vec3 m_sigma_a[2] = vec3[2] ( vec3(0.0f, 1.0, 0.0), vec3(0.0f, 0.4f, 0.7f) );
vec3 m_sigma_s[2] = vec3[2] ( vec3(0.0f), vec3(0.0f) );

const bool m_useTIR = false;

float average(vec3 t)
{
    return (t.r + t.g + t.b) * (1.0/ 3.0);
}

#define USE_BEST_FIT
float roughnessToVariance(float a)
{
#ifdef USE_BEST_FIT
   a = clamp(a, 0.0, 0.9999);
   float a3 = pow(a, 1.1);
   return a3 / (1.0f - a3);
#else
   return a / (1.0f-a);
#endif
}
float varianceToRoughness(float v) 
{
#ifdef USE_BEST_FIT
   return pow(v / (1.0f + v), 1.0f/1.1f);
#else
   return v / (1.0f+v);
#endif
}

bool isZero(vec3 s) 
{
    return s.r - 0.0f > 0.00001f || s.g - 0.0 > 0.00001f || s.b - 0.0 > 0.00001f ? false : true;
}

void FGD(float cti, float temp_alpha, vec3 R12, vec3 T12)
{
    vec2 brdf  = texture(brdfLUT, vec2(max(cti, 0.0), temp_alpha)).rg;
    vec3 F = fresnelSchlickRoughness(cti, vec3(0.04), temp_alpha);
    R12 = (F * brdf.x + brdf.y);
    T12 = vec3(1) - R12;
}

void computeAddingDoubling(float _cti, out vec3 coeffs[3], out float alphas[3], out int nb_valid)
{
    float cti  = _cti;
    vec3 R0i = vec3(0.0f);
    vec3 Ri0 = vec3(0.0f);
    vec3 T0i = vec3(1.0f);
    vec3 Ti0 = vec3(1.0f);
    float s_r0i = 0.0f;
    float s_ri0=0.0f;
    float s_t0i=0.0f;
    float s_ti0=0.0f;
    float j0i=1.0f;
    float ji0=1.0f;

    // Iterate over the layers
    for(int i=0; i<nb_layers; ++i)
	{
        // Extract layer data
        vec3 eta_1   = m_etas[i];
        vec3 eta_2   = m_etas[i+1];
        vec3 kappa_2 = m_kappas[i+1];
        vec3 eta     = eta_2 / eta_1;
        vec3 kappa   = kappa_2 / eta_1;
        float alpha  = m_alphas[i];
        float n12    = average(eta);
        float depth  = m_depths[i];

        vec3 R12, T12, R21, T21;
        float s_r12=0.0f, s_r21=0.0f, s_t12=0.0f, s_t21=0.0f, j12=1.0f, j21=1.0f, ctt;

        //calculate 2nd layer and 3rd layer statistics 
        //Medium
        if(depth > 0.0f)
		{
            // Mean doesn't change with volumes
            ctt = cti;

            // Evaluate transmittance
            const vec3 sigma_t = m_sigma_a[i] + m_sigma_s[i];

            // Volume Scattering + Volume Absorption
            T12 = (vec3(1.0f) + m_sigma_s[i]*depth/ctt) * exp(- (depth/ctt) * sigma_t);
            T21 = T12;
            // ????
            R12 = vec3(0.0f);
            R21 = vec3(0.0f);

            // Fetch precomputed variance for HG phase function
            // alpha = sigma_g 
            s_t12 = alpha;
            s_t21 = alpha;

        } 
        //Boundary
        else 
        {
            // Evaluate off-specular transmission
            float sti = sqrt(1.0f - cti*cti);
            float stt = sti / n12;
            if(stt <= 1.0f) ctt = sqrt(1.0f - stt*stt);
            else ctt = -1.0f;


            // Ray is not block by conducting interface or total reflection
            const bool has_transmissive = ctt > 0.0f && isZero(kappa);

            // Evaluate interface variance term
            s_r12 = roughnessToVariance(alpha);
            s_r21 = s_r12;

            // For dielectric interfaces, evaluate the transmissive roughnesses
            if(has_transmissive) 
            {
                const float _ctt = 1.0f;
                const float _cti = 1.0f;
                                           
                // ??????
                s_t12 = roughnessToVariance(alpha * 0.5f * abs(_ctt*n12 - _cti)/(_ctt*n12));
                s_t21 = roughnessToVariance(alpha * 0.5f * abs(_cti/n12 - _ctt)/(_cti/n12));
                    j12 = (ctt/cti) * n12; // Scale due to the interface
                    j21 = (cti/ctt) / n12;
            }

            // Evaluate FGD using a modified roughness accounting for top layers
            float temp_alpha = varianceToRoughness(s_t0i + s_r12);

            // Evaluate r12, r21, t12, t21
            FGD(cti, temp_alpha, R12, T12);
            // Reflection / Refraction by a rough interface
            if(has_transmissive) 
            {
                R21 = R12;
                T21 = T12; // We don't need the IOR scaling since we are
                T12 = T12; // computing reflectance only here.
            } 
            else
            {
                R21 = vec3(0.0f);
                T21 = vec3(0.0f);
                T12 = vec3(0.0f);
            }

            // Evaluate TIR using the decoupling approximation
            if(i > 0 && m_useTIR)
            {
          //       vec3 eta_0   = (i==0) ? m_etas[0] : m_etas[i-1];
          //       float n10    = average(eta_0/eta_1);

          //       const float _TIR  = m_TIR(cti, temp_alpha, n10);
          //       Ri0 += (1.0f-_TIR) * Ti0;
		        // Ri0.r = min(Ri0.r, 1.0f); Ri0.g = min(Ri0.g, 1.0f); Ri0.b = min(Ri0.b, 1.0f);
          //       Ti0 *= _TIR;
            }
        }

        // Multiple scattering forms
        const vec3 denom = (vec3(1.0f) - Ri0*R12);
        //TODO isZero()
        const vec3 m_R0i = (average(denom) <= 0.0f)? vec3(0.0f) : (T0i*R12*Ti0) / denom;
        const vec3 m_Ri0 = (average(denom) <= 0.0f)? vec3(0.0f) : (T21*Ri0*T12) / denom;
        const vec3 m_Rr  = (average(denom) <= 0.0f)? vec3(0.0f) : (Ri0*R12) / denom;
            
        // Evaluate the adding operator on the energy
        const vec3 e_R0i = R0i + m_R0i;
        const vec3 e_T0i = (T0i*T12) / denom;
        const vec3 e_Ri0 = R21 + m_Ri0;
        const vec3 e_Ti0 = (T21*Ti0) / denom;

        // Scalar forms for the spectral quantities
        const float r0i   = average(R0i);
        const float e_r0i = average(e_R0i);
        const float e_ri0 = average(e_Ri0);
        const float m_r0i = average(m_R0i);
        const float m_ri0 = average(m_Ri0);
        const float m_rr  = average(m_Rr);
        const float r21   = average(R21);

        // Evaluate the adding operator on the normalized variance
        float _s_r0i = (r0i*s_r0i + m_r0i*(s_ti0 + j0i*(s_t0i + s_r12 + m_rr*(s_r12+s_ri0)))) ;// e_r0i;
        float _s_t0i = j12*s_t0i + s_t12 + j12*(s_r12 + s_ri0)*m_rr;
        float _s_ri0 = (r21*s_r21 + m_ri0*(s_t12 + j12*(s_t21 + s_ri0 + m_rr*(s_r12+s_ri0)))) ;// e_ri0;
        float _s_ti0 = ji0*s_t21 + s_ti0 + ji0*(s_r12 + s_ri0)*m_rr;
        _s_r0i = (e_r0i > 0.0f) ? _s_r0i/e_r0i : 0.0f;
        _s_ri0 = (e_ri0 > 0.0f) ? _s_ri0/e_ri0 : 0.0f;

        // Store the coefficient and variance
        if(m_r0i > 0.0f)
         {
            coeffs[i] = m_R0i;
            alphas[i] = varianceToRoughness(s_ti0 + j0i*(s_t0i + s_r12 + m_rr*(s_r12+s_ri0)));
        } 
        else 
        {
            coeffs[i] = vec3(0.0f);
            alphas[i] = 0.0f;
        }

        // Update energy
        R0i = e_R0i;
        T0i = e_T0i;
        Ri0 = e_Ri0;
        Ti0 = e_Ti0;

        // Update mean
        cti = ctt;

        // Update variance
        s_r0i = _s_r0i;
        s_t0i = _s_t0i;
        s_ri0 = _s_ri0;
        s_ti0 = _s_ti0;

        // Update jacobian
        j0i *= j12;
        ji0 *= j21;

        // Escape if a conductor is present
        if(average(kappa) > 0.0f) 
        {
            nb_valid = i+1;
            return;
        }
    }

    nb_valid = nb_layers;
}


void main()
{		
	computeAddingDoubling(float _cti, out vec3 coeffs[3], out float alphas[3], out int nb_valid)
    vec3 albedo = pow(texture(M_albedoMap, Tex).rgb, vec3(2.2));
    float metallic = texture(M_metallicMap, Tex).r;
    float roughness = texture(M_roughnessMap, Tex).r;
     
    vec3 N = getNormalFromMap();
    vec3 V = normalize(M_CamPos - WorldPos);
    vec3 R = reflect(-V, N); 

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    vec3 L = normalize(R_dir.position - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(R_dir.position - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = R_dir.color * attenuation;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);    
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	                
            
    float NdotL = max(dot(N, L), 0.0);        

    Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular);
    
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);
}
