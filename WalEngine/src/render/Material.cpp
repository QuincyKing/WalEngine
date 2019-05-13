#include "material.h"
#include <iostream>
#include <cassert>

std::map<std::string, MaterialData*> Material::ResourceMap;

Material::Material(const std::string& materialName) :
	mMaterialName(materialName)
{
	if (materialName.length() > 0)
	{
		std::map<std::string, MaterialData*>::const_iterator it = Material::ResourceMap.find(materialName);
		if (it == ResourceMap.end())
		{
			std::cout << "fsdfs" << std::endl;
			mMateriaData = new MaterialData();
			ResourceMap[mMaterialName] = mMateriaData;
			//mShader.set_shader("default.vert", "default.frag");
		}
		else
		{
			mMateriaData = it->second;
			mMateriaData->add_reference();
		}
	}
}

Material::Material(const Material& other) :
	mMateriaData(other.mMateriaData),
	mMaterialName(other.mMaterialName)
{
	mMateriaData->add_reference();
}

Material::~Material()
{
	if (mMateriaData && mMateriaData->remove_reference())
	{
		if (mMaterialName.length() > 0)
		{
			ResourceMap.erase(mMaterialName);
		}

		delete mMateriaData;
	}
}

//void Material::update_uniforms(const Transform& transform, const RenderEngine& renderEngine, const Camera& camera) const
//{
//	glm::mat4 worldMatrix = transform.get_model();
//	glm::mat4 projectedMatrix = camera.get_view_projection() * worldMatrix;
//
//	for (unsigned int i = 0; i < mShader.mShaderData->get_uniform_names().size(); i++)
//	{
//		std::string uniformName = mShader.mShaderData->get_uniform_names()[i];
//		std::string uniformType = mShader.mShaderData->get_uniform_types()[i];
//
//		// RenderEngine variance  
//		if (uniformName.substr(0, 2) == "R_")
//		{
//			/*std::string unprefixedName = uniformName.substr(2, uniformName.length());
//
//			if (unprefixedName == "lightMatrix")
//				set_uniformmat4(uniformName, renderEngine.GetLightMatrix() * worldMatrix);
//			else if (uniformType == "sampler2D")
//			{
//				int samplerSlot = renderEngine.GetSamplerSlot(unprefixedName);
//				renderEngine.GetTexture(unprefixedName).bind(samplerSlot);
//				set_uniformi(uniformName, samplerSlot);
//			}
//			else if (uniformType == "vec3")
//				set_uniformvec3(uniformName, renderEngine.GetVector3f(unprefixedName));
//			else if (uniformType == "float")
//				set_uniformf(uniformName, renderEngine.GetFloat(unprefixedName));*/
//				/*else if (uniformType == "DirectionalLight")
//					set_uniform_dirlight(uniformName, *(const DirectionalLight*)&renderEngine.GetActiveLight());
//				else if (uniformType == "PointLight")
//					set_uniform_pointlight(uniformName, *(const PointLight*)&renderEngine.GetActiveLight());
//				else if (uniformType == "SpotLight")
//					set_uniform_spotlight(uniformName, *(const SpotLight*)&renderEngine.GetActiveLight());*/
//					/*else
//						renderEngine.update_uniformstruct(transform, material, *this, uniformName, uniformType);*/
//		}
//		//texture variance
//		else if (uniformType == "sampler2D")
//		{
//			int samplerSlot = renderEngine.get_sampler_slot(uniformName);
//			get_texture(uniformName).bind(samplerSlot);
//			mShader.set_int(uniformName, samplerSlot);
//		}
//		//transform variance
//		else if (uniformName.substr(0, 2) == "T_")
//		{
//			if (uniformName == "T_MVP")
//				mShader.set_mat4(uniformName, projectedMatrix);
//			else if (uniformName == "T_model")
//				mShader.set_mat4(uniformName, worldMatrix);
//			else
//				throw "Invalid Transform Uniform: " + uniformName;
//		}
//		//constant variance
//		else if (uniformName.substr(0, 2) == "C_")
//		{
//			//camera position
//			if (uniformName == "C_cameraPos")
//				mShader.set_vec3(uniformName, camera.get_transform().get_transform_pos());
//			else
//				throw "Invalid Camera Uniform: " + uniformName;
//		}
//		//other variance
//		else
//		{
//			if (uniformType == "vec3")
//				mShader.set_vec3(uniformName, get_vec3(uniformName));
//			else if (uniformType == "float")
//				mShader.set_float(uniformName, get_float(uniformName));
//			else
//				throw uniformType + " is not supported by the Material class";
//		}
//	}
//}

void Material::set_shader(const std::string& vsFile, const std::string& fsFile)
{
	mShader.set_shader(vsFile, fsFile);
}