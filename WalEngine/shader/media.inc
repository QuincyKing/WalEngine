#define MEDIA_SHADER

#ifndef COMMON_SHADER
  #error Missing common.shader include in main shader
#endif

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                  Absorption                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

float Absorption(vec3 wi, float opt_depth) {
    float tanwi = length(wi.xy) / wi.z;
    return exp(- tanwi*opt_depth);
}

float Absorption(float opt_depth) {
    return exp(- opt_depth);
}

float Absorption(float d, float at) {
    return exp(- d * at);
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                              HG phase function                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* Evaluate the HG phase function
 */
float HG(in float cosa, in float g) {

    // if(g == 1.0) {
    //     return 1.0;
    // }
    float g2 = sqr(g);
    return (1.0-g2) / ((4.0*PI)*pow(1.0 + g2 - 2.0*g*cosa, 1.5));
}

/* Sample the HG phase function
 */
void HG_Sample(in float g, in vec2 uv, out vec3 w) {
  
    float g2 = g*g;
    float cosT = 1.0 - 2.0*uv.x;
    if(g > 0.0001) {
        float sqrT  = (1.0 - g2) / (1.0 - g + 2.0*g*uv.x);
        cosT = (1.0 + g2 - sqrT*sqrT) / (2.0*g);
    }

    float sinT = sqrt(1.0-cosT*cosT);
    float sinP = sin(2.0*PI*uv.y);
    float cosP = cos(2.0*PI*uv.y);
    w = vec3(sinT*cosP, sinT*sinP, cosT);
}