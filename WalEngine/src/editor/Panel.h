#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Menu.h"
#include "Hierarchy.h"
#include "LayerMaterialPanel.h"

class Panel
{
public:
	Panel()
	{
		mAboutSize.y = 200;
		mAboutSize.x = 400;
	}
	virtual ~Panel() {}
	void make_panel();

private:
	void show_about();
	void show_hierarchy();
	void show_gameobject();
	void show_layer_material_panel();

private:
	Menu mMenu;
	Hierarchy mHierarchy;
	ImVec2 mAboutSize;
	LayerMaterialPanel mLayerMaterialPanel;

public:
	ImVec2 mScreen;
};
