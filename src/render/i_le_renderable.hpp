#ifndef I_LE_RENDERABLE_HPP
#define I_LE_RENDERABLE_HPP

#include <vulkan/vulkan.h>
#include <string>

namespace le {
    class LeRenderable {
    public:
        virtual ~LeRenderable() = default;

        const std::string& getName() const {
            return name_;
        }

        void setName(std::string name) {
            name_ = std::move(name);
        }

        virtual void bind(VkCommandBuffer commandBuffer) const = 0;
        virtual void draw(VkCommandBuffer commandBuffer) const = 0;

    private:
        std::string name_;
    };
}

#endif 