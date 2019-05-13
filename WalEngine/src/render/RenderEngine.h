#pragma once

#include "../core/Window.h"
#include "../core/MapVal.h"
#include "Camera.h"
#include <memory>
#include <map>

class Entity;

class RenderEngine : public MapVal
{
public:
	RenderEngine() = default;
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(Entity& object, glm::mat4 view);
	inline unsigned int get_sampler_slot(const std::string& samplerName) const { return mSamplerMap.find(samplerName)->second; }
	inline void set_sampler_slot(const std::string& name, unsigned int value) { mSamplerMap[name] = value; }

private:
	const Window*                       mWindow;
	const Camera*                       mMainCamera;
	std::map<std::string, unsigned int> mSamplerMap;
};