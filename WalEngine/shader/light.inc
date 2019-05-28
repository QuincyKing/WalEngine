struct DirectionalLight
{
    vec3 color;
    float intensity;
    vec3 direction;
};

struct PointLight
{
	float intensity;
    vec3 color;
    float constant;
    float linear;
    float exponent;
    vec3 position;
    float range;
};

struct SpotLight
{
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float exponent;
    vec3 position;
    float range;
    vec3 direction;
    float cutoff;
};