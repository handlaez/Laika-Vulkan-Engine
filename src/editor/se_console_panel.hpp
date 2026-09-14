#ifndef CONSOLE_PANEL_HPP
#define CONSOLE_PANEL_HPP

#include "i_editor_panel.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class ConsolePanel : public IEditorPanel {
    public:
        ConsolePanel();

        void onImGuiRender() override;
        void onUpdate() override;
    };
}

#endif