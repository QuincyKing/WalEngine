#include "Hierarchy.h"
#include <cstdio>

int Hierarchy::selected = -1;

void Hierarchy::show_hierarchy(std::vector<Entity*> root)
{	
	if (!root.empty())
	{
		for (auto e : root)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ((selected == e->mIndex) ? ImGuiTreeNodeFlags_Selected : 0);
			if (!(e->mChildren.empty()))
			{
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)e->mIndex, node_flags, (e->mName).c_str());
				if (ImGui::IsItemClicked())
					selected = e->mIndex;
				if (node_open)
				{
					show_hierarchy(e->mChildren);
					ImGui::TreePop();
				}
			}
			else
			{	
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx((void*)(intptr_t)e->mIndex, node_flags, (e->mName).c_str());
				if (ImGui::IsItemClicked())
					selected = e->mIndex;
			}
		}

	}

	return;
}