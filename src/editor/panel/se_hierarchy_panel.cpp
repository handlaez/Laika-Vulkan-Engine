#include "se_hierarchy_panel.hpp"
#include "imgui.h"
#include "src/scene/le_scene.hpp"
#include "src/editor/se_editor_selection.hpp"
#include "src/editor/se_mode_controller.hpp"

namespace se {
    HierarchyPanel::HierarchyPanel(EditorContext& context)
        : context_(context)
    {
    }

    void HierarchyPanel::onImGuiRender()
    {
        ImGui::Begin("Hierarchy");

        if (!context_.scene || !context_.modeController)
        {
            ImGui::TextUnformatted("No project loaded.");
            ImGui::End();
            return;
        }

        auto& editorScene = *context_.scene;
        auto& selection = context_.selection;

        if (!canEdit_)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Add Cube"))
        {
            const auto id = editorScene.addActor(0, 0);
            selection.select(id);
        }

        ImGui::SameLine();

        const bool hasSelection = selection.hasSelection();

        if (!hasSelection)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Delete"))
        {
            const auto id = selection.getSelectedActor();

            if (id && editorScene.removeActorById(*id))
            {
                selection.clear();
            }
        }

        if (!hasSelection)
        {
            ImGui::EndDisabled();
        }

        ImGui::Separator();

        for (const auto& actor : editorScene.getActors())
        {
            const bool selected = selection.getSelectedActor() == actor.getId();

            const std::string label = "Actor " + std::to_string(actor.getId());

            if (ImGui::Selectable(label.c_str(), selected))
            {
                selection.select(actor.getId());
            }
        }

        if (!canEdit_)
        {
            ImGui::EndDisabled();
        }

        ImGui::End();
    }

    void HierarchyPanel::onUpdate()
    {
        if (!context_.modeController)
        {
            canEdit_ = false;
            return;
        }

        canEdit_ = context_.modeController->getState() == PlayState::Edit;
    }
}