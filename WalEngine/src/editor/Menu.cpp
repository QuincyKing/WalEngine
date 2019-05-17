#include "Menu.h"
#include "../core/Window.h"
#include <iostream>

void Menu::make_menu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			show_menu_file();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			show_menu_edit();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			show_menu_window();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			show_gameobject();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			show_menu_help();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Menu::show_menu_file()
{
	//创建文档
	if (ImGui::MenuItem("New"))
	{

	}
	//打开文档
	if (ImGui::MenuItem("Open", "Ctrl+O"))
	{

	}
	//打开近期文档
	if (ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		ImGui::EndMenu();
	}
	//保存
	if (ImGui::MenuItem("Save", "Ctrl+S"))
	{

	}
	//另存为
	if (ImGui::MenuItem("Save As.."))
	{

	}

	ImGui::Separator();

	//导入模型
	if (ImGui::MenuItem("Import"))
	{

	}

	ImGui::Separator();

	if (ImGui::MenuItem("Quit", "ESC"))
	{
		Window::Inputs.set_key_down(Input::KEY_ESCAPE, true);
	}
}

void Menu::show_menu_edit()
{
	if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
	ImGui::Separator();
	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
}

void Menu::show_menu_window()
{
	//Hierarchy
	if (ImGui::MenuItem("Hierarchy"))
	{
		mHierarchyW = true;
	}
}

void Menu::show_gameobject()
{
	if (ImGui::MenuItem("Create Empty"))
	{

	}
}

void Menu::show_menu_help()
{
	if (ImGui::MenuItem("About"))
	{
		mAboutW = true;
	}
}
