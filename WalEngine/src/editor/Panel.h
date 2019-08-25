#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Menu.h"
#include "Hierarchy.h"
#include "LayerMaterialPanel.h"
#include "FGDPanel.h"

class Panel
{
public:
	Panel()
	{
		mAboutSize.y = 200;
		mAboutSize.x = 400;
	}
	virtual ~Panel() {}

public:
	static void show_about();
	static void show_hierarchy();
	static void show_gameobject();
	static void make_panel();

public:
	static float WinX;
	static float WinY;
	static Menu mMenu;
	static Hierarchy mHierarchy;
	static ImVec2 mAboutSize;
};
