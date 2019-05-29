#pragma once

#include <map>

#include "../render/Texture.h"
#include "../core/MapVal.h"
#include "../core/Ref.h"
#include "../core/Transform.h"
#include "../core/Component.h"
#include "../render/Shader.h"

class MaterialData : public MapVal, public Ref
{
};

class DefineData
{
public:
	DefineData()
	{

	}

	DefineData(MaterialData *md, Shader* s) 
	{
		mMateriaData = md;
		mShader = s;
	}

	MaterialData* mMateriaData;
	Shader*		  mShader;
};

class RenderEngine;

class Material : public Component
{
public:
	Material(const std::string& materialName);
	Material(const Material& other);
	virtual ~Material();

	void render() {};
	inline void set_vec3(const std::string& name, const glm::vec3& value) { mMateriaData->set_vec3(name, value); }
	inline void set_float(const std::string& name, float value) { mMateriaData->set_float(name, value); }
	inline void set_texture(const std::string& name, const Texture& value) { mMateriaData->set_texture(name, value); }

	inline const glm::vec3& get_vec3(const std::string& name) const { return mMateriaData->get_vec3(name); }
	inline float get_float(const std::string& name)              const { return mMateriaData->get_float(name); }
	inline const Texture& get_texture(const std::string& name)   const { return mMateriaData->get_texture(name); }
	void update_uniforms_constant() const;
	static void update_uniforms_constant_all();
	void update_uniforms_mutable() const;
	static void update_uniforms_mutable_all(RenderEngine* render);
	void operator=(const Material& other) {}
	void set_shader(const std::string& vsFile, const std::string& fsFile);
	void update_uniform_sampler(std::string varianceName, int);
	inline void update_uniform_vec3(std::string varianceName, glm::vec3 &v) { mShader->set_vec3(varianceName, v); };
	inline void update_uniform_mat4(std::string varianceName, glm::mat4 &m) { mShader->set_mat4(varianceName, m); };
	inline void update_uniform_float(std::string varianceName, float f) { mShader->set_float(varianceName, f); };

public:
	Shader*		  mShader;

private:
	static std::map<std::string, DefineData> ResourceMap;
	MaterialData* mMateriaData;
	std::string   mMaterialName;
};