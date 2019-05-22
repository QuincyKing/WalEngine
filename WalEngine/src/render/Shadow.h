#pragma once

#include "../core/Quaternion.h"

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