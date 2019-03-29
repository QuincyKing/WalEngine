#pragma once

#include "../core/Model.h"
#include "../core/Window.h"
#include "../core/Data.h"
#include "../core/Entity.h"
#include "Texture.h"
#include "../core/Camera.h"

#include <vector>
#include <map>
class Entity;

#define NUM_SHADOW_MAPS 10

class Renderer
{
public:
	Renderer(const Window& window);
	virtual ~Renderer() {}

	void render(const Entity& object);

	//inline void add_light(const BaseLight& light) { mLights.push_back(&light); }
	inline void set_main_camera(const Camera& camera) { mMainCamera = &camera; }

	/*virtual void UpdateUniformStruct(const Transform& transform, const Material& material, const Shader& shader,
		const std::string& uniformName, const std::string& uniformType) const
	{
		throw uniformType + " is not supported by the rendering engine";
	}*/

	//inline const BaseLight& get_active_light()                           const { return *mActiveLight; }
	inline unsigned int get_sampler_slot(const std::string& samplerName) const { return mSamplerMap.find(samplerName)->second; }
	inline const glm::mat4& get_light_matrix()                            const { return mLightMatrix; }

protected:
	inline void set_sampler_slot(const std::string& name, unsigned int value) { mSamplerMap[name] = value; }

private:
	static const glm::mat4				BIAS_MATRIX;
	Data								mData;
	//Transform                           mPlaneTransform;
	//Model                               mPlane;

	const Window*                       mWindow;
	//Texture                             mTempTarget;
	Texture                             mShadowMaps[NUM_SHADOW_MAPS];
	Texture                             mShadowMapTempTargets[NUM_SHADOW_MAPS];

	Shader                              mDefaultShader;
	Shader                              mShadowMapShader;
	Shader                              mNullFilter;
	//Shader                              mGausBlurFilter;
	//Shader                              mFxaaFilter;
	glm::mat4                           mLightMatrix;

	Transform                           mAltCameraTransform;
	Camera                              mAltCamera;
	const Camera*                       mMainCamera;
	//const BaseLight*                    mActiveLight;
	//std::vector<const BaseLight*>       mLights;
	std::map<std::string, unsigned int> mSamplerMap;

	//void blur_shadow_map(int shadowMapIndex, float blurAmount);
	void apply_filter(const shared_ptr<Shader>& filter, const Texture& source, const Texture* dest);

	Renderer(const Renderer& other) :
		mAltCamera(glm::mat4(), 0) {}
	void operator=(const Renderer& other) {}
};
