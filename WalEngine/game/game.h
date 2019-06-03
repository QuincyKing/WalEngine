#pragma once

#include "../src/core/Window.h"
#include "../src/core/Quaternion.h";
#include "../src/core/Model.h"
#include "../src/component/Material.h"
#include "../src/render/Texture.h"
#include "../src/model/Sphere.h"
#include "../src/model/Cube.h"
#include "../src/render/RenderEngine.h"

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class Game
{
public:
	Texture albedo, normal, metallic, roughness, ao;
	Model model;
	Model model2;
	Entity renderRoot;
	Material *mat;
	Material *mat_layered;
	PointLight dir;
	PointLight dir2;

public:
	Game() :
		model("pbr-ball", "mitsuba/mitsuba-sphere.obj"),
		model2("layered-material-ball", "mitsuba/mitsuba-sphere.obj"),
		renderRoot("root"),
		dir("dir"),
		dir2("dir2")
	{}

	~Game()
	{
	}

	void init();

	void render(RenderEngine &renderer);
};