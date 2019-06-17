#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "UI.h"

class FGDPanel : public UI
{
public:
	FGDPanel() { }

	void show();

private:
	void update_variances();

public:
	static float  R_Scale;
	static float  R_Gamma;

	static float  R_Eta1;
	static float  R_Eta2;

	static float  R_KappaR;
	static float  R_KappaG;
	static float  R_KappaB;
};