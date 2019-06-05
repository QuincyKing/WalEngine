#pragma once

#include <map>

#include "../render/Texture.h"
#include <glm/glm.hpp>

class DataPool
{
public:
	DataPool() :
		mDefaultTexture(Texture("defaultTexture.png")),
		mDefaultVector3f(glm::vec3(0, 0, 0)) {}

	inline void set_vec3(const std::string& name, const glm::vec3& value) { mVector3fMap[name] = value; }
	inline void set_float(const std::string& name, float value) { mFloatMap[name] = value; }
	inline void set_texture(const std::string& name, const Texture& value) { mTextureMap[name] = value; }

	const glm::vec3& get_vec3(const std::string& name)	  const;
	float get_float(const std::string& name)              const;
	const Texture& get_texture(const std::string& name)   const;

private:
	std::map<std::string, glm::vec3> mVector3fMap;
	std::map<std::string, float> mFloatMap;
	std::map<std::string, Texture> mTextureMap;

	Texture mDefaultTexture;
	glm::vec3 mDefaultVector3f;
};