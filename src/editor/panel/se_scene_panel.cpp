#include "se_scene_panel.hpp"
#include "src/core/le_core.hpp"

#include "imgui.h"

namespace se {
	ScenePanel::ScenePanel(EditorContext& context, const VkDescriptorSet* textureSet, bool* isHovered)
		: context_(context), textureSet_(textureSet), isHovered_(isHovered) {}

    void ScenePanel::onImGuiRender() {
        ImGui::Begin("Scene");

        if (isHovered_) {
            *isHovered_ = ImGui::IsWindowHovered();
        }

        const ImVec2 availableSize = ImGui::GetContentRegionAvail();
        const VkExtent2D sceneExtent = context_.core.getRenderer().getSceneRenderTarget().getExtent();

        if (availableSize.x > 0.0f && availableSize.y > 0.0f && sceneExtent.width > 0 && sceneExtent.height > 0) {
            const float sceneAspect = static_cast<float>(sceneExtent.width) / static_cast<float>(sceneExtent.height);

            ImVec2 imageSize = availableSize;

            if (availableSize.x / availableSize.y > sceneAspect) {
                imageSize.x = availableSize.y * sceneAspect;
            }
            else {
                imageSize.y = availableSize.x / sceneAspect;
            }

            const float offsetX = (availableSize.x - imageSize.x) * 0.5f;
            const float offsetY = (availableSize.y - imageSize.y) * 0.5f;

            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

            if (textureSet_ != nullptr) {
                ImGui::Image(*textureSet_, imageSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
            }
        }

        ImGui::End();
    }
}