#ifndef BASIC_RENDER_SYSTEM_HPP
#define BASIC_RENDER_SYSTEM_HPP

#include "i_render_system.hpp"

#include "le_device.hpp"
#include "le_pipeline.hpp"
#include "le_actor.hpp"
#include "le_camera.hpp"
#include "le_texture.hpp"
#include "le_swapchain.hpp"
#include "le_resource_manager.hpp"

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace le {

    struct UniformBufferObject {
        glm::mat4 view{ 1.f };
        glm::mat4 proj{ 1.f };
    };

    struct SimplePushConstantData {
        glm::mat4 transform{ 1.f };
        alignas(16) glm::vec3 color;
    };

    class BasicRenderSystem : public IRenderSystem {
    public:
        BasicRenderSystem(LeDevice& device,
            VkRenderPass renderPass,
            LeResourceManager& resourceManager,
            VkDescriptorSetLayout frameSetLayout,
            VkDescriptorSetLayout textureSetLayout
        );
        ~BasicRenderSystem();

        // Non-copyable
        BasicRenderSystem(const BasicRenderSystem&) = delete;
        BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;

        void render(const RenderFrameData& frameData, const std::vector<LeActor>& actors);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        LeDevice& device_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;
        VkPipelineLayout pipelineLayout_{ VK_NULL_HANDLE };

        VkDescriptorSetLayout frameSetLayout_;
        VkDescriptorSetLayout textureSetLayout_;
    };

}

#endif