#include "se_console_panel.hpp"
#include "imgui.h"

se::ConsolePanel::ConsolePanel()
{
	// make sure only one exists.
}

void se::ConsolePanel::onImGuiRender()
{
	ImGui::Begin("Console");

	//render
	ImGui::Text("Console output");

	ImGui::End();
}

void se::ConsolePanel::onUpdate()
{
	// update output
}
