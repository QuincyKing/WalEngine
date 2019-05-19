#pragma once

#include "shader.h"
#include "../core/Quaternion.h"
#include "../util/Util.h"

class CoreEngine;

class ShadowCameraTransform
{
public:
	ShadowCameraTransform(const glm::vec3& pos, const Quaternion& rot) :
		m_pos(pos),
		m_rot(rot) {}

	inline const glm::vec3& GetPos()   const { return m_pos; }
	inline const Quaternion& GetRot() const { return m_rot; }

private:
	glm::vec3 m_pos;
	Quaternion m_rot;
};

class ShadowInfo
{
public:
	ShadowInfo(const glm::mat4& projection = glm::mat4(), bool flipFaces = false, int shadowMapSizeAsPowerOf2 = 0, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f) :
		m_projection(projection),
		m_flipFaces(flipFaces),
		m_shadowMapSizeAsPowerOf2(shadowMapSizeAsPowerOf2),
		m_shadowSoftness(shadowSoftness),
		m_lightBleedReductionAmount(lightBleedReductionAmount),
		m_minVariance(minVariance) {}

	inline const glm::mat4& GetProjection()      const { return m_projection; }
	inline bool GetFlipFaces()                  const { return m_flipFaces; }
	inline int GetShadowMapSizeAsPowerOf2()     const { return m_shadowMapSizeAsPowerOf2; }
	inline float GetShadowSoftness()            const { return m_shadowSoftness; }
	inline float GetMinVariance()               const { return m_minVariance; }
	inline float GetLightBleedReductionAmount() const { return m_lightBleedReductionAmount; }

private:
	glm::mat4 m_projection;
	bool m_flipFaces;
	int m_shadowMapSizeAsPowerOf2;
	float m_shadowSoftness;
	float m_lightBleedReductionAmount;
	float m_minVariance;
};

class BaseLight
{
public:
	BaseLight(const glm::vec3& color, float intensity, const Shader& shader) :
		m_color(color),
		m_intensity(intensity),
		m_shader(shader),
		m_shadowInfo(ShadowInfo()) {}

	virtual ShadowCameraTransform CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const;
	virtual void AddToEngine(CoreEngine* engine) const;

	inline const glm::vec3& GetColor()        const { return m_color; }
	inline const float GetIntensity()        const { return m_intensity; }
	inline const Shader& GetShader()         const { return m_shader; }
	inline const ShadowInfo& GetShadowInfo() const { return m_shadowInfo; }

protected:
	inline void SetShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }

private:
	glm::vec3    m_color;
	float       m_intensity;
	Shader      m_shader;
	ShadowInfo  m_shadowInfo;
};

class DirectionalLight : public BaseLight
{
public:
	DirectionalLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, int shadowMapSizeAsPowerOf2 = 0,
		float shadowArea = 80.0f, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f);

	virtual ShadowCameraTransform CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const;

	inline float GetHalfShadowArea() const { return m_halfShadowArea; }

private:
	float m_halfShadowArea;
};

class Attenuation
{
public:
	Attenuation(float constant = 0, float linear = 0, float exponent = 1) :
		m_constant(constant),
		m_linear(linear),
		m_exponent(exponent) {}

	inline float GetConstant() const { return m_constant; }
	inline float GetLinear()   const { return m_linear; }
	inline float GetExponent() const { return m_exponent; }

private:
	float m_constant;
	float m_linear;
	float m_exponent;
};

class PointLight : public BaseLight
{
public:
	PointLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation(),
		const Shader& shader = Shader("forward-point"));

	inline const Attenuation& GetAttenuation() const { return m_attenuation; }
	inline const float GetRange()              const { return m_range; }

private:
	Attenuation m_attenuation;
	float m_range;
};

class SpotLight : public PointLight
{
public:
	SpotLight(const glm::vec3& color = glm::vec3(0, 0, 0), float intensity = 0, const Attenuation& atten = Attenuation(), float viewAngle = TO_RADIANS(170.0f),
		int shadowMapSizeAsPowerOf2 = 0, float shadowSoftness = 1.0f, float lightBleedReductionAmount = 0.2f, float minVariance = 0.00002f);

	inline float GetCutoff() const { return m_cutoff; }

private:
	float m_cutoff;
};
