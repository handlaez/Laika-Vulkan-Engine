#ifndef SCENE_PANEL_HPP
#define SCENE_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/core/le_core.hpp" 

#include <vulkan/vulkan.h>

namespace se {
    class ScenePanel : public IEditorPanel {
    public:
        ScenePanel(le::LeCore& leCore, const VkDescriptorSet* textureSet, bool* isHovered);

        void onImGuiRender() override;
    private:
        le::LeCore& leCore_;
        const VkDescriptorSet* textureSet_;
        bool* isHovered_{ nullptr };
    };
}

#endif