#include "Texture.h"

#include <glm/glm.hpp>
#include <stb_image/stb_image.h>
#include <iostream>
#include <cassert>
#include <cstring>

std::map<std::string, std::shared_ptr<TextureData> > Texture::sResMap;

#pragma region TextureData
TextureData::TextureData
(
	GLenum textureTarget, 
	int width, 
	int height, 
	int numTextures, 
	void** data, 
	GLfloat* filters,
	GLenum* internalFormat, 
	GLenum* format,
	bool clamp, 
	TexType type
)
{
	mTextureID = new GLuint[numTextures];
	mTextureTarget = textureTarget;
	mNumTexs = numTextures;

	mWidth = width;
	mHeight = height;

	init(data, filters, internalFormat, format, clamp, type);
}

TextureData::~TextureData()
{
	if (*mTextureID) glDeleteTextures(mNumTexs, mTextureID);
	if (mTextureID) delete[] mTextureID;
}

void TextureData::init
(
	void** data,
	GLfloat* filters, 
	GLenum* internalFormat,
	GLenum* format, 
	bool clamp,
	TexType type
)
{
	glGenTextures(mNumTexs, mTextureID);
	for (int i = 0; i < mNumTexs; i++)
	{
		glBindTexture(mTextureTarget, mTextureID[i]);

		if (type == TexType::HDR)
			glTexImage2D(mTextureTarget, 0, internalFormat[i], mWidth, mHeight, 0, format[i], GL_FLOAT, data[i]);
		else if (type == TexType::Normal)
			glTexImage2D(mTextureTarget, 0, internalFormat[i], mWidth, mHeight, 0, format[i], GL_UNSIGNED_BYTE, data[i]);
		else if (type == TexType::CUBEMAP)
		{
			for (unsigned int j = 0; j < 6; ++j)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, internalFormat[i], mWidth, mHeight, 0, format[i], GL_FLOAT, data[i]);
			}
		}

		glTexParameterf(mTextureTarget, GL_TEXTURE_MIN_FILTER, filters[i]);
		glTexParameterf(mTextureTarget, GL_TEXTURE_MAG_FILTER, filters[i]);

		if (clamp)
		{
			glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			if (type == TexType::CUBEMAP)
				glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
			if (type == TexType::CUBEMAP)
				glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
		}

		if (filters[i] == GL_NEAREST_MIPMAP_NEAREST ||
			filters[i] == GL_NEAREST_MIPMAP_LINEAR ||
			filters[i] == GL_LINEAR_MIPMAP_NEAREST ||
			filters[i] == GL_LINEAR_MIPMAP_LINEAR)
		{
			glGenerateMipmap(mTextureTarget);
			GLfloat maxAnisotropy;
			glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
			glTexParameterf(mTextureTarget, GL_TEXTURE_MAX_ANISOTROPY, glm::clamp(0.0f, 8.0f, maxAnisotropy));
		}
		else
		{
			glTexParameteri(mTextureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}
	}
}

void TextureData::bind(int textureNum) const
{
	glBindTexture(mTextureTarget, mTextureID[textureNum]);
}

#pragma endregion

#pragma region Texture
Texture::Texture(const std::string& fileName)
{
	mFileName = fileName;
}

Texture::Texture(int width, int height, void* data, GLenum textureTarget, GLfloat filter, GLenum internalFormat, GLenum format, bool clamp)
{
	mFileName = "";
	TexType type = TexType::Normal;
	if(textureTarget == GL_TEXTURE_CUBE_MAP)
		type = TexType::CUBEMAP;
	mTextureData = std::make_shared<TextureData>(textureTarget, width, height, 1, &data, &filter, &internalFormat, &format, clamp, type);
}

void Texture::process
(
	GLenum textureTarget,
	GLfloat filter,
	GLenum internalFormat, 
	bool clamp
)
{
	std::map<std::string, std::shared_ptr<TextureData> >::const_iterator it = sResMap.find(mFileName);
	if (it != sResMap.end())
	{
		mTextureData = it->second;
	}
	else
	{
		
		int x, y, bytesPerPixel;
		void* data;
		TexType type = TexType::Normal;
		if (textureTarget == GL_TEXTURE_CUBE_MAP)
			type = TexType::CUBEMAP;
		if (mFileName.substr(mFileName.length() - 4, mFileName.length()) == ".hdr")
		{
			data = stbi_loadf(("../res/textures/" + mFileName).c_str(), &x, &y, &bytesPerPixel, 0);
			type = TexType::HDR;
		}
		else 
			data = stbi_load(("../res/textures/" + mFileName).c_str(), &x, &y, &bytesPerPixel, 0);

		mComponents = bytesPerPixel;

		if (data == NULL)
		{
			std::cerr << "Unable to load texture: " << mFileName << std::endl;
		}

		GLenum format;
		if (mComponents == 1)
			format = GL_RED;
		else if (mComponents == 3)
			format = GL_RGB;
		else if (mComponents == 4)
			format = GL_RGBA;

		mTextureData = std::make_shared<TextureData>(textureTarget, x, y, 1, &data, &filter, &internalFormat, &format, clamp, type);
		stbi_image_free(data);

		sResMap.insert(std::pair<std::string, std::shared_ptr<TextureData> >(mFileName, mTextureData));
	}
}

void Texture::process
(
	int width,
	int height,
	void* data,
	GLenum textureTarget,
	GLfloat filter,
	GLenum internalFormat,
	GLenum format,
	bool clamp
)
{
	TexType type = TexType::Normal;
	if (textureTarget == GL_TEXTURE_CUBE_MAP)
		type = TexType::CUBEMAP;
	mTextureData = std::make_shared<TextureData>(textureTarget, width, height, 1, &data, &filter, &internalFormat, &format, clamp, type);
}

Texture::Texture(const Texture& texture) :
	mTextureData(texture.mTextureData),
	mFileName(texture.mFileName)
{

}

void Texture::operator=(Texture texture)
{
	char* temp[sizeof(Texture) / sizeof(char)];
	memcpy(temp, this, sizeof(Texture));
	memcpy(this, &texture, sizeof(Texture));
	memcpy(&texture, temp, sizeof(Texture));
}

Texture::~Texture()
{
	if (mTextureData && sResMap.size() != 0)
	{
		if (mFileName.length() > 0)
			sResMap.erase(mFileName);
		mTextureData.reset();
	}
}

void Texture::bind(unsigned int unit) const
{
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	mTextureData->bind(0);
}

#pragma endregion