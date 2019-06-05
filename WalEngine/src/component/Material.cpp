#include "material.h"
#include "../core/Window.h"
#include "../render/RenderEngine.h"
#include "../model/Light.h"
#include <iostream>
#include <algorithm>
#include <cassert>

std::map<std::string, DefineData> Material::ResourceMap;

void set_uniform_dirlight(Shader* shader, const std::string& uniformName, const DirectionalLight& directionalLight)
{
	shader->set_vec3(uniformName + ".direction", directionalLight.get_transform().get_transform_rot().get_forward());
	shader->set_vec3(uniformName + ".color", directionalLight.get_component<DirectionalLightCom>()->get_color());
	shader->set_float(uniformName + ".intensity", directionalLight.get_component<DirectionalLightCom>()->get_intensity());
}

void set_uniform_pointlight(Shader* shader, const std::string& uniformName, const PointLight& pointLight)
{
	shader->set_vec3(uniformName + ".color", pointLight.get_component<PointLightCom>()->get_color());
	shader->set_float(uniformName + ".intensity", pointLight.get_component<PointLightCom>()->get_intensity());
	shader->set_float(uniformName + ".constant", pointLight.get_component<PointLightCom>()->get_attenuation().get_constant());
	shader->set_float(uniformName + ".linear", pointLight.get_component<PointLightCom>()->get_attenuation().get_linear());
	shader->set_float(uniformName + ".exponent", pointLight.get_component<PointLightCom>()->get_attenuation().get_exponent());
	shader->set_vec3(uniformName + ".position", pointLight.get_transform().get_transform_pos());
	shader->set_float(uniformName + ".range", pointLight.get_component<PointLightCom>()->get_range());
}

void set_uniform_spotlight(Shader* shader, const std::string& uniformName, const SpotLight& spotLight)
{
	shader->set_vec3(uniformName + ".color", spotLight.get_component<SpotLightCom>()->get_color());
	shader->set_float(uniformName + ".intensity", spotLight.get_component<SpotLightCom>()->get_intensity());
	shader->set_float(uniformName + ".constant", spotLight.get_component<SpotLightCom>()->get_attenuation().get_constant());
	shader->set_float(uniformName + ".linear", spotLight.get_component<SpotLightCom>()->get_attenuation().get_linear());
	shader->set_float(uniformName + ".exponent", spotLight.get_component<SpotLightCom>()->get_attenuation().get_exponent());
	shader->set_vec3(uniformName + ".position", spotLight.get_transform().get_transform_pos());
	shader->set_float(uniformName + ".range", spotLight.get_component<SpotLightCom>()->get_range());
	shader->set_vec3(uniformName + ".direction", spotLight.get_transform().get_transform_rot().get_forward());
	shader->set_float(uniformName + ".cutoff", spotLight.get_component<SpotLightCom>()->get_cutoff());
}

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
		//if (uniformName.substr(0, 2) == "R_")
		//{
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
		//}
		//texture variance
		//if (uniformType == "sampler2D")
		//{
			//int samplerSlot = renderEngine.get_sampler_slot(uniformName);
			//get_texture(uniformName).bind(samplerSlot);
			//mShader.set_int(uniformName, samplerSlot);
		//}
		//constant variance
		if (uniformName.substr(0, 2) == "C_")
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

			//constant variance
			if (uniformName.substr(0, 2) == "C_")
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
				else if (uniformType == "sampler2D")
				{
					std::string varianceName = uniformName.substr(2, uniformName.length());
					int samplerSlot = RenderEngine::get_sampler_slot(varianceName);
					if (samplerSlot != INVALID_VALUE)
					{
						md->get_texture(varianceName).bind(samplerSlot);
						shader->set_int(uniformName, samplerSlot);
					}
				}
				else
					throw uniformType + " is not supported by the Material class";
			}
			else if (uniformName.substr(0, 2) == "R_")
			{
				std::string varianceName = uniformName.substr(2, uniformName.length());

				if (uniformType == "DirectionalLight")
				{
					auto iter = std::find_if(RenderEngine::Lights.begin(), RenderEngine::Lights.end(), [&varianceName](const BaseLight* b) { return b->mName == varianceName; });
					BaseLight* light = *iter;
					set_uniform_dirlight(shader, uniformName, *dynamic_cast<DirectionalLight *>(light));
				}
				else if (uniformType == "PointLight")
				{
					auto iter = std::find_if(RenderEngine::Lights.begin(), RenderEngine::Lights.end(), [&varianceName](const BaseLight* b) { return b->mName == varianceName; });
					BaseLight* light = *iter;
					set_uniform_pointlight(shader, uniformName, *dynamic_cast<PointLight *>(light));
				}
				else if (uniformType == "SpotLight")
				{
					auto iter = std::find_if(RenderEngine::Lights.begin(), RenderEngine::Lights.end(), [&varianceName](const BaseLight* b) { return b->mName == varianceName; });
					BaseLight* light = *iter;
					set_uniform_spotlight(shader, uniformName, *dynamic_cast<SpotLight *>(light));
				}
				else if (uniformType == "vec3")
					shader->set_vec3(uniformName, RenderEngine::Data.get_vec3(varianceName));
				else if (uniformType == "float")
					shader->set_float(uniformName, RenderEngine::Data.get_float(varianceName));
			}
		}
	}
}

void Material::update_uniform_sampler(std::string varianceName, int samplerSlot)
{
	if (samplerSlot != INVALID_VALUE)
	{
		mMateriaData->get_texture(varianceName).bind(samplerSlot);
		mShader->set_int(varianceName, samplerSlot);
	}
}

void Material::set_shader(const std::string& vsFile, const std::string& fsFile)
{
	mShader->set_shader(vsFile, fsFile);
}