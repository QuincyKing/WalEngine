#include "DataPool.h"

const glm::vec3& DataPool::get_vec3(const std::string& name) const
{
	std::map<std::string, glm::vec3>::const_iterator it = mVector3fMap.find(name);
	if (it != mVector3fMap.end())
	{
		return it->second;
	}

	return mDefaultVector3f;
}

float DataPool::get_float(const std::string& name) const
{
	std::map<std::string, float>::const_iterator it = mFloatMap.find(name);
	if (it != mFloatMap.end())
	{
		return it->second;
	}

	return 0;
}

const Texture& DataPool::get_texture(const std::string& name) const
{
	std::map<std::string, Texture>::const_iterator it = mTextureMap.find(name);
	if (it != mTextureMap.end())
	{
		return it->second;
	}

	return mDefaultTexture;
}