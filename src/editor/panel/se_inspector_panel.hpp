#ifndef INSPECTOR_PANEL_HPP
#define INSPECTOR_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/scene/le_scene.hpp"
#include "src/editor/se_editor_selection.hpp"
#include "src/editor/se_mode_controller.hpp"

#include "src/render/i_le_renderable.hpp"
#include "src/objects/le_texture.hpp"

namespace se {
    class InspectorPanel : public IEditorPanel {
    public:
        InspectorPanel(le::LeScene& scene, EditorSelection& selection, ModeController& modeController);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        le::LeScene& editorScene_;
        EditorSelection& selection_;
        ModeController& modeController_;

        const std::unordered_map<uint32_t, std::shared_ptr<le::LeRenderable>>& getModels() const;
        const std::unordered_map<uint32_t, std::shared_ptr<le::LeTexture>>& getTextures() const;
    };
}

#endif