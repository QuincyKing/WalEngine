#version 430 core
#define PI     3.14159265358979323846
precision highp float;
out vec4 FragColor;

uniform float  R_Scale;
uniform float  R_Gamma;

uniform float  R_Eta1;
uniform float  R_Eta2;

uniform float  R_KappaR;
uniform float  R_KappaG;
uniform float  R_KappaB;

in vec2 TexCoords;
in vec2 Pos;

float sqr(float x) 
{
    return x*x;
}
vec2 sqr(vec2 x)
{
    return x*x;
}

int mod1(int n, int N) {
    return n - N*(n / N);
}

vec3 sqr(vec3 x) 
{
    return x*x;
}
vec3 Gamma(vec3 R) 
{
    return exp(R_Gamma * log(R_Scale * R));
}

vec2 QMC_Additive_2D(int n) 
{
    return mod(vec2(0.5545497, 0.308517) * float(n), vec2(1.0, 1.0));
}

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

void GGX_Sample_VNDF(in vec3 wi, in float alpha, in vec2 uv,
                     out vec3 m, out float pdf) {
    // stretch view
    vec3 V = normalize(vec3(alpha * wi.x, alpha * wi.y, wi.z));
    // orthonormal basis
    vec3 T1 = (V.z < 0.9999) ? normalize(cross(V, vec3(0,0,1))) : vec3(1,0,0);
    vec3 T2 = cross(T1, V);
    // sample point with polar coordinates (r, phi)
    float a = 1.0 / (1.0 + V.z);
    float r = sqrt(uv.x);
    float phi = (uv.y<a) ? uv.y/a * PI : PI + (uv.y-a)/(1.0-a) * PI;
    float P1 = r*cos(phi);
    float P2 = r*sin(phi)*((uv.y<a) ? 1.0 : V.z);
    // compute normal
    vec3 N = P1*T1 + P2*T2 + sqrt(max(0.0, 1.0 - P1*P1 - P2*P2))*V;
    // unstretch
    m = normalize(vec3(alpha*N.x, alpha*N.y, max(0.0, N.z)));
}

/* Fresnel equations for dielectric/dielectric interfaces.
 * Outputs the amplitude 'r' and phase 'p' coefficients.
 */
void FresnelDielectric(in float ct1, in float n1, in float n2,
                       out vec2 R, out vec2 phi) {
  float st1  = (1.0 - ct1*ct1); // Sinus theta1 'squared'
  float nr  = n1/n2;

  if(sqr(nr)*st1 > 1.0) { // Total reflection

    R = vec2(1.0, 1.0);
    phi = 2.0 * atan(vec2(- sqr(nr) *  sqrt(st1 - 1.0/sqr(nr)) / ct1,
                        - sqrt(st1 - 1.0/sqr(nr)) / ct1));
  } else {   // Transmission & Reflection

    float ct2 = sqrt(1.0 - sqr(nr) * st1);
    vec2 r = vec2((n2*ct1 - n1*ct2) / (n2*ct1 + n1*ct2),
                 (n1*ct1 - n2*ct2) / (n1*ct1 + n2*ct2));
    phi.x = (r.x < 0.0) ? PI : 0.0;
    phi.y = (r.y < 0.0) ? PI : 0.0;
    R = sqr(r);
  }
}

/* Fresnel equations for dielectric/conductor interfaces.
 * Outputs the amplitude 'r' and phase 'p' coefficients.
 */
void FresnelConductor(in float ct1, in float n1, in float n2, in float k,
                       out vec2 R, out vec2 phi) {

    if (k<=0.0) {
		FresnelDielectric(ct1, n1, n2, R, phi);
		return;
    }

    float A = sqr(n2) * (1.0-sqr(k)) - sqr(n1) * (1.0-sqr(ct1));
    float B = sqrt( sqr(A) + sqr(2.0*sqr(n2)*k) );
    float U = sqrt((A+B)/2.0);
    float V = sqrt((B-A)/2.0);

    R.y = (sqr(n1*ct1 - U) + sqr(V)) / (sqr(n1*ct1 + U) + sqr(V));
    phi.y = atan( 2.0*n1 * V*ct1, sqr(U)+sqr(V)-sqr(n1*ct1) ) + PI;

    R.x = ( sqr(sqr(n2)*(1.0-sqr(k))*ct1 - n1*U) + sqr(2.0*sqr(n2)*k*ct1 - n1*V) ) 
            / ( sqr(sqr(n2)*(1.0-sqr(k))*ct1 + n1*U) + sqr(2.0*sqr(n2)*k*ct1 + n1*V) );
    phi.x = atan( 2.0*n1*sqr(n2)*ct1 * (2.0*k*U - (1.0-sqr(k))*V), sqr(sqr(n2)*(1.0+sqr(k))*ct1) - sqr(n1)*(sqr(U)+sqr(V)) );
}

float FresnelUnpolarized(in float ct1, in float n1, in float n2, in float k) {
  vec2 R, phi;
  FresnelConductor(ct1, n1, n2, k, R, phi);
  return 0.5*(R.x+R.y);
}

void main(void) 
{
	FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    // Remove issues with the roughness
	const int NB_SAMPLES = 1024;
	const int NB_PER_RUN = 1024;
    // Get the omega_o direction from the vPos variable
    vec3 wo;
    wo.z = cos(0.25*PI*(1.0+Pos.x));
    wo.x = sqrt(1.0 - sqr(wo.z));
    wo.y = 0.0;

    for(int i=0; i<NB_PER_RUN; i++) 
	{
        /* Radom number
        */
        int idx =  mod1(i, NB_SAMPLES);
		
        vec2 rnd = QMC_Additive_2D(i);

        /* Generate a sample from the GGX distribution
        */
        vec3 m; float pdf;
        GGX_Sample_VNDF(wo, max(0.5*(Pos.y + 1.0), 0.01), rnd.xy, m, pdf);
        //m.z  = rnd.x;
        //m.xy = sqrt(1.0 - sqr(m.z)) * vec2(cos(2.0*PI*rnd.y), sin(2.0*PI*rnd.y));
        //
        vec3 ws = -reflect(wo, m);

        /* Evaluate radiance */
        vec3 value = vec3(0,0,0);
        if(ws.z > 0.0) {
            /* Evaluate the Fresnel term */
            vec3 R;
			R.x = FresnelUnpolarized(dot(wo, m), R_Eta1, R_Eta2, R_KappaR);
            R.y = FresnelUnpolarized(dot(wo, m), R_Eta1, R_Eta2, R_KappaG);
            R.z = FresnelUnpolarized(dot(wo, m), R_Eta1, R_Eta2, R_KappaB);
            FragColor.xyz += R;
        }
    }

    FragColor.xyz = Gamma(FragColor.xyz / float(NB_PER_RUN));
    FragColor.w = 1.0;

}
