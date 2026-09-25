#ifndef HIERARCHY_PANEL_HPP
#define HIERARCHY_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/editor/se_editor_selection.hpp"
#include "src/scene/le_scene.hpp"
#include "src/editor/se_mode_controller.hpp"

#include <vulkan/vulkan.h>

namespace se {
    class HierarchyPanel : public IEditorPanel {
    public:
        HierarchyPanel(le::LeScene& scene, EditorSelection& selection, ModeController& modeController);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        le::LeScene& editorScene_;
        EditorSelection& selection_;
        ModeController& modeController_;

        bool canEdit_ = true;
    };
}

#endif