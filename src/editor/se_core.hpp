#ifndef SE_CORE_HPP
#define SE_CORE_HPP

#include "src/core/le_core.hpp"
#include "src/scene/le_scene.hpp"
#include "src/render/le_scene_render_target.hpp"
#include "src/editor/panel/i_editor_panel.hpp"
#include "src/logger/le_log_record.hpp"
#include "src/systems/keyboard_movement_controller.hpp"
#include "src/editor/project/se_project_manager.hpp"
#include "src/editor/se_editor_context.hpp"
#include "src/editor/se_editor_session.hpp"

#include <filesystem>

namespace se {

    class SeCore
    {
    public:
        SeCore();
        ~SeCore();

        void run();

    private:
        le::LeCore leCore_;

        EditorSelection editorSelection_;
        ProjectManager projectManager_;
        EditorContext editorContext_;
        EditorSession editorSession_;

        le::KeyboardMovementController editorCameraController_;

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
        void updateEditorCamera();

        void renderPlayToolbar();
        void renderProjectPopups();

        bool dockspaceInitialized_{ false };
        bool sceneViewportHovered_{ false };

        bool openNewProjectPopup_{ false };
        bool openOpenProjectPopup_{ false };

        std::array<char, 256> newProjectName_{};
        std::array<char, 512> newProjectDirectory_{};
        std::array<char, 512> openProjectFile_{};
    };
}

#endif // !SE_CORE_HPP