#pragma once

#include "../core/Window.h"
#include "../render/Shader.h"
#include "../core/Entity.h"
#include "Camera.h"
#include <memory>

class RenderEngine
{
public:
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(Entity& object);

private:
	std::shared_ptr<Shader>				mDefaultShader;
	const Window*                       mWindow;
	const Camera*                       mMainCamera;
};