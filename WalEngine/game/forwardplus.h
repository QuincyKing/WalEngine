#pragma once

#include "game.h"
#include "../src/core/Model.h"
#include "../src/render/Texture.h"
#include "../src/component/Material.h"

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#define OBJECT_NUM 16

struct LightParticle
{
	glm::vec4	color;
	float		previous[4];
	float		current[4];
	float		velocity[3];
	float		radius;
};

class ForwardPlus : public Game
{
public:
	std::vector<Model> models;
	Texture coatNormalMap, normalMap, MainTex, RoughnessMap, OcclusionMap, BentNormal, GeomNormal;
	Material *mat;
	LightParticle *particles;
	Texture	FGDTexture;
	Shader	fgdShader;
	DirectionalLight dir;

public:
	ForwardPlus() : 
		fgdShader("brdf.vert", "pre_fgd_and_disney_diffuse.frag"),
		dir("dir2")
	{
		for (int i = 0; i < OBJECT_NUM; i++)
			models.push_back(Model("shaderball" + i, "mitsuba/mitsuba-sphere.obj"));
	}

	~ForwardPlus() {}

	void init();

	void render(RenderEngine &renderer);

	void precompute();

	void gui();
};