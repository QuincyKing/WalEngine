#pragma once

#include "src/core/Window.h"
#include "./src/core/Shader.h"
#include "./src/core/Window.h"
#include <stb_image.h>
#include <GLFW/glfw3.h>

#include "src/core/Quaternion.h";
//#include "./src/core/Model.h"
#include "./src/render/Texture.h"
#include <iostream>
#include <memory>

class Pbr : public Window
{
private:
	std::shared_ptr<Shader> shader;
	Texture albedo, normal, metallic, roughness, ao;
	unsigned int sphereVAO = 0;
	unsigned int indexCount;
	//Model model;

public:
	Pbr(unsigned int _Width, unsigned int _Height) : Window(_Width, _Height)
	{
		albedo = Texture("pbr/plastic/albedo.png");
		normal = Texture("pbr/plastic/normal.png");
		metallic = Texture("pbr/plastic/metallic.png");
		roughness = Texture("pbr/plastic/roughness.png");
		ao = Texture("pbr/plastic/ao.png");
	}

	~Pbr()
	{
	}

	void onrun()
	{
		Window::onrun();
	}

	void oninit()
	{
		//model = Model("C:/Users/QuincyKing/Desktop/WalnutEngine/res/objects/nanosuit/nanosuit.obj");
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		albedo.process();
		normal.process();
		metallic.process();
		roughness.process();
		ao.process();

		shader = std::make_shared<Shader>("./shader/pbr.vs", "./shader/pbr.fs");
		shader->use();

		shader->setInt("albedoMap", 0);
		shader->setInt("normalMap", 1);
		shader->setInt("metallicMap", 2);
		shader->setInt("roughnessMap", 3);
		shader->setInt("aoMap", 4);

		
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

		shader->use();
		shader->setMat4("model", model);
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
		glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom), (float)mInput.get_win_size_x() / (float)mInput.get_win_size_y(), 0.1f, 100.0f);
		shader->setMat4("projection", projection);
		glm::mat4 view = mCamera.GetViewMatrix();
		shader->setMat4("view", view);
		shader->setVec3("camPos", mCamera.Position);
		shader->setVec3("lightPos", lightPosition + glm::vec3(curScreen, 0.0));
		shader->setVec3("lightColor", lightColor);

		albedo.bind(0);
		normal.bind(1);
		metallic.bind(2);
		roughness.bind(3);
		ao.bind(4);

		RenderSphere();

		////model.Draw(shader);
	}

	void RenderSphere()
	{
		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = indices.size();

			std::vector<float> data;
			for (int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
			}
			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			float stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}
};