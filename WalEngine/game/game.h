#pragma once

#include "../src/core/Window.h"
#include "../src/render/Shader.h"
#include "../src/core/Window.h"
#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>

#include "../src/core/Quaternion.h";
#include "../src/core/Model.h"
#include "../src/render/Texture.h"
#include "../src/model/sphere.h"
#include "../src/model/Cube.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "../src/render/RenderEngine.h"

class Game
{
public:
	Shader shader;
	Texture albedo, normal, metallic, roughness, ao;
	//Model model;
	Sphere sphere1;
	Sphere sphere2;
	Cube cube1;
	Sphere sphere4;
	Entity root;

public:
	Game() :
		sphere1("123455"),
		sphere2("223333"),
		cube1("cube"),
		sphere4("sphere4"),
		root("root")
	{}

	~Game()
	{
	}

	void init();

	void render(RenderEngine& renderer, Window& window);
};