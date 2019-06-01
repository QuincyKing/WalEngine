#pragma once

#include "../model/Cube.h"

class SkyBox
{
public:
	SkyBox(std::string hdrfile);
	void render();
	inline Texture get_env_cubemap() { return mEnvCubemap; };
	void precompute();

public:
	static glm::mat4 CubeProjection;
	static glm::mat4 CubeViews[6];

private:
	Cube		mBox;
	Shader		mSkyboxShader;
	Shader		mToCubemapShader;
	Texture		mEnvCubemap;
	Texture		mHdr;
};