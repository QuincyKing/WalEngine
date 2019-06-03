#include "RenderEngine.h"
#include "../core/Entity.h"
#include "../model/Cube.h"
#include <stb_image/stb_image.h>

std::map<std::string, unsigned int> RenderEngine::SamplerMap;
std::vector<BaseLight*>		RenderEngine::Lights = std::vector<BaseLight*>();

unsigned int quadVAO = 0, quadVBO = 0;

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window),
	mFxaaFilter("fxaa"),
	irradianceShader("cubemap.vert", "irradiance_convolution.frag"),
	prefilterShader("cubemap.vert", "prefilter.frag"),
	brdfShader("brdf.vert", "brdf.frag"),
	box("box"),
	displayFrame(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y()),
	mSkyBox("hdr/uffizi.hdr")
{
	set_float("fxaaSpanMax", 8.0f);
	set_float("fxaaReduceMin", 1.0f / 128.0f);
	set_float("fxaaReduceMul", 1.0f / 8.0f);
	set_float("fxaaAspectDistortion", 150.0f);

	set_sampler_slot("screenTexture", 0);
	set_texture("displayTexture", Texture(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y(), 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true));

	mFxaaFilter.set_shader("fxaa.vert", "fxaa.frag");
	mFxaaFilter.mShader->set_int("screenTexture", RenderEngine::get_sampler_slot("screenTexture"));

	precompute();
}

void draw_quad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
								 // positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};
		// screen quad VAO

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void RenderEngine::precompute()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); 

	mSkyBox.precompute();

	FrameBuffer capture(512, 512);
	irradianceMap = Texture(32, 32, 0, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_RGB16F, GL_RGB, true);
	capture.change_render_buffer_storage(32, 32);

	irradianceShader.use();
	irradianceShader.set_int("environmentMap", 0);
	irradianceShader.set_mat4("projection", SkyBox::CubeProjection);
	mSkyBox.get_env_cubemap().bind(0);

	glViewport(0, 0, 32, 32); 
	capture.bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.set_mat4("view", SkyBox::CubeViews[i]);
		capture.bind_texture(irradianceMap.get_ID()[0], GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		box.draw();
	}
	FrameBuffer::bind_render_targer_reset();

	prefilterMap = Texture(128, 128, 0, GL_TEXTURE_CUBE_MAP, GL_LINEAR_MIPMAP_LINEAR, GL_RGB16F, GL_RGB, true);

	prefilterShader.use();
	prefilterShader.set_int("environmentMap", 0);
	prefilterShader.set_mat4("projection", SkyBox::CubeProjection);
	mSkyBox.get_env_cubemap().bind(0);

	capture.bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		capture.change_render_buffer_storage(mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.set_float("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.set_mat4("view", SkyBox::CubeViews[i]);
			capture.bind_texture(prefilterMap.get_ID()[0], GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			box.draw();
		}
	}
	FrameBuffer::bind_render_targer_reset();

	brdfLUTTexture = Texture(512, 512, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RG16F, GL_RG, true);

	capture.change_render_buffer_storage(512, 512);
	capture.bind_texture(brdfLUTTexture.get_ID()[0]);

	glViewport(0, 0, 512, 512);
	brdfShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_quad();

	FrameBuffer::bind_render_targer_reset();
}

void RenderEngine::render(Entity& object)
{
	displayFrame.bind_render_target();
	displayFrame.bind_texture(get_texture("displayTexture").get_ID()[0]);
	//Material::update_uniforms_constant_all();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	irradianceMap.bind(0);
	prefilterMap.bind(1);
	brdfLUTTexture.bind(2);
	//for (int i = 0; i < mLights.size(); i++)
	//{
		//ActiveLight = mLights[0];
		Material::update_uniforms_mutable_all(this);


		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);

		object.render_all();

		mSkyBox.render();
		//glDepthMask(GL_TRUE);
		//glDepthFunc(GL_LESS);
		//glDisable(GL_BLEND);
	//}
		
	post_processing(mFxaaFilter, displayFrame, 0);
}

void RenderEngine::post_processing(Material& filter, const FrameBuffer& source, const FrameBuffer* dest)
{
	assert(&source != dest);
	if (dest == 0) mWindow->bind_render_target();
	else dest->bind_render_target();

	float displayTextureAspect = (float)get_texture("displayTexture").get_width() / (float)get_texture("displayTexture").get_height();
	float displayTextureHeightAdditive = displayTextureAspect * get_float("fxaaAspectDistortion");
	set_vec3("inverseFilterTextureSize", glm::vec3(1.0f / (float)get_texture("displayTexture").get_width(),
		1.0f / ((float)get_texture("displayTexture").get_height() + displayTextureHeightAdditive), 0.0f));

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	filter.mShader->use();
	get_texture("displayTexture").bind(0);
	draw_quad();
}