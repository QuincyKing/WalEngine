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
			mMateriaData = new MaterialData();
			ResourceMap[mMaterialName] = mMateriaData;
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