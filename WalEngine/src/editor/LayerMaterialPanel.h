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

public:
	static float  depth1;
	static float  depth2;
	static float  alpha1;
	static float  alpha2;
	static float  eta1[3];
	static float  eta2[3];
	static float  eta3[3];
	static float  kappa1[3];
	static float  kappa2[3];
	static float  kappa3[3];
	static ImVec4 sigma_a1;
	static ImVec4 sigma_a2;
	static ImVec4 sigma_s1;
	static ImVec4 sigma_s2;
};