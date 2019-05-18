#include "material.h"
#include "../core/Window.h"
#include "RenderEngine.h"
#include <iostream>
#include <cassert>

std::map<std::string, DefineData> Material::ResourceMap;

Material::Material(const std::string& materialName) :
	mMaterialName(materialName)
{
	if (materialName.length() > 0)
	{
		std::map<std::string, DefineData>::const_iterator it = Material::ResourceMap.find(materialName);
		if (it == ResourceMap.end())
		{
			mMateriaData = new MaterialData();
			mShader = new Shader();
			ResourceMap[mMaterialName] = DefineData(mMateriaData, mShader);
		}
		else
		{
			mMateriaData = it->second.mMateriaData;
			mShader = it->second.mShader;
			mMateriaData->add_reference();
			mShader->add_reference();
		}
	}
}

Material::Material(const Material& other) :
	mMateriaData(other.mMateriaData),
	mMaterialName(other.mMaterialName)
{
	mMateriaData->add_reference();
	mShader->add_reference();
}

Material::~Material()
{
	if (mMateriaData && mShader&& mMateriaData->remove_reference() && mShader->remove_reference())
	{
		if (mMaterialName.length() > 0 && !ResourceMap.empty())
		{
			Material::ResourceMap.erase(mMaterialName);
		}

		delete mMateriaData;
		delete mShader;
	}
}

void Material::update_uniforms_constant() const
{
	for (unsigned int i = 0; i < mShader->mShaderData->get_uniform_names().size(); i++)
	{
		std::string uniformName = mShader->mShaderData->get_uniform_names()[i];
		std::string uniformType = mShader->mShaderData->get_uniform_types()[i];

		// RenderEngine variance  
		if (uniformName.substr(0, 2) == "R_")
		{
			/*std::string unprefixedName = uniformName.substr(2, uniformName.length());

			if (unprefixedName == "lightMatrix")
				set_uniformmat4(uniformName, renderEngine.GetLightMatrix() * worldMatrix);
			else if (uniformType == "sampler2D")
			{
				int samplerSlot = renderEngine.GetSamplerSlot(unprefixedName);
				renderEngine.GetTexture(unprefixedName).bind(samplerSlot);
				set_uniformi(uniformName, samplerSlot);
			}
			else if (uniformType == "vec3")
				set_uniformvec3(uniformName, renderEngine.GetVector3f(unprefixedName));
			else if (uniformType == "float")
				set_uniformf(uniformName, renderEngine.GetFloat(unprefixedName));*/
				/*else if (uniformType == "DirectionalLight")
					set_uniform_dirlight(uniformName, *(const DirectionalLight*)&renderEngine.GetActiveLight());
				else if (uniformType == "PointLight")
					set_uniform_pointlight(uniformName, *(const PointLight*)&renderEngine.GetActiveLight());
				else if (uniformType == "SpotLight")
					set_uniform_spotlight(uniformName, *(const SpotLight*)&renderEngine.GetActiveLight());*/
					/*else
						renderEngine.update_uniformstruct(transform, material, *this, uniformName, uniformType);*/
		}
		//texture variance
		else if (uniformType == "sampler2D")
		{
			//int samplerSlot = renderEngine.get_sampler_slot(uniformName);
			//get_texture(uniformName).bind(samplerSlot);
			//mShader.set_int(uniformName, samplerSlot);
		}
		//constant variance
		else if (uniformName.substr(0, 2) == "C_")
		{
			if (uniformName == "C_CamPos")
				mShader->set_vec3(uniformName, Window::MainCamera.get_transform()->get_transform_pos());
			else
				throw "Invalid Camera Uniform: " + uniformName;
		}
		//other variance
		else
		{
			if (uniformType == "vec3")
				mShader->set_vec3(uniformName, get_vec3(uniformName));
			else if (uniformType == "float")
				mShader->set_float(uniformName, get_float(uniformName));
			else
				throw uniformType + " is not supported by the Material class";
		}
	}
}

