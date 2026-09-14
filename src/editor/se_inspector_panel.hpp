#ifndef INSPECTOR_PANEL_HPP
#define INSPECTOR_PANEL_HPP

#include "i_editor_panel.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class InspectorPanel : public IEditorPanel {
    public:
        InspectorPanel();

        void onImGuiRender() override;
        void onUpdate() override;
    };
}

#endif