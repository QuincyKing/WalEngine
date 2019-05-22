#include "Light.h"

//#include "renderingEngine.h"
//#include "../core/coreEngine.h"
//
//#define COLOR_DEPTH 256
//
//void BaseLight::AddToEngine(CoreEngine* engine) const
//{
//	engine->GetRenderingEngine()->AddLight(*this);
//}
//
//ShadowCameraTransform BaseLight::CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const
//{
//	return ShadowCameraTransform(GetTransform().GetTransformedPos(), GetTransform().GetTransformedRot());
//}
//

//
//
//ShadowCameraTransform DirectionalLight::CalcShadowCameraTransform(const glm::vec3& mainCameraPos, const Quaternion& mainCameraRot) const
//{
//	glm::vec3 resultPos = mainCameraPos + mainCameraRot.GetForward() * GetHalfShadowArea();
//	Quaternion resultRot = GetTransform().GetTransformedRot();
//
//	float worldTexelSize = (GetHalfShadowArea() * 2) / ((float)(1 << GetShadowInfo().GetShadowMapSizeAsPowerOf2()));
//
//	glm::vec3 lightSpaceCameraPos = resultPos.Rotate(resultRot.Conjugate());
//
//	lightSpaceCameraPos.x = worldTexelSize * floor(lightSpaceCameraPos.x / worldTexelSize);
//	lightSpaceCameraPos.y = worldTexelSize * floor(lightSpaceCameraPos.y / worldTexelSize);
//
//	resultPos = lightSpaceCameraPos.Rotate(resultRot);
//
//	return ShadowCameraTransform(resultPos, resultRot);
//}
//
//PointLight::PointLight(const glm::vec3& color, float intensity, const Attenuation& attenuation, const Shader& shader) :
//	BaseLight(color, intensity, shader),
//	m_attenuation(attenuation)
//{
//	float a = m_attenuation.GetExponent();
//	float b = m_attenuation.GetLinear();
//	float c = m_attenuation.GetConstant() - COLOR_DEPTH * intensity * color.Max();
//
//	m_range = (-b + sqrtf(b*b - 4 * a*c)) / (2 * a);
//}
//
//SpotLight::SpotLight(const glm::vec3& color, float intensity, const Attenuation& attenuation, float viewAngle,
//	int shadowMapSizeAsPowerOf2, float shadowSoftness, float lightBleedReductionAmount, float minVariance) :
//	PointLight(color, intensity, attenuation, Shader("forward-spot")),
//	m_cutoff(cos(viewAngle / 2))
//{
//	if (shadowMapSizeAsPowerOf2 != 0)
//	{
//		SetShadowInfo(ShadowInfo(glm::perspective(viewAngle, 1.0, 0.1, GetRange()), false, shadowMapSizeAsPowerOf2,
//			shadowSoftness, lightBleedReductionAmount, minVariance));
//	}
//}

//ShadowCameraTransform BaseLight::CalcShadowCameraTransform(const glm::vec3 & mainCameraPos, const Quaternion & mainCameraRot) const
//{
//	return ShadowCameraTransform();
//}


DirectionalLight::DirectionalLight(const glm::vec3& color, float intensity, int shadowMapSizeAsPowerOf2,
	float shadowArea, float shadowSoftness, float lightBleedReductionAmount, float minVariance) :
	BaseLight(color, intensity, Shader("direction.vert", "direction.frag")),
	mHalfShadowArea(shadowArea / 2.0f)
{
	/*if (shadowMapSizeAsPowerOf2 != 0)
	{
		SetShadowInfo(ShadowInfo(glm::orthographic(-m_halfShadowArea, m_halfShadowArea, -m_halfShadowArea,
			m_halfShadowArea, -m_halfShadowArea, m_halfShadowArea),
			true, shadowMapSizeAsPowerOf2, shadowSoftness, lightBleedReductionAmount, minVariance));
	}*/
}