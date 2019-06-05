#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>

#include "UI.h"

class Entity;

class Hierarchy : public UI
{
public:
	void show(std::vector<Entity*> root);

private:
	static int selected;
};
