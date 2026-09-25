#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/editor/se_editor_context.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class ScenePanel : public IEditorPanel {
    public:
        ScenePanel(EditorContext& context, const VkDescriptorSet* textureSet, bool* isHovered);

        void onImGuiRender() override;
    private:
        EditorContext& context_;
        const VkDescriptorSet* textureSet_;
        bool* isHovered_{ nullptr };
    };
}

#endif