void Material::update_uniforms_constant_all()
{
	for (auto iter = ResourceMap.begin(); iter != ResourceMap.end(); iter++)
	{
		DefineData data = iter->second;
		Shader* shader = data.mShader;
		MaterialData* md = data.mMateriaData;

		shader->use();
		for (unsigned int i = 0; i < shader->mShaderData->get_uniform_names().size(); i++)
		{
			std::string uniformName = shader->mShaderData->get_uniform_names()[i];
			std::string uniformType = shader->mShaderData->get_uniform_types()[i];

			// RenderEngine variance  
			if (uniformName.substr(0, 2) == "R_")
			{
				/*std::string unprefixedName = uniformName.substr(2, uniformName.length());

				if (unprefixedName == "lightMatrix")
					set_uniformmat4(uniformName, renderEngine.GetLightMatrix() * worldMatrix);
				else if (uniformType == "sampler2D")
				{
					int samplerSlot = renderEngine.GetSamplerSlot(unprefixedName);
					renderEngine.GetTexture(unprefixedName).bind(samplerSlot);
					set_uniformi(uniformName, samplerSlot);
				}
				else if (uniformType == "vec3")
					set_uniformvec3(uniformName, renderEngine.GetVector3f(unprefixedName));
				else if (uniformType == "float")
					set_uniformf(uniformName, renderEngine.GetFloat(unprefixedName));*/
					/*else if (uniformType == "DirectionalLight")
						set_uniform_dirlight(uniformName, *(const DirectionalLight*)&renderEngine.GetActiveLight());
					else if (uniformType == "PointLight")
						set_uniform_pointlight(uniformName, *(const PointLight*)&renderEngine.GetActiveLight());
					else if (uniformType == "SpotLight")
						set_uniform_spotlight(uniformName, *(const SpotLight*)&renderEngine.GetActiveLight());*/
						/*else
							renderEngine.update_uniformstruct(transform, material, *this, uniformName, uniformType);*/
			}
			//texture variance
			else if (uniformType == "sampler2D")
			{
				int samplerSlot = RenderEngine::get_sampler_slot(uniformName);
				md->get_texture(uniformName).bind(samplerSlot);
				shader->set_int(uniformName, samplerSlot);
			}
			//constant variance
			else if (uniformName.substr(0, 2) == "C_")
			{
				if(uniformType == "vec3")
					shader->set_vec3(uniformName, md->get_vec3(uniformName));
				else if(uniformType == "float")
					shader->set_float(uniformName, md->get_float(uniformName));
				else
					throw uniformType + " is not supported by the Material class";
			}
		}
	}
}

void Material::update_uniforms_mutable() const
{
	for (unsigned int i = 0; i < mShader->mShaderData->get_uniform_names().size(); i++)
	{
		std::string uniformName = mShader->mShaderData->get_uniform_names()[i];
		std::string uniformType = mShader->mShaderData->get_uniform_types()[i];

	}
}

void Material::update_uniforms_mutable_all()
{
	for (auto iter = ResourceMap.begin(); iter != ResourceMap.end(); iter++)
	{
		DefineData data = iter->second;
		Shader* shader = data.mShader;
		MaterialData* md = data.mMateriaData;

		shader->use();
		for (unsigned int i = 0; i < shader->mShaderData->get_uniform_names().size(); i++)
		{
			std::string uniformName = shader->mShaderData->get_uniform_names()[i];
			std::string uniformType = shader->mShaderData->get_uniform_types()[i];

			//transform variance
			if (uniformName.substr(0, 2) == "T_")
			{
				glm::mat4 projectedMatrix = Window::MainCamera.get_view_projection();
				if (uniformName == "T_VP")
					shader->set_mat4(uniformName, projectedMatrix);
			}
			else if (uniformName.substr(0, 2) == "M_")
			{
				if(uniformName == "M_CamPos")
					shader->set_vec3(uniformName, Window::MainCamera.get_transform()->get_transform_pos());
				else if (uniformType == "vec3")
					shader->set_vec3(uniformName, md->get_vec3(uniformName));
				else if (uniformType == "float")
					shader->set_float(uniformName, md->get_float(uniformName));
				else
					throw uniformType + " is not supported by the Material class";
			}
		}
	}
}

void Material::set_shader(const std::string& vsFile, const std::string& fsFile)
{
	mShader->set_shader(vsFile, fsFile);
}