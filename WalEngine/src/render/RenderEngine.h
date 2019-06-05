#pragma once

#include "../core/Window.h"
#include "../core/DataPool.h"
#include "Camera.h"
#include "../model/Light.h"
#include "../model/Cube.h"
#include "FrameBuffer.h"
#include "../util/SkyBox.h"
#include <memory>
#include <vector>
#include <map>

class Entity;

class RenderEngine
{
public:
	RenderEngine() = default;
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(Entity& object);
	static unsigned int get_sampler_slot(const std::string& samplerName) { return SamplerMap.count(samplerName) > 0 ? SamplerMap.at(samplerName) : INVALID_VALUE; }
	static void set_sampler_slot(const std::string& name, unsigned int value) { SamplerMap[name] = value; }
	void post_processing(Material& filter, const FrameBuffer& source, const FrameBuffer* dest);
	void precompute();

public:
	static std::vector<BaseLight*>		Lights;
	static DataPool						Data;

private:
	static std::map<std::string, unsigned int> SamplerMap;
	const Window*                       mWindow;
	Material                            mFxaaFilter;
	Shader								irradianceShader;
	Shader								prefilterShader;
	Shader								brdfShader;
	Texture								irradianceMap;
	Texture								prefilterMap;
	Texture								brdfLUTTexture;
	Cube								box;
	FrameBuffer							displayFrame;
	SkyBox								mSkyBox;
};