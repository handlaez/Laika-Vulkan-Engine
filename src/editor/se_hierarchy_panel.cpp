#include "se_hierarchy_panel.hpp"
#include "imgui.h"

se::HierarchyPanel::HierarchyPanel()
{
}

void se::HierarchyPanel::onImGuiRender()
{
    ImGui::Begin("Hierarchy");

    ImGui::Text("Scene hierarchy");

    ImGui::End();
}

void se::HierarchyPanel::onUpdate()
{
}
