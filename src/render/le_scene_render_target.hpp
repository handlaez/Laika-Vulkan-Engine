#ifndef LE_SCENE_RENDER_TARGET_HPP
#define LE_SCENE_RENDER_TARGET_HPP

#include "src/core/le_device.hpp"
#include "src/core/le_window.hpp"

namespace le {

    class LeSceneRenderTarget {
    public:
        LeSceneRenderTarget(LeDevice& device, VkExtent2D extent);

        ~LeSceneRenderTarget();

        LeSceneRenderTarget(const LeSceneRenderTarget&) = delete;
        LeSceneRenderTarget& operator=(const LeSceneRenderTarget&) = delete;

        VkRenderPass getRenderPass() const { return renderPass_; }
        VkFramebuffer getFramebuffer() const { return framebuffer_; }

        VkImageView getColorImageView() const { return colorImageView_; }
        VkSampler getColorSampler() const { return colorSampler_; }

        VkExtent2D getExtent() const { return extent_; }

        void begin(VkCommandBuffer commandBuffer);
        void end(VkCommandBuffer commandBuffer);

    private:
        void createColorResources();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffer();
        void createSampler();

        LeDevice& device_;
        VkExtent2D extent_;

        VkFormat colorFormat_{ VK_FORMAT_R8G8B8A8_UNORM };
        VkFormat depthFormat_{ VK_FORMAT_D32_SFLOAT };

        VkImage colorImage_{ VK_NULL_HANDLE };
        VkDeviceMemory colorImageMemory_{ VK_NULL_HANDLE };
        VkImageView colorImageView_{ VK_NULL_HANDLE };

        VkImage depthImage_{ VK_NULL_HANDLE };
        VkDeviceMemory depthImageMemory_{ VK_NULL_HANDLE };
        VkImageView depthImageView_{ VK_NULL_HANDLE };

        VkSampler colorSampler_{ VK_NULL_HANDLE };

        VkRenderPass renderPass_{ VK_NULL_HANDLE };
        VkFramebuffer framebuffer_{ VK_NULL_HANDLE };
    };

}

#endif