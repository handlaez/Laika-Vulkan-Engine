#include "se_inspector_panel.hpp"
#include "src/scene/le_scene.hpp"
#include "src/editor/se_editor_selection.hpp"
#include "src/editor/se_mode_controller.hpp"
#include "src/editor/se_editor_session.hpp"
#include "src/core/le_utils.hpp"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>
#endif
#include <fstream>
#include <cctype>

namespace se {

    InspectorPanel::InspectorPanel(EditorContext& context)
        : context_(context)
    {
    }

    void InspectorPanel::onImGuiRender()
    {
        ImGui::Begin("Inspector");

        if (!context_.scene || !context_.modeController)
        {
            ImGui::TextUnformatted("No project loaded.");
            ImGui::End();
            return;
        }

        auto& editorScene = *context_.scene;
        auto& selection = context_.selection;
        auto& resources = context_.resources;

        const auto selectedId = selection.getSelectedActor();

        if (!selectedId.has_value())
        {
            ImGui::TextUnformatted("No actor selected.");
            ImGui::End();
            return;
        }

        le::LeActor* actor = editorScene.getActorById(*selectedId);

        if (actor == nullptr)
        {
            selection.clear();
            ImGui::TextUnformatted("No actor selected.");
            ImGui::End();
            return;
        }

        const bool canEdit = context_.modeController->getState() == PlayState::Edit;

        ImGui::Text("Actor %u", actor->getId());
        ImGui::Separator();

        if (!canEdit)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::DragFloat3("Position", glm::value_ptr(actor->transform.translation), 0.05f))
            {
                context_.session->setSceneModified();
            }


            glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(actor->transform.rotation));
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerDegrees), 1.0f))
            {
                actor->transform.rotation = glm::quat(glm::radians(eulerDegrees));
                context_.session->setSceneModified();
            }

            if (ImGui::DragFloat3("Scale", glm::value_ptr(actor->transform.scale), 0.05f))
            {
                context_.session->setSceneModified();
            }
        }

        if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen))
        {
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
                modelIds.reserve(resources.getModels().size());

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

                    const std::string label = modelName + "##model_" + std::to_string(id);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        if (actor->modelID != id)
                        {
                            actor->modelID = id;
                            actor->clearBVH();

                            context_.session->setSceneModified();
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
                textureIds.reserve(resources.getTextures().size());

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

                    const std::string label = textureName + "##texture_" + std::to_string(id);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        actor->textureID = id;

                        context_.session->setSceneModified();
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
        }

        if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (size_t i = 0; i < actor->getScripts().size(); ++i)
            {
                auto& script = actor->getScripts()[i];
                ImGui::PushID(static_cast<int>(i));

                bool enabled = script.enabled;

                if (ImGui::Checkbox("##Enabled", &enabled))
                {
                    script.enabled = enabled;
                    context_.session->setSceneModified();
                }

                ImGui::SameLine();
                ImGui::TextUnformatted(script.path.empty() ? script.className.c_str() : script.path.generic_string().c_str());

                ImGui::PopID();
            }

            if (ImGui::Button("+ Add Script"))
            {
                addScriptPopupOpen_ = true;
                addScriptName_[0] = '\0';
                ImGui::OpenPopup("Add Script");
            }

            if (ImGui::BeginPopupModal("Add Script", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::InputText("Name", addScriptName_, sizeof(addScriptName_));

                ImGui::Spacing();

                if (ImGui::Button("Create"))
                {
                    createScript(*actor);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
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

    void InspectorPanel::createScript(le::LeActor& actor)
    {
        if (context_.session == nullptr)
        {
            return;
        }

        auto* project = context_.projects.getProject();

        if (project == nullptr)
        {
            return;
        }

        std::string scriptName = addScriptName_;

        if (scriptName.empty())
        {
            return;
        }

        // Very simple first-pass sanitization.
        for (const char character : scriptName)
        {
            if (!std::isalnum(static_cast<unsigned char>(character)) && character != '_')
            {
                return;
            }
        }

        const auto scriptDirectory = project->getScriptDirectory();
        const auto scriptPath = scriptDirectory / (scriptName + ".cs");

        if (std::filesystem::exists(scriptPath))
        {
            return;
        }

        std::filesystem::create_directories(scriptDirectory);

        const std::string className = project->getName() + "." + scriptName;

        std::ofstream file(scriptPath);

        if (!file)
        {
            return;
        }

        file
            << "using Laika;\n"
            << "\n"
            << "namespace " << project->getName() << ";\n"
            << "\n"
            << "public sealed class " << scriptName << " : MonoBehaviour\n"
            << "{\n"
            << "    public override void OnStart()\n"
            << "    {\n"
            << "    }\n"
            << "\n"
            << "    public override void OnUpdate(float deltaTime)\n"
            << "    {\n"
            << "    }\n"
            << "\n"
            << "    public override void OnDestroy()\n"
            << "    {\n"
            << "    }\n"
            << "}\n";

        file.close();

        le::LeScript script;
        script.path = std::filesystem::relative(scriptPath, project->getRoot()).lexically_normal();

        script.className = className;
        script.enabled = true;

        actor.addScript(std::move(script));
        context_.session->setSceneModified();

        Utils::openDefaultApplication(scriptPath);
    }
}