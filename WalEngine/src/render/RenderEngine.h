#pragma once

#include "../core/Window.h"
#include "../core/MapVal.h"
#include "Camera.h"
#include "../model/Light.h"
#include "../model/Cube.h"
#include <memory>
#include <vector>
#include <map>

class Entity;

class RenderEngine : public MapVal
{
public:
	RenderEngine() = default;
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(Entity& object);
	static unsigned int get_sampler_slot(const std::string& samplerName) { return SamplerMap.count(samplerName) > 0 ? SamplerMap.at(samplerName) : INVALID_VALUE; }
	static void set_sampler_slot(const std::string& name, unsigned int value) { SamplerMap[name] = value; }
	void add_light(BaseLight& light) { mLights.push_back(&light); }
	void post_processing(Material& filter, const Texture& source, const Texture* dest);
	void skybox();

public:
	static BaseLight*					ActiveLight;

private:
	static std::map<std::string, unsigned int> SamplerMap;
	const Window*                       mWindow;
	std::vector<BaseLight*>				mLights;
	Material                            mFxaaFilter;
	Shader								equirectangularToCubemapShader;
	Shader								backgroundShader;
	Shader								irradianceShader;
	Shader								prefilterShader;
	Shader								brdfShader;
	Texture								envCubemap;
	Texture								irradianceMap;
	Texture								prefilterMap;
	Texture								brdfLUTTexture;
	Cube								box;
};