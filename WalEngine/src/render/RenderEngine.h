#pragma once

#include "../core/Window.h"
#include "../core/MapVal.h"
#include "Camera.h"
#include "../model/Light.h"
#include <memory>
#include <map>

class Entity;

class RenderEngine : public MapVal
{
public:
	RenderEngine() = default;
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(Entity& object);
	static unsigned int get_sampler_slot(const std::string& samplerName) { return SamplerMap.find(samplerName)->second; }
	static void set_sampler_slot(const std::string& name, unsigned int value) { SamplerMap[name] = value; }

public:
	static BaseLight*					ActiveLight;

private:
	static std::map<std::string, unsigned int> SamplerMap;
	const Window*                       mWindow;
	const Camera*                       mMainCamera;
	std::vector<BaseLight*>				mLights;
};