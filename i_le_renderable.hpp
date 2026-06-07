#ifndef I_LE_RENDERABLE_HPP
#define I_LE_RENDERABLE_HPP

#include <vulkan/vulkan.h>

namespace le {
    class LeRenderable {
    public:
        virtual ~LeRenderable() = default;

        virtual void bind(VkCommandBuffer commandBuffer) const = 0;
        virtual void draw(VkCommandBuffer commandBuffer) const = 0;
    };
}

#endif 