#include "RenderEngine.h"

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window)
{
	mDefaultShader = std::make_shared<Shader>("./shader/default.vert", "./shader/default.frag");
}

void RenderEngine::render(Entity& object, glm::mat4 view)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDefaultShader->use();
	mDefaultShader->setMat4("vp", view);
	object.render_all(mDefaultShader, *this, *mMainCamera);
}