#version 430

struct PointLightData
{
    vec4 color;
    vec4 position;
};

struct ListHead
{
	uvec4 StartAndCount;
};

struct ListNode
{
	uvec4 LightIndexAndNext;
};

layout(std140, binding = 0) readonly buffer HeadBuffer {
	ListHead data[];
} headbuffer;

layout(std140, binding = 1) readonly buffer NodeBuffer {
	ListNode data[];
} nodebuffer;

layout(std140, binding = 2) readonly buffer LightBuffer {
	PointLightData data[];
} lightbuffer;

uniform sampler2D M_sampler;
uniform int numTilesX;
uniform vec3 M_CamPos;

in vec3 Pos;
in vec3 Normal;
in vec2 Tex;

out vec4 my_FragColor0;

float Attenuate(vec3 ldir, float radius)
{
	float atten = dot(ldir, ldir) / radius;

	atten = 1.0 / (atten * 15.0 + 1.0);
	atten = (atten - 0.0625) * 1.066666;

	return clamp(atten, 0.0, 1.0);
}

layout(early_fragment_tests) in;
void main()
{
	ivec2	loc = ivec2(gl_FragCoord.xy);
	ivec2	tileID = loc / ivec2(16, 16);
	int		index = tileID.y * numTilesX + tileID.x;

	vec4 base = texture(M_sampler, Tex);
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 lightcolor;
	vec4 lightpos;

	vec3 irrad;
	vec3 l;
	vec3 h;
	vec3 n = normalize(Normal);
	vec3 v = normalize(M_CamPos - Pos);

	float diff;
	float spec;
	float atten;
	float radius;

	uint start = headbuffer.data[index].StartAndCount.x;
	uint count = headbuffer.data[index].StartAndCount.y;
	uint nodeID = start;
	uint lightID;

	color.rgb += base.rgb * vec3(0.6, 0.3, 0.1);

	for( uint i = 0; i < count; ++i )
	{
		lightID = nodebuffer.data[nodeID].LightIndexAndNext.x;
		nodeID = nodebuffer.data[nodeID].LightIndexAndNext.y;

		lightcolor = lightbuffer.data[lightID].color;
		lightpos = vec4(lightbuffer.data[lightID].position.xyz, 1.0);
		radius = lightbuffer.data[lightID].position.w;

		l = lightpos.xyz - Pos.xyz;
		atten = Attenuate(l, radius);

		l = normalize(l);
		h = normalize(l + v);

		diff = max(dot(l, n), 0);
		spec = pow(max(dot(h, n), 0), 80.0);
		
		irrad = lightcolor.rgb * (base.rgb * diff + vec3(spec)) * atten * 4;
		color.rgb += irrad;
	}

	my_FragColor0 = color;
}
