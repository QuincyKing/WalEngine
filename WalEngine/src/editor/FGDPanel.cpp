#include "FGDPanel.h"
#include <limits.h>  
#include "../render/RenderEngine.h"

//init variances
float  FGDPanel::R_Scale = 1.0;
float  FGDPanel::R_Gamma = 1.0;

float  FGDPanel::R_Eta1 = 2.0;
float  FGDPanel::R_Eta2 = 0.65;

float  FGDPanel::R_KappaR = 1.0;
float  FGDPanel::R_KappaG = 0.01;
float  FGDPanel::R_KappaB = 0.01;

void FGDPanel::show()
{
	const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = 0.0f, f32_hi_a = +1.0f;
	const float g_lo = 0.001f, g_hi = 3.0f, k_lo = 0.0f;
	const float eta_lo = 1.0000f;
	const float drag_speed = 0.005f;
	ImGui::Text("Parameters:");
	ImGui::DragScalar("Scale", ImGuiDataType_Float, &R_Scale, 0.001, &g_lo, &g_hi);
	ImGui::DragScalar("Gamma", ImGuiDataType_Float, &R_Gamma, 0.005, &g_lo, &g_hi);
	ImGui::DragScalar("Eta1", ImGuiDataType_Float, &R_Eta1, 0.005, &g_lo, &g_hi);
	ImGui::DragScalar("Eta2", ImGuiDataType_Float, &R_Eta2, drag_speed, &g_lo, &g_hi);
	ImGui::DragScalar("KappaR", ImGuiDataType_Float, &R_KappaR, drag_speed, &k_lo, &g_hi);
	ImGui::DragScalar("KappaG", ImGuiDataType_Float, &R_KappaG, drag_speed, &k_lo, &g_hi);
	ImGui::DragScalar("KappaB", ImGuiDataType_Float, &R_KappaB, drag_speed, &k_lo, &g_hi);
	update_variances();
}

void FGDPanel::update_variances()
{
	RenderEngine::Data.set_float("Scale", R_Scale);
	RenderEngine::Data.set_float("Gamma", R_Gamma);
	RenderEngine::Data.set_float("Eta1", R_Eta1);
	RenderEngine::Data.set_float("Eta2", R_Eta2);
	RenderEngine::Data.set_float("KappaR", R_KappaR);
	RenderEngine::Data.set_float("KappaG", R_KappaG);
	RenderEngine::Data.set_float("KappaB", R_KappaB);
}