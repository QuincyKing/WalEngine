#pragma once

#include "game.h"
#include "../src/core/Window.h"
#include "../src/core/Quaternion.h"
#include "../src/core/Model.h"
#include "../src/component/Material.h"
#include "../src/render/Texture.h"
#include "../src/model/Sphere.h"
#include "../src/model/Cube.h"

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Layered : public Game
{
public:
	Texture albedo, normal, metallic, roughness, ao;
	Model model;
	Model model2;
	Material *mat;
	Material *mat_layered;
	DirectionalLight dir;
	DirectionalLight dir2;
	Texture	FGDTexture;
	Texture	brdfLUTTexture;
	Texture coatNormalMap;
	Texture normalMap;
	Texture MainTex;
	Texture RoughnessMap;
	Texture OcclusionMap;
	Texture BentNormal;
	Texture GeomNormal;
	Shader	fgdShader;
	Shader	brdfShader;
	LayerMaterialPanel mLayerMaterialPanel;

public:
	Layered() :
		model("pbr-ball", "mitsuba/mitsuba-sphere.obj"),
		model2("layered-material-ball", "mitsuba/mitsuba-sphere.obj"),
		fgdShader("brdf.vert", "pre_fgd_and_disney_diffuse.frag"),
		brdfShader("brdf.vert", "brdf.frag"),
		dir("dir"),
		dir2("dir2")
	{}

	~Layered()
	{
	}

	void init();

	void render(RenderEngine &renderer);

	void precompute();

	void gui();
};