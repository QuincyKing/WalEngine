#pragma once

#include "shader.h"
#include "../core/Quaternion.h"
#include "../util/Util.h"

class BaseLight
{
public:
	BaseLight(const glm::vec3& color, float intensity) :
		mColor(color),
		mIntensity(intensity)
	{}
		//mShadowInfo(ShadowInfo()) {}

	//virtual ShadowCameraTransform CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const;
	//virtual void AddToEngine(CoreEngine* engine) const;

	inline const glm::vec3& get_color()        const { return mColor; }
	inline const float get_intensity()        const { return mIntensity; }
	//inline const ShadowInfo& get_shadow_info() const { return mShadowInfo; }

protected:
	//inline void SetShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }

private:
	glm::vec3    mColor;
	float       mIntensity;
	//ShadowInfo  mShadowInfo;
};

class DirectionalLight : public BaseLight
{
public:
	DirectionalLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, int shadowMapSizeAsPowerOf2 = 0,
		float shadowArea = 80.0f, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f);

	//virtual ShadowCameraTransform CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const;

	inline float get_half_shadow_area() const { return mHalfShadowArea; }

private:
	float mHalfShadowArea;
};

class Attenuation
{
public:
	Attenuation(float constant = 0, float linear = 0, float exponent = 1) :
		mConstant(constant),
		mLinear(linear),
		mExponent(exponent) {}

	inline float get_constant() const { return mConstant; }
	inline float get_linear()   const { return mLinear; }
	inline float get_exponent() const { return mExponent; }

private:
	float mConstant;
	float mLinear;
	float mExponent;
};

class PointLight : public BaseLight
{
public:
	PointLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation(),
		const Shader& shader = Shader("forward-point"));

	inline const Attenuation& get_attenuation() const { return mAttenuation; }
	inline const float get_range()              const { return mRange; }

private:
	Attenuation mAttenuation;
	float mRange;
};

class SpotLight : public PointLight
{
public:
	SpotLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation(), float viewAngle = TO_RADIANS(170.0f),
		int shadowMapSizeAsPowerOf2 = 0, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f);

	inline float get_cutoff() const { return mCutoff; }

private:
	float mCutoff;
};