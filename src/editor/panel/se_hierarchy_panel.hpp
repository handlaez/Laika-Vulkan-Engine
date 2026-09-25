#ifndef HIERARCHY_PANEL_HPP
#define HIERARCHY_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/editor/se_editor_context.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class HierarchyPanel : public IEditorPanel {
    public:
        HierarchyPanel(EditorContext& context);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        EditorContext& context_;

        bool canEdit_ = true;
    };
}

#endif