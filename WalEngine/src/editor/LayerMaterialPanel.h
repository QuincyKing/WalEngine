#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "UI.h"

class LayerMaterialPanel : public UI
{
public:
	LayerMaterialPanel(){ }

	void show();

private:
	void update_variances();

public:
	static float  depth1;
	static float  alpha1;
	static float  g;
	static float  eta1;
	static float  eta2;
	static ImVec4 sigma_a1;
	static ImVec4 sigma_s1;
};