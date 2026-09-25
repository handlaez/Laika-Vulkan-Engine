#ifndef EXPLORER_PANEL_HPP
#define EXPLORER_PANEL_HPP

#include "i_editor_panel.hpp"
#include "src/editor/se_editor_context.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace se {

    class ExplorerPanel : public IEditorPanel {
    public:
        explicit ExplorerPanel(EditorContext& context);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        void drawDirectory(const std::filesystem::path& path);
        void refresh();

    private:
        EditorContext& context_;

        std::filesystem::path selectedPath_;

        bool shouldRefresh_ = false;
    };

}
#endif