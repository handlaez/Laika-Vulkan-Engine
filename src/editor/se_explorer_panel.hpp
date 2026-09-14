#ifndef EXPLORER_PANEL_HPP
#define EXPLORER_PANEL_HPP

#include "i_editor_panel.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace se {

    class ExplorerPanel : public IEditorPanel {
    public:
        explicit ExplorerPanel(const std::filesystem::path& rootDirectory);

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        void drawDirectory(const std::filesystem::path& path);
        void refresh();

    private:
        std::filesystem::path rootDirectory_;
        std::filesystem::path selectedPath_;

        bool shouldRefresh_ = false;
    };

}
#endif