#pragma once

#include "../src/render/RenderEngine.h"

class Game
{
public:
	Game() : renderRoot("root") {}
	virtual ~Game() {};
	virtual void precompute() {};
	virtual void init() {};
	virtual void render(RenderEngine &renderer) {};
	virtual void gui() {};
	virtual void key() {};

public:
	Entity renderRoot;
};