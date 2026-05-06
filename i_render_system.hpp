#ifndef I_LAIKA_ENGINE_APP_HPP
#define I_LAIKA_ENGINE_APP_HPP

#include "le_scene.hpp"

namespace le {
    struct RenderFrameData {
        VkCommandBuffer cmd;
        LeCamera& camera;
        size_t frameIndex;
        VkDescriptorSet frameDescriptorSet;
    };

    class IRenderSystem
    {
    public:
        virtual ~IRenderSystem() = default;

        virtual void render(const RenderFrameData& frameData, std::vector<LeActor>& actors) = 0;
    };
}
#endif