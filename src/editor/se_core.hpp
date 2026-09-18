#ifndef SE_CORE_HPP
#define SE_CORE_HPP

#include "src/core/le_core.hpp"
#include "src/scene/le_scene.hpp"
#include "src/render/le_scene_render_target.hpp"
#include "src/editor/i_editor_panel.hpp"
#include "src/logger/le_log_record.hpp"
#include "src/editor/se_mode_controller.hpp"
#include "src/laika_app.hpp"

namespace se {

    class SeCore
    {
    public:
        SeCore();
        ~SeCore();

        void run();

    private:
        le::LeCore leCore_;
        le::LeScene editorScene_;

        LaikaApp laikaApp_;
        ModeController modeController_;

        std::vector<std::unique_ptr<IEditorPanel>> editorPanels_;
        std::shared_ptr<std::vector<le::log::Record>> consoleLogRecords_;

        void createImGuiDescriptorPool();
        VkDescriptorPool imguiDescriptorPool_{ VK_NULL_HANDLE };
        VkDescriptorSet sceneTextureDescriptorSet_{ VK_NULL_HANDLE };

        void initImGui();
        void shutdownImGui();
        void renderImGui(VkCommandBuffer commandBuffer);
        void renderEditorWindows();
        void initializeDockspace();
        void recreateSceneTexture(le::LeSceneRenderTarget& sceneTarget);

        void updateEditor();
        void renderEditor();

        void renderPlayToolbar();

        bool dockspaceInitialized_{ false };
        bool sceneViewportHovered_{ false };
    };
}

#endif // !SE_CORE_HPP