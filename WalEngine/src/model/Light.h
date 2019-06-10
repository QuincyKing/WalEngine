#pragma once

#include <memory>

#include "../component/LightCom.h"
#include "../core/Entity.h"

class BaseLight : public Entity
{
public:
	BaseLight(std::string name, 
		glm::vec3 pos,
		Quaternion rot,
		glm::vec3 scale ) :
		Entity(name, pos, rot, scale)
	{

	}

	inline std::shared_ptr<Transform> get_transform() { return mTransform; }
	inline const Transform& get_transform() const { return *mTransform; }
	inline void set_pos(glm::vec3 pos) { mTransform->set_pos(pos); }
};

class DirectionalLight : public BaseLight
{
public:
	DirectionalLight(std::string name, 
		const glm::vec3& color = glm::vec3(0, 0, 0),
		float intensity = 1.0f,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0f) ) :
	BaseLight(name, pos, rot, scale)
	{
		//TODO : memory leaking
		add_component(ComType::DirLit, new DirectionalLightCom(color, intensity));
	}
	~DirectionalLight() {}

	template<typename T>
	T* get_component() const { return Entity::get_component<T>(ComType::DirLit); }
};

class PointLight : public BaseLight
{
public:
  	PointLight(std::string name,
		const glm::vec3& color = glm::vec3(0, 0, 0), 
		float intensity = 1.0f, 
		const Attenuation& atten = Attenuation(),
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0f)) :
		BaseLight(name, pos, rot, scale)
	{
		add_component(ComType::PointLit, new PointLightCom(color, intensity, atten));
	}

	~PointLight() {}

	template<typename T>
	T* get_component() const { return Entity::get_component<T>(ComType::PointLit); }
};

class SpotLight : public BaseLight
{
public:
	SpotLight(std::string name,
		const glm::vec3& color = glm::vec3(0, 0, 0), 
		float intensity = 1.0f,
		const Attenuation& atten = Attenuation(),
		float viewAngle = TO_RADIANS(170.0f),
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0f)) :
		BaseLight(name, pos, rot, scale)
	{
		add_component(ComType::SpotLit, new SpotLightCom(color, intensity, atten, viewAngle));
	}

	~SpotLight() {}

	template<typename T>
	T* get_component() const { return Entity::get_component<T>(ComType::SpotLit); }
};

