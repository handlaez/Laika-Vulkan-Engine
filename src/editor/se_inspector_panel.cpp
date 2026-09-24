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
                actor->transform.rotation = glm::quat(glm::radians(eulerDegrees));
            }

            ImGui::DragFloat3("Scale", glm::value_ptr(actor->transform.scale), 0.05f);
        }

        if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& resources = editorScene_.leResourceManager;

            // Model selector
            auto currentModel = resources.findModel(actor->modelID);
            std::string currentModelName = "Unknown Model";

            if (currentModel)
            {
                currentModelName = currentModel->getName();

                if (currentModelName.empty())
                {
                    currentModelName = "Unnamed Model";
                }
            }

            if (ImGui::BeginCombo("Model", currentModelName.c_str()))
            {
                std::vector<uint32_t> modelIds;

                for (const auto& [id, model] : resources.getModels())
                {
                    modelIds.push_back(id);
                }

                std::sort(modelIds.begin(), modelIds.end());

                for (uint32_t id : modelIds)
                {
                    auto model = resources.findModel(id);

                    if (!model)
                    {
                        continue;
                    }

                    const bool selected = actor->modelID == id;

                    std::string modelName = model->getName();

                    if (modelName.empty())
                    {
                        modelName = "Unnamed Model";
                    }

                    // Visible text = modelName
                    // Internal ImGui ID = model_<resource ID>
                    const std::string label =
                        modelName + "##model_" + std::to_string(id);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        if (actor->modelID != id)
                        {
                            actor->modelID = id;
                            actor->clearBVH();
                        }
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            // Texture selector
            auto currentTexture = resources.findTexture(actor->textureID);
            std::string currentTextureName = "Unknown Texture";

            if (currentTexture)
            {
                currentTextureName = currentTexture->getName();

                if (currentTextureName.empty())
                {
                    currentTextureName = "Unnamed Texture";
                }
            }

            if (ImGui::BeginCombo("Texture", currentTextureName.c_str()))
            {
                std::vector<uint32_t> textureIds;

                for (const auto& [id, texture] : resources.getTextures())
                {
                    textureIds.push_back(id);
                }

                std::sort(textureIds.begin(), textureIds.end());

                for (uint32_t id : textureIds)
                {
                    auto texture = resources.findTexture(id);

                    if (!texture)
                    {
                        continue;
                    }

                    const bool selected = actor->textureID == id;

                    std::string textureName = texture->getName();

                    if (textureName.empty())
                    {
                        textureName = "Unnamed Texture";
                    }

                    const std::string label =
                        textureName + "##texture_" + std::to_string(id);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        actor->textureID = id;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
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