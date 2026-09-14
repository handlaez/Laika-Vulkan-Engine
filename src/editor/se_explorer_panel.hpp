#ifndef EXPLORER_PANEL_HPP
#define EXPLORER_PANEL_HPP

#include "i_editor_panel.hpp"

#include <vulkan/vulkan.h>

#include <filesystem>
#include <string>
#include <vector>

namespace se {

    class ExplorerPanel : public IEditorPanel {
    public:
        ExplorerPanel();

        void onImGuiRender() override;
        void onUpdate() override;

    private:
        void drawDirectory(const std::filesystem::path& path);
        void refresh();

    private:
        std::filesystem::path m_rootDirectory;
        std::filesystem::path m_selectedPath;

        bool m_shouldRefresh = false;
    };

}
#endif