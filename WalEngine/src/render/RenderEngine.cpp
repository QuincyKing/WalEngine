#include "RenderEngine.h"
#include "../core/Entity.h"

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window)
{
}

void RenderEngine::render(Entity& object)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	object.render_all(*mMainCamera);
}