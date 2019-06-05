#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "UI.h"

class Menu : public UI
{
public:
	Menu()
	{
		mAboutW = false;
		mHierarchyW = true;
	}

	void make_menu();
	bool get_shutdown() { return mShutdown; }

private:
	void show_menu_file();
	void show_menu_edit();
	void show_menu_window();
	void show_gameobject();
	void show_menu_help();

private:
	bool mShutdown;

public:
	bool mAboutW;
	bool mHierarchyW;
};
