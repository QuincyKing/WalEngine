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
	//mTempTarget(),
	mDefaultShader("../../shader/pbr.vert", "../../shader/pbr.frag"),
	mShadowMapShader("../../shader/shadowmapgen.vert", "../../shader/shadowmapgen.frag"),
	mNullFilter("../../shader/filter-null.vert", "../../shader/filter-null.frag"),
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
	set_sampler_slot("albedoMap", 0);
	set_sampler_slot("normalMap", 1);
	set_sampler_slot("metallicMap", 2);
	set_sampler_slot("roughnessMap", 3);
	set_sampler_slot("aoMap", 4);

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

	//??????????
	//glFrontFace(GL_CW);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	/*mPlaneTransform.set_scale(glm::vec3(1));
	mPlaneTransform.set_rot(Quaternion(glm::vec3(1, 0, 0), glm::radians(90.0f)));
	mPlaneTransform.set_rot(Quaternion(glm::vec3(0, 0, 1), glm::radians(180.0f)));*/

	for (int i = 0; i < NUM_SHADOW_MAPS; i++)
	{
		int shadowMapSize = 1 << (i + 1);
		Texture t = Texture();
		t.process(
			shadowMapSize,
			shadowMapSize,
			0,
			GL_TEXTURE_2D,
			GL_LINEAR,
			GL_RG32F,
			GL_RGBA,
			true,
			GL_COLOR_ATTACHMENT0);
		mShadowMaps[i] = t;
		Texture tt = Texture();
		tt.process(
			shadowMapSize,
			shadowMapSize,
			0,
			GL_TEXTURE_2D,
			GL_LINEAR,
			GL_RG32F,
			GL_RGBA,
			true,
			GL_COLOR_ATTACHMENT0);
		mShadowMapTempTargets[i] = tt;
	}
	
	mLightMatrix = glm::scale(glm::mat4(), glm::vec3(0, 0, 0));
}

//void Renderer::blur_shadow_map(int shadowMapIndex, float blurAmount)
//{
//	mData.set_vec3("blurScale", glm::vec3(blurAmount / (mShadowMaps[shadowMapIndex].get_width()), 0.0f, 0.0f));
//	apply_filter(std::make_shared<Shader>(mGausBlurFilter), mShadowMaps[shadowMapIndex], &mShadowMapTempTargets[shadowMapIndex]);
//
//	mData.set_vec3("blurScale", glm::vec3(0.0f, blurAmount / (mShadowMaps[shadowMapIndex].get_height()), 0.0f));
//	apply_filter(std::make_shared<Shader>(mGausBlurFilter), mShadowMapTempTargets[shadowMapIndex], &mShadowMaps[shadowMapIndex]);
//}

void Renderer::apply_filter(const shared_ptr<Shader>& filter, const Texture& source, const Texture* dest)
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

	mAltCamera.get_transform()->set_pos(glm::vec3(0, 0, 0));
	mAltCamera.get_transform()->set_rot(Quaternion(glm::vec3(0, 1, 0), glm::radians(180.0f)));

	glClear(GL_DEPTH_BUFFER_BIT);
	filter.UpdateUniforms(mPlaneTransform, m_planeMaterial, *this, mAltCamera);
	mPlane.draw(filter);

	mData.set_texture("filterTexture", Texture());
}

void Renderer::render(const Entity& object)
{
	mData.get_texture("displayTexture").bind_render_target();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	object.render_all(mDefaultShader, *this, *mMainCamera);

	for (unsigned int i = 0; i < mLights.size(); i++)
	{
		mActiveLight = mLights[i];
		ShadowInfo shadowInfo = mActiveLight->get_shadowinfo();

		int shadowMapIndex = 0;
		if (shadowInfo.get_shadowmap_size_as_powerof2() != 0)
			shadowMapIndex = shadowInfo.get_shadowmap_size_as_powerof2() - 1;

		assert(shadowMapIndex >= 0 && shadowMapIndex < NUM_SHADOW_MAPS);

		mData.set_texture("shadowMap", mShadowMaps[shadowMapIndex]);
		mShadowMaps[shadowMapIndex].bind_render_target();
		glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (shadowInfo.get_shadowmap_size_as_powerof2() != 0)
		{
			mAltCamera.set_projection(shadowInfo.get_projection());
			ShadowCameraTransform shadowCameraTransform = mActiveLight->calc_shadow_camera_transform(
				mMainCamera->get_transform().get_transform_pos(),
				mMainCamera->get_transform().get_transform_rot());
			mAltCamera.get_transform()->set_pos(shadowCameraTransform.get_pos());
			mAltCamera.get_transform()->set_rot(shadowCameraTransform.get_rot());

			mLightMatrix = BIAS_MATRIX * mAltCamera.get_view_projection();

			mData.set_float("shadowVarianceMin", shadowInfo.get_min_variance());
			mData.set_float("shadowLightBleedingReduction", shadowInfo.get_light_bleed_reduction_amount());
			bool flipFaces = shadowInfo.get_flip_faces();

			if (flipFaces)
			{
				glCullFace(GL_FRONT);
			}

			glEnable(GL_DEPTH_CLAMP);
			object.render_all(mShadowMapShader, *this, mAltCamera);
			glDisable(GL_DEPTH_CLAMP);

			if (flipFaces)
			{
				glCullFace(GL_BACK);
			}

			float shadowSoftness = shadowInfo.get_shadow_softness();
			if (shadowSoftness != 0)
			{
				blur_shadow_map(shadowMapIndex, shadowSoftness);
			}
		}
		else
		{
			mLightMatrix = glm::scale(glm::mat4(), glm::vec3(0, 0, 0));
			mData.set_float("shadowVarianceMin", 0.00002f);
			mData.set_float("shadowLightBleedingReduction", 0.0f);
		}

		mData.get_texture("displayTexture").bind_render_target();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_EQUAL);

		object.render_all(mActiveLight->get_shader(), *this, *mMainCamera);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
	}

	float displayTextureAspect = (float)(mData.get_texture("displayTexture").get_width()) / (float)(mData.get_texture("displayTexture").get_height());
	float displayTextureHeightAdditive = displayTextureAspect * mData.get_float("fxaaAspectDistortion");
	mData.set_vec3("inverseFilterTextureSize", glm::vec3(1.0f / (float)(mData.get_texture("displayTexture").get_width()),
		1.0f / ((float)(mData.get_texture("displayTexture").get_height()) + displayTextureHeightAdditive), 0.0f));

	apply_filter(std::make_shared<Shader>(mNullFilter), mData.get_texture("displayTexture"), 0);
}
