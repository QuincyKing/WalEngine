#include "Panel.h"
#include "../core/Entity.h"

float Panel::WinX = 0.0;
float Panel::WinY = 0.0;
Menu Panel::mMenu = Menu();
Hierarchy Panel::mHierarchy = Hierarchy();
ImVec2 Panel::mAboutSize = ImVec2(0.0, 0.0);

void Panel::make_panel()
{
	mMenu.make_menu();
	//if (mMenu.mAboutW)
	//	show_about();
	////if (mMenu.mHierarchyW)
	//if(false)
	//	show_hierarchy();
}

//ÏÔÊ¾About´°¿Ú
void Panel::show_about()
{
	ImGui::SetNextWindowPos(ImVec2((WinX - mAboutSize.x) / 2, 
		(WinY - mAboutSize.y) / 2), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(mAboutSize, ImGuiCond_Appearing);
	if (!ImGui::Begin("About", &mMenu.mAboutW, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
		return;
	}
	ImGui::TextWrapped("This is a Game Engine named WalEngine. This is not a business project, just a learning project. I hope to strengthen my knowledge through this project.");
	ImGui::End();
}

void Panel::show_hierarchy()
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Hierarchy", &mMenu.mHierarchyW))
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	mHierarchy.show(Entity::Root);
	ImGui::PopStyleVar();
	ImGui::End();
}
