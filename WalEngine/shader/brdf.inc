#define BRDF_SHADER

#ifndef COMMON_SHADER
  #error Missing common.shader include in main shader
#endif

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                              Fresnel Equations                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* Fresnel equations for dielectric/dielectric interfaces.
 * Outputs the amplitude 'r' and phase 'p' coefficients.
 */
void FresnelDielectric(in float ct1, in float n1, in float n2,
                       out vec2 R, out vec2 phi) {

  float st1  = (1.0 - ct1*ct1); // Sinus theta1 'squared'
  float nr  = n1/n2;

  if(sqr(nr)*st1 > 1.0) { // Total reflection

    vec2 R = vec2(1.0, 1.0);
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

/* Return the unpolarized version of the complete dielectric Fresnel equations
 * from `FresnelDielectric` without accounting for wave phase shift.
 */
float FresnelUnpolarized(in float ct1, in float n1, in float n2) {
#ifdef OLD
  vec2 R, phi;
  FresnelDielectric(ct1, n1, n2, R, phi);
  return 0.5*(R.x+R.y);
#else
  float cti = ct1;
  float st2 = (1.0 - sqr(cti));
  float nr  = n1/n2;
  if(nr == 1.0) { return 0.0; }

  if(sqr(nr)*st2 <= 1.0) {
     float ctt = sqrt(1.0 - sqr(nr)*st2) ;
     float tpp = (nr*cti-ctt) / (nr*cti + ctt);
     float tps = (cti-nr*ctt) / (nr*ctt + cti);
     return 0.5 * (tpp*tpp + tps*tps);
  } else {
     return 1.0;
  }
#endif
}

/* Return the unpolarized version of the complete dielectric Fresnel equations
 * from `FresnelConductor` without accounting for wave phase shift.
 */
float FresnelUnpolarized(in float ct1, in float n1, in float n2, in float k) {
  vec2 R, phi;
  FresnelConductor(ct1, n1, n2, k, R, phi);
  return 0.5*(R.x+R.y);
}

/* Schlick Fresnel using the knowledge of F(0d) and F(90d)
 */
vec3 FresnelSchlick(in float ct1, in vec3 F0, in vec3 F90) {
  float x     = 1.0 - ct1;
  float x5    = x * x;
  x5          = x5 * x5 * x;
  return F0 + (F90 - F0) * x5;
}

/* Schlick Fresnel using the knowledge of F(0d) and assuming that the grazing reflectance
 * is a pure white (1,1,1).
 */
vec3 FresnelSchlick(in float ct1, in vec3 F0) {
  return FresnelSchlick(ct1, F0, vec3(1.0, 1.0, 1.0));
}

/* Fresnel interaction of an incoming ray with a planar dielectric/dieletric 
 * interface of normal vector 'z'. The inputs are the cosine to the surface
 * normal 'ct1' and the two IORs. This function outputs 'ct2' the outgoing
 * cosine componnent for tranmission and the reflectance 'R12'.
 */
void Fresnel(in  float ct1, in  float n1, in  float n2,
             out float ct2, out float R12) {
   float st1 = (1.0 - ct1*ct1);
   float n12 = n1/n2;
   if(sqr(n12)*st1 > 1.0) {
      R12 = 1.0;
   }
   ct2 = sqrt(1.0 - sqr(n12) * st1);
   R12 = FresnelUnpolarized(ct1, n1, n2);
}

/* {Convention: to be consistent with the cosine based Fresnel function, the
 *  refracted ray is described with a positive elevation.}
 */
void Fresnel(in  vec3 wi, in  float n1, in  float n2,
             out vec3 wt, out float R12) {

   float ct1 = wi.z;
   R12 = FresnelUnpolarized(ct1, n1, n2);
   if(R12 >= 1.0) {
     return;
   }

   float st1 = (1.0 - ct1*ct1);
   float n12 = n1/n2;
   float st2 = sqr(n12)*st1;     
   float ct2 = sqrt(1.0 - st2);
   wt.xy = n12*wi.xy;
   wt.z  = ct2;
}

/* {Convention: to be consistent with the cosine based Fresnel function, the
 *  refracted ray is described with a positive elevation.}
 */
void Fresnel(in  vec3 wi, in  float n1, in  float n2, in float k2,
             out vec3 wt, out float R12) {

   float ct1 = wi.z;
   R12 = FresnelUnpolarized(ct1, n1, n2, k2);
   if(R12 >= 1.0) {
     return;
   }

   float st1 = (1.0 - ct1*ct1);
   float n12 = n1/n2;
   float st2 = sqr(n12)*st1;     
   float ct2 = sqrt(1.0 - st2);
   wt.xy = n12*wi.xy;
   wt.z  = ct2;
}

void Fresnel(in  vec3  wi, in  vec3  m,
             in  float n1, in  float n2,
             out vec3  wt, out float R12) {

   float ct1 = dot(wi, m);
   if(ct1 < 0.0) {
     R12 = 1.0;
     return;
   }

   R12 = clamp(FresnelUnpolarized(ct1, n1, n2), 0.0, 1.0);
   if(R12 >= 1.0) {
     return;
   }

   float st1 = (1.0 - ct1*ct1);
   float n12 = n1/n2;
   float st2 = sqr(n12)*st1;     
   float ct2 = sqrt(1.0 - st2);
   wt  = n12*wi;
   wt += (ct2 - n12*ct1)*m;
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                       Directional sampling functions                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* Sample the upper hemisphere uniformly
 *
 * Take as input a 2D random vector 'uv' in [0, 1]^2 and output a vector 'w'
 * on the upper hemisphere and the sampling weight.
 */
void SamplingUniformHemisphere(in vec2 uv, out vec3 w, out float pdf) {
    float phi  = 2.0 * PI * uv.x;
    float cost = uv.y;
    float sint = sqrt(max(0.0, 1.0 - cost * cost));
    
    w   = vec3(sint * cos(phi), sint * sin(phi), cost);
    pdf = 1.0 / (2.0*PI);
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             GGX Distribution                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* GGX distribution function 'D'
 */
float GGX_D(float NdotH, float a) {
    if(NdotH < 0.0) {
        return 0.0;
    }

    float a2 = sqr(a);
    return a2 / (PI * sqr( sqr(NdotH) * (a2 - 1.0) + 1.0));
}

/* Smith GGX geometric functions
 */
float GGX_G1(float NdotV, float a) {
    float a2 = sqr(a);
    return 2.0/(1.0 + sqrt(1.0 + a2 * (1.0-sqr(NdotV)) / sqr(NdotV) ));
}

/* Smith uncorrelated shadowing/masking function.
 */
float GGX_G(float NdotL, float NdotV, float a) {
    return GGX_G1(NdotL, a) * GGX_G1(NdotV, a);
}

/* GGX formula without Fresnel for mirror-like facets.
 *
 * Note: for performance reason, do not use this code. This is a snippet for
 *       copy/paste purposes.
 */
float GGX(vec3 wi, vec3 wo, vec3 n, float a) {
    vec3  h     = normalize(wi+wo);
    float NdotH = dot(n, h);
    float NdotL = dot(n, wi);
    float NdotV = dot(n, wo);

    return GGX_D(NdotH, a)*GGX_G(NdotL, NdotV, a) / abs(4.0*NdotL*NdotV);
}

/* GGX transmission.
 *
 *  'n12' is the IOR ratio 'n12 = n_1 / n_2'
 */
float GGX_T(vec3 wi, vec3 wo, vec3 n, float n1, float n2, float a) {
    float n12   = n1 / n2;
    vec3  h;
    if(n1 < n2) {
      h = -normalize(n12*wi+wo);
    } else {
      h = normalize(n12*wi+wo);
    }

    float NdotH = dot(n, h);
    float NdotL = dot(n, wi);
    float NdotV = dot(n, wo);
    float HdotL = dot(h, wi);
    float HdotV = dot(h, wo);

    if(HdotL < 0.0 || HdotV > 0.0) { return 0.0;}

    float F     =  FresnelUnpolarized(HdotL, n1, n2);
    float fact  = abs((HdotL*HdotV) / (NdotL*NdotV));
    return fact * ((1.0 - F) * GGX_D(NdotH, a)*GGX_G(NdotL, NdotV, a)) / sqr(n12*HdotL + HdotV);
}

/* Importance sampling the GGX distribution using the visible Normal Distribution Function (vNDF)
 */
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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             vMF Distribution                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

float vMF_A3(float km) {
   return coth(km) - 1.0/km;
}

float vMF_invA3(float y) {
  return (y - 2.0*y*(1.0-y)) / (1.0-y);
}

float vMF_RoughnessToKm(float roughness) {
  float sqr_roughness = sqr(roughness); 
  return 4.0 * ( 1.0 - sqr_roughness) / sqr_roughness;
}

/* von Mises-Fisher distribution
 */
float vMF_D(vec3 x, vec3 mu, float km) {
  float dotXM = dot(x, mu);
  // Hack? changed 0.5 to 0.25
  return 0.25* INV_PI * km / (exp(km * (1.0 - dotXM)) - exp(- km * (1.0 + dotXM)));
}

/* Geometric terms using the vMF distribution of microfacets
 * Use the approximate form of Guo et al. [2016]
 */
float vMF_G1(vec3 o, vec3 m, vec3 mu, float km) {
  float a = 0.25*sqr(vMF_A3(km)+1.0);
  float b = pow(vMF_A3(km), 0.3333);
  float dotOMu = dot(o, mu);
  return a * cos(b * acos(dotOMu));
}

float vMF_G(vec3 wi, vec3 wo, vec3 m, vec3 mu, float km) {
  return vMF_G1(wo, m, mu, km) * vMF_G1(wi, m, mu, km);
}

/* von Mises-Fisher BRDF using a vMF distribution for the microfacets
 * Assume that 'n' is [0,0,1].
 */
float vMF_BRDF(vec3 wi, vec3 wo, float km) {
  vec3  h = normalize(wi + wo);
  float NdotH = h.z;
  float NdotL = wi.z;
  float NdotV = wo.z;  
  float D = vMF_D(h, vec3(0.0, 0.0, 1.0), km);
  float G = 1.0;//vMF_G(wi, wo, h, vec3(0.0, 0.0, 1.0), km);
  return (D*G) / abs(4.0*NdotL*NdotV);
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                            Lambert material                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void Lambert_Sample(in vec3 wi, in vec3 n, in vec2 uv, out vec3 wo, out float pdf) {  
  // Generate a random sample on the hemisphere
  vec3 tmp;  
  SamplingUniformHemisphere(uv, tmp, pdf);

  // Use the local frame spawned by 'n' to provide the correct direction
  vec3 t, b;
  FrameFrisvad(n, t, b);
  wo  = t*tmp.x + b*tmp.y + n*tmp.z;
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                               Clear Coat                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* Snell transmission for an incoming vector and an interface consisting
 * of a ratio of IORs 'eta= n1/n2' and a normal 'n'.
 */
vec3 SnellRefraction(in vec3 wi, in vec3 n, in float eta) {
  return refract(wi, n, eta);
}

/* Evaluate the reflection componnent of a Clear Coat of IOR n2 below a medium
 * of IOR n2. Output the refracted directions of wi and wo.
 */
float ClearCoat(in vec3 wi, in vec3 wo, in float n1, in float n2,
                out vec3 wi_in, out vec3 wo_in) {

  vec3  h  = normalize(wi+wo);
  vec3  n  = vec3(0.0,0.0,1.0);
  float hl = dot(h, wi);

  float eta = n1/n2;
  wi_in = SnellRefraction(wi, n, eta);
  wo_in = SnellRefraction(wo, n, eta);

  /* TODO replace by a continuous distribution to match wi = reflect(wo) */
  return (h.z > 0.99999) ? FresnelUnpolarized(hl, n1, n2) : 0.0;
}
