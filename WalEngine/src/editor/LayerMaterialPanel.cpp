#include "LayerMaterialPanel.h"
#include <limits.h>  
#include "../render/RenderEngine.h"

//init variances
float  LayerMaterialPanel::x = 1.0f;
float  LayerMaterialPanel::y = 1.0f;
float  LayerMaterialPanel::metal = 0.0f;
float  LayerMaterialPanel::delectricIOR = 1.68f;
float  LayerMaterialPanel::coatSmoothness = 1.0f;
float  LayerMaterialPanel::coatIOR = 2.5f;
float LayerMaterialPanel::coatThickness = 0.25f;
ImVec4 LayerMaterialPanel::coatExtinction = ImColor(0.0f, 0.0f, 0.0f);

void LayerMaterialPanel::show() 
{
	const float drag_speed = 0.05f;
	const float one = 1.0f;
	const float zero = 0.0f;
	const float iorh = 3.0f;
	const float tile = 30;
	ImGui::Text("Base Layer:");
	ImGui::Text("Tiling:"); 
	ImGui::DragScalar("x", ImGuiDataType_Float, &x, drag_speed, &one, &tile);
	ImGui::DragScalar("y", ImGuiDataType_Float, &y, drag_speed, &one, &tile);
	ImGui::DragScalar("metallic", ImGuiDataType_Float, &metal, drag_speed, &zero, &one);
	ImGui::DragScalar("DelectricIOR", ImGuiDataType_Float, &delectricIOR, drag_speed, &one, &iorh);
	
	ImGui::Text("Coat Layer:");
	ImGui::DragScalar("Coat Smoothness", ImGuiDataType_Float, &coatSmoothness, drag_speed, &zero, &one);
	ImGui::DragScalar("Coat IOR", ImGuiDataType_Float, &coatIOR, drag_speed, &one, &iorh);
	ImGui::DragScalar("Coat Thickness", ImGuiDataType_Float, &coatThickness, drag_speed, &zero, &iorh);
	static bool alpha_preview = false;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	
	ImGui::ColorEdit3("Coat Extinction", (float*)&coatExtinction, ImGuiColorEditFlags_Float | misc_flags);

	update_variances();
}

void LayerMaterialPanel::update_variances()
{
	RenderEngine::Data.set_vec3("ST", glm::vec3(x, y, 0.0));
	RenderEngine::Data.set_float("metallic", metal);
	RenderEngine::Data.set_float("DelectricIOR", delectricIOR);
	RenderEngine::Data.set_float("CoatPerceptualRoughness", coatSmoothness);
	RenderEngine::Data.set_float("CoatIOR", coatIOR);
	RenderEngine::Data.set_float("CoatThickness", coatThickness);
	RenderEngine::Data.set_vec3("CoatExtinction", glm::vec3(coatExtinction.x, coatExtinction.y, coatExtinction.z));
}