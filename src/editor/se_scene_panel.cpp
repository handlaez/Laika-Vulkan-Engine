#include "se_scene_panel.hpp"
#include "imgui.h"

namespace se {
	ScenePanel::ScenePanel(le::LeCore& leCore, const VkDescriptorSet* textureSet, bool* isHovered)
		: leCore_(leCore), textureSet_(textureSet), isHovered_(isHovered) {}

    void ScenePanel::onImGuiRender() {
        ImGui::Begin("Scene");

        if (isHovered_) {
            *isHovered_ = ImGui::IsWindowHovered();
        }

        const ImVec2 availableSize = ImGui::GetContentRegionAvail();
        const VkExtent2D sceneExtent = leCore_.getRenderer().getSceneRenderTarget().getExtent();

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

            if (textureSet_ != VK_NULL_HANDLE) {
                ImGui::Image(*textureSet_, imageSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
            }
        }

        ImGui::End();
    }
}