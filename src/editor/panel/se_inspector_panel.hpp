#ifndef INSPECTOR_PANEL_HPP
#define INSPECTOR_PANEL_HPP

#include "i_editor_panel.hpp"

#include "src/editor/se_editor_context.hpp"
#include "src/render/i_le_renderable.hpp"
#include "src/objects/le_texture.hpp"

namespace se {
    class InspectorPanel : public IEditorPanel {
    public:
        InspectorPanel(EditorContext& context);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        EditorContext& context_;
    };
}

#endif