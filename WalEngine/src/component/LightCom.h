#pragma once

#include "../render/shader.h"
#include "../core/Quaternion.h"
#include "../util/Util.h"
#include "../core/Component.h"

#define COLOR_DEPTH 256

class BaseLightCom : public Component
{
public:
	BaseLightCom(const glm::vec3& color, float intensity) :
		mColor(color),
		mIntensity(intensity)
	{}
	//mShadowInfo(ShadowInfo()) {}

	//virtual ShadowCameraTransform CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const;
	//virtual void AddToEngine(CoreEngine* engine) const;

	inline const glm::vec3& get_color()        const { return mColor; }
	inline const float get_intensity()        const { return mIntensity; }
	inline void set_color(const glm::vec3& color) { mColor = color; }
	inline void set_intensity(float intensity) { mIntensity = intensity; }
	//inline const ShadowInfo& get_shadow_info() const { return mShadowInfo; }

protected:
	//inline void SetShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }

private:
	glm::vec3    mColor;
	float        mIntensity;
	//ShadowInfo  mShadowInfo;
};

class DirectionalLightCom : public BaseLightCom
{
public:
	DirectionalLightCom(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, int shadowMapSizeAsPowerOf2 = 0,
		float shadowArea = 80.0f, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f) :
		BaseLightCom(color, intensity),
		mHalfShadowArea(shadowArea / 2.0f)
	{
		/*if (shadowMapSizeAsPowerOf2 != 0)
		{
		SetShadowInfo(ShadowInfo(glm::orthographic(-m_halfShadowArea, m_halfShadowArea, -m_halfShadowArea,
		m_halfShadowArea, -m_halfShadowArea, m_halfShadowArea),
		true, shadowMapSizeAsPowerOf2, shadowSoftness, lightBleedReductionAmount, minVariance));
		}*/
	};

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
	inline void set_constant(float constant) { mConstant = constant; }
	inline void set_linear(float linear) { mLinear = linear; }
	inline void set_exponent(float exp) { mExponent = exp; }

private:
	float mConstant;
	float mLinear;
	float mExponent;
};

class PointLightCom : public BaseLightCom
{
public:
	PointLightCom(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation()) :
		BaseLightCom(color, intensity),
		mAttenuation(atten)
	{
		float a = mAttenuation.get_exponent();
		float b = mAttenuation.get_linear();
		float c = mAttenuation.get_constant() - COLOR_DEPTH * intensity * fmax(fmax(color.r, color.g), color.b);

		mRange = (-b + sqrtf(b*b - 4 * a*c)) / (2 * a);
	}

	inline const Attenuation& get_attenuation() const { return mAttenuation; }
	inline const float get_range()              const { return mRange; }
	inline void set_constant(float constant) { mAttenuation.set_constant(constant); }
	inline void set_linear(float linear) { mAttenuation.set_linear(linear); }
	inline void set_exponent(float exp) { mAttenuation.set_exponent(exp); }
	inline void set_range(float range) { mRange = range; }

private:
	Attenuation mAttenuation;
	float mRange;
};

class SpotLightCom : public PointLightCom
{
public:
	SpotLightCom(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation(), float viewAngle = TO_RADIANS(170.0f),
		int shadowMapSizeAsPowerOf2 = 0, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f) :
		PointLightCom(color, intensity, atten),
		mCutoff(cos(viewAngle / 2))
	{
		/*if (shadowMapSizeAsPowerOf2 != 0)
		{
			SetShadowInfo(ShadowInfo(Matrix4f().InitPerspective(viewAngle, 1.0, 0.1, GetRange()), false, shadowMapSizeAsPowerOf2,
				shadowSoftness, lightBleedReductionAmount, minVariance));
		}*/
	}

	inline float get_cutoff() const { return mCutoff; }
	inline void set_cutoff(float cutoff) { mCutoff = cutoff; }

private:
	float mCutoff;
};