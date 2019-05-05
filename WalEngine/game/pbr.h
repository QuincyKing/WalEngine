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

class Pbr : public Window
{
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> shader2;
	Texture albedo, normal, metallic, roughness, ao;
	Model model;
	Sphere sphere1;
	Sphere sphere2;
	Cube cube1;
	Sphere sphere4;
	Entity root;
	RenderEngine renderer;

public:
	Pbr(unsigned int _Width, unsigned int _Height) :
		Window(_Width, _Height),
		sphere1("123455"),
		sphere2("223333"),
		cube1("cube"),
		sphere4("sphere4"),
		root("root")
	{}

	~Pbr()
	{
	}

	void onrun()
	{
		Window::onrun();
	}

	void oninit()
	{
		albedo = Texture("pbr/plastic/albedo.png");
		normal = Texture("pbr/plastic/normal.png");
		metallic = Texture("pbr/plastic/metallic.png");
		roughness = Texture("pbr/plastic/roughness.png");
		ao = Texture("pbr/plastic/ao.png");

		Sphere::load();
		Cube::load();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		albedo.process();
		normal.process();
		metallic.process();
		roughness.process();
		ao.process();

		shader = std::make_shared<Shader>("./shader/pbr.vert", "./shader/pbr.frag");
		shader2 = std::make_shared<Shader>("./shader/default.vert", "./shader/default.frag");
		shader->use();

		shader->setInt("albedoMap", 0);
		shader->setInt("normalMap", 1);
		shader->setInt("metallicMap", 2);
		shader->setInt("roughnessMap", 3);
		shader->setInt("aoMap", 4);
		albedo.bind(0);
		normal.bind(1);
		metallic.bind(2);
		roughness.bind(3);
		ao.bind(4);

		renderer = RenderEngine(*this);
		sphere1.mTransform->set_pos(glm::vec3(1.0, 0.0, 0.0));
		sphere1.add_child(&sphere2);
		sphere2.add_child(&cube1);
		sphere2.mTransform->set_pos(glm::vec3(1.0, 1.0, 0.0));
		cube1.mTransform->set_pos(glm::vec3(-1.0, 0.0, 0.0));
		cube1.mTransform->set_rot(Quaternion(glm::vec3(1.0, 1.0, 0.0), glm::pi<float>() * 1 / 6));
		
		sphere1.set_shader(shader);
		sphere2.set_shader(shader);
		cube1.set_shader(shader);

		root.add_child(&sphere1);
		root.add_child(&sphere4);
	}

	void onupdate()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec2 curScreen;
		 
		curScreen.x = 5 * float((mInput.get_mouse_x()) - mInput.get_win_size_x() / 2.0f) / float(mInput.get_win_size_x());
		curScreen.y = 5 * float(0 - (mInput.get_mouse_y()) - mInput.get_win_size_y() / 2.0f) / float(mInput.get_win_size_y());
		glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 10.0f);
		glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);
		shader->use();
		
		//shader->setMat4("projection", projection);
		glm::mat4 view = mCamera.get_view_projection();
		shader->setMat4("vp", view);
		shader->setVec3("camPos", *(mCamera.get_transform()->get_pos()));
		shader->setVec3("lightPos", lightPosition + glm::vec3(curScreen, 0.0));
		shader->setVec3("lightColor", lightColor);

		shader2->use();
		shader2->setMat4("vp", view);

		renderer.render(root);
		//cube1.render(shader2);
		//model.draw(shader);
	}
};