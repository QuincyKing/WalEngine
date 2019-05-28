#include "RenderEngine.h"
#include "../core/Entity.h"


std::map<std::string, unsigned int> RenderEngine::SamplerMap;
BaseLight*  RenderEngine::ActiveLight = NULL;

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window),
	mFxaaFilter("fxaa")
{
	set_float("fxaaSpanMax", 8.0f);
	set_float("fxaaReduceMin", 1.0f / 128.0f);
	set_float("fxaaReduceMul", 1.0f / 8.0f);
	set_float("fxaaAspectDistortion", 150.0f);

	set_sampler_slot("filterTexture", 0);
	set_texture("displayTexture", Texture(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y(), 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0));
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	mFxaaFilter.set_shader("filter-null.vert", "filter-null.frag");
}

unsigned int vao;

void draw()
{
	if (vao == 0)
	{
		float quadVertices[] = {
			-1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,

			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};

		unsigned int quadVBO;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
	else
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void RenderEngine::render(Entity& object)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	get_texture("displayTexture").bind_render_target();

	//for (int i = 0; i < mLights.size(); i++)
	//{
		ActiveLight = mLights[0];
		Material::update_uniforms_mutable_all(this);


		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);

		object.render_all(*mMainCamera);

		//glDepthMask(GL_TRUE);
		//glDepthFunc(GL_LESS);
		//glDisable(GL_BLEND);
	//}

	//post_processing(mFxaaFilter, get_texture("displayTexture"), 0);
}

void RenderEngine::post_processing(Material& filter, const Texture& source, const Texture* dest)
{
	assert(&source != dest);
	if (dest == 0) mWindow->bind_render_target();
	else dest->bind_render_target();

	float displayTextureAspect = (float)get_texture("displayTexture").get_width() / (float)get_texture("displayTexture").get_height();
	float displayTextureHeightAdditive = displayTextureAspect * get_float("fxaaAspectDistortion");
	set_vec3("inverseFilterTextureSize", glm::vec3(1.0f / (float)get_texture("displayTexture").get_width(),
		1.0f / ((float)get_texture("displayTexture").get_height() + displayTextureHeightAdditive), 0.0f));

	filter.set_texture("filterTexture", source);

	glClear(GL_DEPTH_BUFFER_BIT);
	filter.mShader->use();
	filter.update_uniform("filterTexture");
	//filter.UpdateUniforms(m_planeTransform, m_planeMaterial, *this, m_altCamera);
	draw();

	filter.set_texture("filterTexture", Texture());
}