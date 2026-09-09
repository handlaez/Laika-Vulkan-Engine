#ifndef SE_CORE_HPP
#define SE_CORE_HPP

#include "src/core/le_core.hpp"
#include "src/scene/le_scene.hpp"

namespace se {

    class SeCore
    {
    public:
        SeCore();
        ~SeCore();

        void run();

    private:
        le::LeCore leCore_;
        le::LeScene currentScene_;

        void createImGuiDescriptorPool();
        VkDescriptorPool imguiDescriptorPool_{ VK_NULL_HANDLE };
        VkDescriptorSet sceneTextureDescriptorSet_{ VK_NULL_HANDLE };

        void initImGui();
        void shutdownImGui();
        void renderImGui(VkCommandBuffer commandBuffer);
        void renderEditorWindows();
        void initializeDockspace();

        void updateEditor();
        void renderEditor();

        bool dockspaceInitialized_{ false };
    };
}

#endif // !SE_CORE_HPP