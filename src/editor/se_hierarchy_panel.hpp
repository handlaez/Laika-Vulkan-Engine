#ifndef HIERARCHY_PANEL_HPP
#define HIERARCHY_PANEL_HPP

#include "i_editor_panel.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class HierarchyPanel : public IEditorPanel {
    public:
        HierarchyPanel();

        void onImGuiRender() override;
        void onUpdate() override;
    };
}

#endif