#pragma once

#include "game.h"
#include "../src/core/Model.h"
#include "../src/render/Texture.h"
#include "../src/component/Material.h"
#include "../src/model/Cube.h"

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#define OBJECT_NUM 16
#define NUM_LIGHTS 25
#define LIGHT_RADIUS  3.0f
#define INTERAL 3

struct PointLightData
{
	glm::vec4 color;
	glm::vec4 position;
};

class ForwardPlus : public Game
{
public:
	std::vector<Model> models;
	Texture coatNormalMap, normalMap, MainTex, RoughnessMap, OcclusionMap, BentNormal, GeomNormal;
	Material *mat, *mat2;
	PointLightData *particles;
	Texture	FGDTexture;
	Shader	fgdShader;
	DirectionalLight dir;
	GLuint	headbuffer = 0;	// head of linked lists
	GLuint	nodebuffer = 0;	// nodes of linked lists
	GLuint	lightbuffer = 0;	// light particles
	GLuint	counterbuffer = 0;	// atomic counter
	GLuint	workgroupsx = 0;
	GLuint	workgroupsy = 0;
	Shader	lightcull;
	Shader	zbuffer;
	FrameBuffer framebuffer;
	Texture tmp;
	Cube plane;
	Material *wood;
	Texture woodTex;

public:
	ForwardPlus() :
		fgdShader("brdf.vert", "pre_fgd_and_disney_diffuse.frag"),
		lightcull(GL_COMPUTE_SHADER, "light_cull.comp"),
		dir("dir2"),
		plane("plane"),
		framebuffer(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y()),
		tmp(Window::Inputs.get_win_size_x(), Window::Inputs.get_win_size_y(), 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true)
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