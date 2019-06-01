#pragma once

#include <glad/glad.h>
#include <string>
#include <memory>
#include <map>

enum class TexType
{
	Normal,
	HDR,
	CUBEMAP
};

class TextureData
{
public:
	TextureData
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
	);

	void bind(int textureNum) const;
	inline int get_width()  const { return mWidth; }
	inline int get_height() const { return mHeight; }
	inline GLuint* get_ID() const { return mTextureID; }

	virtual ~TextureData();

private:
	void init
	(
		void** data, 
		GLfloat* filter, 
		GLenum* internalFormat, 
		GLenum* format, 
		bool clamp,
		TexType type
	);

	GLuint* mTextureID;
	GLenum mTextureTarget;
	int mNumTexs;
	int mWidth;
	int mHeight;
};

class Texture
{
public:
	Texture( const std::string& fileName );
	Texture(int width = 0, int height = 0, void* data = 0, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR_MIPMAP_LINEAR, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false);

	Texture(const Texture& texture);
	void operator=(Texture texture);
	virtual ~Texture();

	void process
	(
		GLenum textureTarget = GL_TEXTURE_2D,
		GLfloat filter = GL_LINEAR_MIPMAP_LINEAR,
		GLenum internalFormat = GL_RGB,
		bool clamp = true
	);

	void process
	(
		int width,
		int height,
		void* data,
		GLenum textureTarget = GL_TEXTURE_2D,
		GLfloat filter = GL_LINEAR_MIPMAP_LINEAR,
		GLenum internalFormat = GL_RGBA,
		GLenum format = GL_RGBA,
		bool clamp = false
	);

	void bind(unsigned int unit = 0) const;

	inline int get_width()  const { return mTextureData->get_width(); }
	inline int get_height() const { return mTextureData->get_height(); }
	GLuint* get_ID() const { return mTextureData->get_ID(); }
	bool operator==(const Texture& texture) const { return mTextureData == texture.mTextureData; }
	bool operator!=(const Texture& texture) const { return !operator==(texture); }

private:
	static std::map<std::string, std::shared_ptr<TextureData> > sResMap;

	int mComponents;
	std::shared_ptr<TextureData> mTextureData;
	std::string mFileName;
};