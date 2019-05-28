#pragma once

#include "../src/core/Window.h"
#include "../src/core/Quaternion.h";
//#include "../src/core/Model.h"
#include "../src/render/Material.h"
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
	//Model model;
	Sphere sphere1;
	Sphere sphere2;
	Cube cube1;
	Sphere sphere4;
	Entity root;
	std::shared_ptr<Material> mat;
	PointLight dir;

public:
	Game() :
		sphere1("123455"),
		sphere2("223333"),
		cube1("cube"),
		sphere4("sphere4"),
		root("root"),
		dir("dl")
	{}

	~Game()
	{
	}

	void init();

	void render(RenderEngine &renderer);
};