#pragma once

#include "../core/Window.h"

class RenderEngine
{
public:
	RenderEngine(const Window& window);
	virtual ~RenderEngine() {}

	void render(const Entity& object);

private:

};