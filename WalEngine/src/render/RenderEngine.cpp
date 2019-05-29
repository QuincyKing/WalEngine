#include "RenderEngine.h"
#include "../core/Entity.h"
#include "../model/Cube.h"
#include <stb_image/stb_image.h>

std::map<std::string, unsigned int> RenderEngine::SamplerMap;
BaseLight*  RenderEngine::ActiveLight = NULL;

unsigned int quadVAO = 0, quadVBO = 0;

RenderEngine::RenderEngine(const Window& window)
	:mWindow(&window),
	mFxaaFilter("fxaa")
{
	set_float("fxaaSpanMax", 8.0f);
	set_float("fxaaReduceMin", 1.0f / 128.0f);
	set_float("fxaaReduceMul", 1.0f / 8.0f);
	set_float("fxaaAspectDistortion", 150.0f);

	set_sampler_slot("screenTexture", 0);
	set_texture("displayTexture", Texture(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y(), 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0));

	mFxaaFilter.set_shader("fxaa.vert", "fxaa.frag");
	mFxaaFilter.mShader->set_int("screenTexture", RenderEngine::get_sampler_slot("screenTexture"));

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

	skybox();
}

void RenderEngine::skybox()
{
	Cube skyBox("");
	Shader equirectangularToCubemapShader("cubemap.vert", "equirectangular_to_cubemap.frag");
	set_texture("skybox", Texture(512, 512, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_DEPTH_ATTACHMENT));
	//stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf("../res/textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	equirectangularToCubemapShader.use();

	equirectangularToCubemapShader.set_int("equirectangularMap", 0);
	equirectangularToCubemapShader.set_mat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	get_texture("skybox").bind_render_target();
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.set_mat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skyBox.render();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void RenderEngine::render(Entity& object)
{
	get_texture("displayTexture").bind_render_target();
	//Material::update_uniforms_constant_all();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

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
		
	post_processing(mFxaaFilter, get_texture("displayTexture"), 0);
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

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	filter.mShader->use();
	source.bind(0);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}