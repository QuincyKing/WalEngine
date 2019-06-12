#include "LayerMaterialPanel.h"
#include <limits.h>  
#include "../render/RenderEngine.h"

//init variances
float  LayerMaterialPanel::depth1 = 0.5f;
float  LayerMaterialPanel::alpha1 = 0.8f;
float  LayerMaterialPanel::g = 1.0f;
float  LayerMaterialPanel::eta1 = 1.49f;
ImVec4 LayerMaterialPanel::sigma_a1 = ImColor(1.0f, 1.0f, 1.0f);
ImVec4 LayerMaterialPanel::sigma_s1 = ImColor(0.0f, 0.0f, 0.0f);

void LayerMaterialPanel::show() 
{
	const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = 0.0f, f32_hi_a = +10000000000.0f;
	const float g_lo = 0.001f, g_hi = 100.0000001f;
	const float eta_lo = 1.0000f;
	const float drag_speed = 0.05f;
	ImGui::Text("Parameters:");
	ImGui::DragScalar("depth", ImGuiDataType_Float, &depth1, 0.001, &f32_lo_a, &f32_hi_a);
	ImGui::DragScalar("alpha1", ImGuiDataType_Float, &alpha1, 0.005, &g_lo, &g_hi);
	ImGui::DragScalar("g", ImGuiDataType_Float, &g, 0.005, &g_lo, &g_hi); 
	ImGui::DragScalar("eta1", ImGuiDataType_Float, &eta1, drag_speed, &eta_lo, &f32_hi_a);

	//color
	ImGui::Text("Color:");
	static bool alpha_preview = false;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	ImGui::Text("sigma_a1:  ");
	ImGui::SameLine(); ImGui::ColorEdit3("Color#1", (float*)&sigma_a1, ImGuiColorEditFlags_Float | misc_flags);

	ImGui::Text("sigma_s1:  ");
	ImGui::SameLine(); ImGui::ColorEdit3("Color#3", (float*)&sigma_s1, ImGuiColorEditFlags_Float | misc_flags);

	update_variances();
}

void LayerMaterialPanel::update_variances()
{
	RenderEngine::Data.set_float("depth1", depth1);
	RenderEngine::Data.set_float("alpha1", alpha1);
	RenderEngine::Data.set_float("g", g);
	RenderEngine::Data.set_float("eta1", eta1);
	RenderEngine::Data.set_vec3("sigma_a1", glm::vec3(sigma_a1.x, sigma_a1.y, sigma_a1.z));
	RenderEngine::Data.set_vec3("sigma_s1", glm::vec3(sigma_s1.x, sigma_s1.y, sigma_s1.z));
}