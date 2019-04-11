#pragma once

#include <map>

#include "../render/Texture.h"
#include <glm/glm.hpp>

class MapVal
{
public:
	MapVal() :
		m_defaultTexture(Texture("defaultTexture.png")),
		m_defaultVector3f(glm::vec3(0, 0, 0)) {}

	inline void set_vec3(const std::string& name, const glm::vec3& value) { m_vector3fMap[name] = value; }
	inline void set_float(const std::string& name, float value) { m_floatMap[name] = value; }
	inline void set_texture(const std::string& name, const Texture& value) { m_textureMap[name] = value; }

	const glm::vec3& get_vec3(const std::string& name)	  const;
	float get_float(const std::string& name)              const;
	const Texture& get_texture(const std::string& name)   const;

private:
	std::map<std::string, glm::vec3> m_vector3fMap;
	std::map<std::string, float> m_floatMap;
	std::map<std::string, Texture> m_textureMap;

	Texture m_defaultTexture;
	glm::vec3 m_defaultVector3f;
};