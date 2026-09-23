#include "se_inspector_panel.hpp"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

namespace se {

    InspectorPanel::InspectorPanel(le::LeScene& scene, EditorSelection& selection, ModeController& modeController)
        : editorScene_(scene), selection_(selection), modeController_(modeController)
    {
    }

    void InspectorPanel::onImGuiRender()
    {
        ImGui::Begin("Inspector");

        const auto selectedId = selection_.getSelectedActor();

        if (!selectedId.has_value())
        {
            ImGui::TextUnformatted("No actor selected.");
            ImGui::End();
            return;
        }

        le::LeActor* actor = editorScene_.getActorById(*selectedId);

        if (actor == nullptr)
        {
            selection_.clear();
            ImGui::TextUnformatted("No actor selected.");
            ImGui::End();
            return;
        }

        const bool canEdit = modeController_.getState() == PlayState::Edit;

        ImGui::Text("Actor %u", actor->getId());

        ImGui::Separator();

        if (!canEdit)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Position", glm::value_ptr(actor->transform.translation), 0.05f);

            glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(actor->transform.rotation));

            if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerDegrees), 1.0f))
            {
                actor->transform.rotation =
                    glm::quat(glm::radians(eulerDegrees));
            }

            ImGui::DragFloat3("Scale", glm::value_ptr(actor->transform.scale), 0.05f);
        }

        if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Model ID: %u", actor->modelID);
            ImGui::Text("Texture ID: %u", actor->textureID);
        }

        if (!canEdit)
        {
            ImGui::EndDisabled();
        }

        ImGui::End();
    }

    void InspectorPanel::onUpdate()
    {
    }
}