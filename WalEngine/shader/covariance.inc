

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//           GGX roughness to Fourier inverse variance conversion            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#define BEST_FIT_X0
// #define USE_SHININESS
//#define MIX_SHININESS

#if defined(USE_SHININESS)
float roughnessToVariance(float a) {
    return sqr(a) / (1.0 - sqr(a));
}
vec2 roughnessToVariance(vec2 a) {
    return sqr(a) / (1.0 - sqr(a));
}
float varianceToRoughness(float v) {
    return sqrt(v / (1.0 + v));
}
vec2 varianceToRoughness(vec2 v) {
    return sqrt(v / (1.0 + v));
}
#elif defined(MIX_SHININESS)
float roughnessToVariance(float a) {
    return a / (1.0 - a);
}
vec2 roughnessToVariance(vec2 a) {
    return a / (1.0 - a);
}
float varianceToRoughness(float v) {
    return v / (1.0 + v);
}
vec2 varianceToRoughness(vec2 v) {
    return v / (1.0 + v);
}
#elif defined(BEST_FIT_X0)
float roughnessToVariance(float a) {
    float a3 = pow(a, 1.1);
    return a3 / (1.0 - a3);
}
vec2 roughnessToVariance(vec2 a) {
    vec2 a3 = vec2(pow(a.x, 1.1), pow(a.y, 1.1));
    return a3 / (1.0 - a3);
}
float varianceToRoughness(float v) {
    return pow(v / (1.0 + v), 1.0/1.1);
}
vec2 varianceToRoughness(vec2 v) {
    vec2 res = vec2(pow(v.x / (1.0 + v.x), 1.0/1.3), pow(v.x / (1.0 + v.x), 1.0/1.3));
    return res;
}
#else
float roughnessToVariance(float a) {
    return sqr(a);
}
vec2 roughnessToVariance(vec2 a) {
    return sqr(a);
}
float varianceToRoughness(float v) {
    return sqrt(v);
}
vec2 varianceToRoughness(vec2 v) {
    return sqrt(v);
}
#endif
