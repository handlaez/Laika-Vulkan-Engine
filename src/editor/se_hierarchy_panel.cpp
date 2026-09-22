#include "se_hierarchy_panel.hpp"
#include "imgui.h"

namespace se {
    HierarchyPanel::HierarchyPanel(le::LeScene& scene, EditorSelection& selection, ModeController& modeController)
        : editorScene_(scene), selection_(selection), modeController_(modeController)
    {
    }

    void HierarchyPanel::onImGuiRender()
    {
        ImGui::Begin("Hierarchy");

        if (!canEdit_)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Add Cube"))
        {
            const auto id = editorScene_.addActor(0, 0);
            selection_.select(id);
        }

        ImGui::SameLine();

        const bool hasSelection = selection_.hasSelection();

        if (!hasSelection)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Delete"))
        {
            const auto id = selection_.getSelectedActor();

            if (id)
            {
                if (editorScene_.removeActorById(*id))
                {
                    selection_.clear();
                }
            }
        }

        if (!hasSelection)
        {
            ImGui::EndDisabled();
        }

        ImGui::Separator();

        for (const auto& actor : editorScene_.getActors())
        {
            const bool selected = selection_.getSelectedActor() == actor.getId();
            const std::string label = "Actor " + std::to_string(actor.getId());

            if (ImGui::Selectable(label.c_str(), selected))
            {
                selection_.select(actor.getId());
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
        canEdit_ = modeController_.getState() == PlayState::Edit;
    }
}