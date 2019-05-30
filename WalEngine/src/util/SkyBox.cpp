#include "SkyBox.h"
#include "../core/Window.h"
#include <stb_image/stb_image.h>

SkyBox::SkyBox(std::string hdrfile):
	mBox("box"),
	mSkyboxShader("background.vert", "background.frag"),
	mToCubemapShader("cubemap.vert", "equirectangular_to_cubemap.frag")
{
	mSkyboxShader.use();
	mSkyboxShader.set_int("environmentMap", 0);
	mSkyboxShader.set_mat4("projection", glm::mat4(1));
	Texture hdr(hdrfile);
	hdr.process(GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_RGB16F);
	hdr.bind(0);
}

void SkyBox::precompute()
{
	stbi_set_flip_vertically_on_load(true);
	mEnvCubemap = Texture(512, 512, 0, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_RGB16F, GL_RGB, true, GL_DEPTH_ATTACHMENT);

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	captureViews = views;
	mToCubemapShader.use();
	mToCubemapShader.set_int("equirectangularMap", 0);
	mToCubemapShader.set_mat4("projection", captureProjection);

	mEnvCubemap.bind_render_target();
	for (unsigned int i = 0; i < 6; ++i)
	{
		mToCubemapShader.set_mat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mEnvCubemap.get_ID()[0], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//renderCube();
		mBox.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y());
}


void SkyBox::render()
{
	mEnvCubemap.bind(0);
	mSkyboxShader.use();
	mSkyboxShader.set_mat4("view", Window::MainCamera.get_view_projection());
	mBox.draw();
}
