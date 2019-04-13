#include "Renderer.h"
#include "../core/Window.h"
#include "../core/Mesh.h"
#include "Shader.h"

#include "../core/Entity.h"

#include <glad/glad.h>
#include <cassert>

glm::mat4 s = glm::mat4();
glm::mat4 t = glm::mat4();
//This matrix will convert 3D coordinates from the range (-1, 1) to the range (0, 1).
const glm::mat4 Renderer::BIAS_MATRIX = glm::scale(s, glm::vec3(0.5, 0.5, 0.5)) * glm::translate(t, glm::vec3(1.0, 1.0, 1.0));

Renderer::Renderer(const Window& window) :
	mWindow(&window),
	mPlane("Display"),
	//mTempTarget(),
	mDefaultShader("../../shader/forward-ambient.vert", "../../shader/forward-ambient.frag"),
	//mShadowMapShader("../../shader/shadowmapgen.vert", "../../shader/shadowmapgen.frag"),
	mNullFilter("../../shader/filter-null.vert", "../../shader/filter-null.frag"),
	//mLightShader("", ""),
	//mGausBlurFilter("../../shader/filter-gausBlur7x1.vs", "./shader/filter-gausBlur7x1.fs"),
	//mFxaaFilter("./shader/filter-fxaa.vs", "./shader/filter-fxaa.fs"),
	mAltCameraTransform(glm::vec3(0, 0, 0), Quaternion(glm::vec3(0, 1, 0), glm::radians(180.0f))),
	mAltCamera(glm::mat4(), make_shared<Transform>(mAltCameraTransform))
{
	/*mTempTarget.process(
		Window::mInput.get_win_size_x(), 
		Window::mInput.get_win_size_y(), 
		0, 
		GL_TEXTURE_2D, 
		GL_NEAREST, 
		GL_RGBA, 
		GL_RGBA, 
		false, 
		GL_COLOR_ATTACHMENT0);*/

	set_sampler_slot("filterTexture", 0);

	Texture t = Texture();
	t.process(
		Window::mInput.get_win_size_x(),
		Window::mInput.get_win_size_y(),
		0, 
		GL_TEXTURE_2D, 
		GL_LINEAR, 
		GL_RGBA, 
		GL_RGBA, 
		true, 
		GL_COLOR_ATTACHMENT0
	);

	mData.set_texture("displayTexture", t);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::apply_filter(Shader& filter, const Texture& source, const Texture* dest)
{
	assert(&source != dest);
	if (dest == 0)
	{
		mWindow->bind_render_target();
	}
	else
	{
		dest->bind_render_target();
	}

	mData.set_texture("filterTexture", source);

	filter.setInt("filterTexture", 0);
	mData.get_texture("filterTexture").bind(0);

	mAltCamera.get_transform()->set_pos(glm::vec3(0, 0, 0));
	mAltCamera.get_transform()->set_rot(Quaternion(glm::vec3(0, 1, 0), glm::radians(180.0f)));

	glClear(GL_DEPTH_BUFFER_BIT);
	//filter.UpdateUniforms(mPlaneTransform, m_planeMaterial, *this, mAltCamera);
	filter.use();
	
	mPlane.draw();

}

void Renderer::render(const Entity& object)
{
	mData.get_texture("displayTexture").bind_render_target();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	object.render_all(mDefaultShader, *this, *mMainCamera);

	float displayTextureAspect = (float)(mData.get_texture("displayTexture").get_width()) / (float)(mData.get_texture("displayTexture").get_height());

	apply_filter(mNullFilter, mData.get_texture("displayTexture"), 0);
}
