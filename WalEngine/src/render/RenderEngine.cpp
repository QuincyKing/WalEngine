#include "RenderEngine.h"
#include "../core/Entity.h"


std::map<std::string, unsigned int> RenderEngine::SamplerMap;
BaseLight*  RenderEngine::ActiveLight = NULL;

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window)
{
}

void RenderEngine::render(Entity& object)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//for (int i = 0; i < mLights.size(); i++)
	//{
		//ActiveLight = mLights[i];
		Material::update_uniforms_mutable_all();
		object.render_all(*mMainCamera);
	//}
}