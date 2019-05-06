#pragma once

#include "Texture.h"
#include "../core/MapVal.h"
#include "../core/ReferenceCounter.h"
#include <map>

class MaterialData : public MapVal, public ReferenceCounter
{
};

class Material
{
public:
	Material(const std::string& materialName = "");
	Material(const Material& other);
	virtual ~Material();

	inline void set_vec3(const std::string& name, const glm::vec3& value) { mMateriaData->set_vec3(name, value); }
	inline void set_float(const std::string& name, float value) { mMateriaData->set_float(name, value); }
	inline void set_texture(const std::string& name, const Texture& value) { mMateriaData->set_texture(name, value); }

	inline const glm::vec3& get_vec3(const std::string& name) const { return mMateriaData->get_vec3(name); }
	inline float get_float(const std::string& name)              const { return mMateriaData->get_float(name); }
	inline const Texture& get_texture(const std::string& name)   const { return mMateriaData->get_texture(name); }

private:
	static std::map<std::string, MaterialData*> ResourceMap;
	MaterialData* mMateriaData;
	std::string   mMaterialName;

	void operator=(const Material& other) {}
};
