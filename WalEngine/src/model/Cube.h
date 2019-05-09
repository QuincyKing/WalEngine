#pragma once

#include "../core/Entity.h"
#include <GLFW/glfw3.h>

class Cube : public Entity
{
public:
	Cube(std::string name,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0)) :
		Entity(name, pos, rot, scale)
	{
	}

	void render(const Shader &shader);
	static void load();
	void draw();

private:
	static unsigned int vao;
	static unsigned int count;
};