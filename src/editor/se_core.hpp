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

        void initImGui();
        void shutdownImGui();

        void updateEditor();
        void renderEditor();
    };
}

#endif // !SE_CORE_HPP