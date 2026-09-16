#include "se_inspector_panel.hpp"
#include "imgui.h"

se::InspectorPanel::InspectorPanel()
{
	// constructor
}

void se::InspectorPanel::onImGuiRender()
{
	ImGui::Begin("Inspector");

	ImGui::Text("No entity selected");

	ImGui::End();
}

void se::InspectorPanel::onUpdate()
{
	//update
}
