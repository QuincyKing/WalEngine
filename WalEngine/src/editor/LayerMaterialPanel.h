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
	static float  x;
	static float  y;
	static float  metal;
	static float  delectricIOR;
	static float  coatSmoothness;
	static float  coatIOR;
	static float  coatThickness;
	static ImVec4 coatExtinction;
};