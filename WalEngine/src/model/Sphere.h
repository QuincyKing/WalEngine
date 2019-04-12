#pragma once

#include "../core/Entity.h"
#include <GLFW/glfw3.h>

class Sphere : public Entity
{
public:
	Sphere(
		std::string name,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		Quaternion rot = Quaternion(0, 0, 0, 1),
		glm::vec3 scale = glm::vec3(1.0)):
		Entity(name, pos, rot, scale)
	{
	}

	void render();
	static void load();

private:
	static unsigned int vao;
	static unsigned int count;
};