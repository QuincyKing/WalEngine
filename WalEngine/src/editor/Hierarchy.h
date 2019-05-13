#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>

class Entity;

class Hierarchy
{
public:
	void show_hierarchy(std::vector<Entity*> root);

private:
	static int selected;
};
