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
	glm::mat4 captureProjection;
	glm::mat4* captureViews;

private:
	Cube		mBox;
	Shader		mSkyboxShader;
	Shader		mToCubemapShader;
	Texture		mEnvCubemap;
};