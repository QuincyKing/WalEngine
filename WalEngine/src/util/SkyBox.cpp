#include "SkyBox.h"
#include "../core/Window.h"
#include "../render/FrameBuffer.h"
#include <stb_image/stb_image.h>

glm::mat4 SkyBox::CubeProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 SkyBox::CubeViews[6] = 
{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

SkyBox::SkyBox(std::string hdrfile) :
	mBox("box"),
	mSkyboxShader("background.vert", "background.frag"),
	mToCubemapShader("cubemap.vert", "equirectangular_to_cubemap.frag"),
	mHdr(hdrfile)
{
	mSkyboxShader.use();
	mSkyboxShader.set_int("environmentMap", 0);
	mSkyboxShader.set_mat4("projection", glm::mat4(1));

	stbi_set_flip_vertically_on_load(true);
	mHdr.process(GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_RGB16F);
}

void SkyBox::precompute()
{
	FrameBuffer envFrame(512, 512);
	mEnvCubemap = Texture(512, 512, 0, GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_RGB16F, GL_RGB, true);

	mToCubemapShader.use();
	mToCubemapShader.set_int("equirectangularMap", 0);
	mToCubemapShader.set_mat4("projection", CubeProjection);
	mHdr.bind(0);

	envFrame.bind_render_target();
	envFrame.bind_texture(mEnvCubemap.get_ID()[0]);
	for (unsigned int i = 0; i < 6; ++i)
	{
		mToCubemapShader.set_mat4("view", CubeViews[i]);
		envFrame.bind_texture(mEnvCubemap.get_ID()[0], GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mBox.draw();
	}
	FrameBuffer::bind_render_targer_reset();
	glViewport(0, 0, Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y());
}

void SkyBox::render()
{
	mEnvCubemap.bind(0);
	mSkyboxShader.use();
	mSkyboxShader.set_mat4("view", Window::MainCamera.get_view_projection());
	mBox.draw();
}
