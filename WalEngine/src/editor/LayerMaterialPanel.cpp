#include "LayerMaterialPanel.h"
#include <limits.h>  
#include "../render/RenderEngine.h"

float  LayerMaterialPanel::depth1 = 0.0f;
float  LayerMaterialPanel::depth2 = 0.0f;
float  LayerMaterialPanel::alpha1 = 1.0f;
float  LayerMaterialPanel::alpha2 = 1.0f;
float  LayerMaterialPanel::eta1[3] = { 1.0f, 1.0f, 1.0f };
float  LayerMaterialPanel::eta2[3] = { 1.49f, 1.49f, 1.49f };
float  LayerMaterialPanel::eta3[3] = { 1.0f, 1.0f, 1.0f };
float  LayerMaterialPanel::kappa1[3] = { 0.0f, 0.0f, 0.0f };
float  LayerMaterialPanel::kappa2[3] = { 0.0f, 0.0f, 0.0f };
float  LayerMaterialPanel::kappa3[3] = { 1.0f, 0.1f, 0.1f };
ImVec4 LayerMaterialPanel::sigma_a1 = ImColor(1.0f, 1.0f, 1.0f);
ImVec4 LayerMaterialPanel::sigma_a2 = ImColor(1.0f, 1.0f, 1.0f);
ImVec4 LayerMaterialPanel::sigma_s1 = ImColor(0.0f, 0.0f, 0.0f);
ImVec4 LayerMaterialPanel::sigma_s2 = ImColor(0.0f, 0.0f, 0.0f);

void LayerMaterialPanel::show() 
{
	const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = 0.0f, f32_hi_a = +10000000000.0f;

	const float drag_speed = 0.05f;
	static bool drag_clamp = false;
	ImGui::Text("Parameters:");
	ImGui::DragScalar("depth1", ImGuiDataType_Float, &depth1, drag_speed, &f32_lo_a, &f32_hi_a);
	ImGui::DragScalar("depth2", ImGuiDataType_Float, &depth2, drag_speed, &f32_lo_a, &f32_hi_a);
	ImGui::DragScalar("alpha1", ImGuiDataType_Float, &alpha1, drag_speed, &f32_lo_a, &f32_hi_a);
	ImGui::DragScalar("alpha2", ImGuiDataType_Float, &alpha2, drag_speed, &f32_lo_a, &f32_hi_a);
	ImGui::DragFloat3("eta1", eta1, drag_speed, 0, f32_hi_a);
	ImGui::DragFloat3("eta2", eta2, drag_speed, 0, f32_hi_a);
	ImGui::DragFloat3("eta3", eta3, drag_speed, 0, f32_hi_a);
	ImGui::DragFloat3("kappa1", kappa1, drag_speed, 0, f32_hi_a);
	ImGui::DragFloat3("kappa2", kappa2, drag_speed, 0, f32_hi_a);
	ImGui::DragFloat3("kappa3", kappa3, drag_speed, 0, f32_hi_a);

	RenderEngine::Data.set_float("depth1", depth1); 
	RenderEngine::Data.set_float("depth2", depth2);
	RenderEngine::Data.set_float("alpha1", alpha1);
	RenderEngine::Data.set_float("alpha2", alpha2);
	RenderEngine::Data.set_vec3("eta1", glm::vec3(eta1[0], eta1[1], eta1[2]));
	RenderEngine::Data.set_vec3("eta2", glm::vec3(eta2[0], eta2[1], eta2[2]));
	RenderEngine::Data.set_vec3("eta3", glm::vec3(eta3[0], eta3[1], eta3[2]));
	RenderEngine::Data.set_vec3("kappa1", glm::vec3(kappa1[0], kappa1[1], kappa1[2]));
	RenderEngine::Data.set_vec3("kappa2", glm::vec3(kappa2[0], kappa2[1], kappa2[2]));
	RenderEngine::Data.set_vec3("kappa3", glm::vec3(kappa3[0], kappa3[1], kappa3[2]));
	RenderEngine::Data.set_vec3("sigma_a1", glm::vec3(sigma_a1.x, sigma_a1.y, sigma_a1.z));
	RenderEngine::Data.set_vec3("sigma_a2", glm::vec3(sigma_a2.x, sigma_a2.y, sigma_a2.z));
	RenderEngine::Data.set_vec3("sigma_s1", glm::vec3(sigma_s1.x, sigma_s1.y, sigma_s1.z));
	RenderEngine::Data.set_vec3("sigma_s2", glm::vec3(sigma_s2.x, sigma_s2.y, sigma_s2.z));

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

	ImGui::Text("sigma_a2: ");
	ImGui::SameLine(); ImGui::ColorEdit3("Color#2", (float*)&sigma_a2, ImGuiColorEditFlags_Float | misc_flags);

	ImGui::Text("sigma_s1:  ");
	ImGui::SameLine(); ImGui::ColorEdit3("Color#3", (float*)&sigma_s1, ImGuiColorEditFlags_Float | misc_flags);

	ImGui::Text("sigma_s2: ");
	ImGui::SameLine(); ImGui::ColorEdit3("Color#4", (float*)&sigma_s2, ImGuiColorEditFlags_Float | misc_flags);
}