#ifndef BASIC_RENDER_SYSTEM_HPP
#define BASIC_RENDER_SYSTEM_HPP

#include "src/core/le_device.hpp"
#include "src/render/le_pipeline.hpp"
#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/objects/le_texture.hpp"
#include "src/render/le_swapchain.hpp"
#include "src/scene/le_resource_manager.hpp"
#include "src/render/i_render_system.hpp"

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
        glm::mat4 model;
        glm::vec4 color;
    };

    class BasicRenderSystem : public IRenderSystem {
    public:
        BasicRenderSystem(
            LeDevice& device,
            VkRenderPass renderPass,
            LeResourceManager& resourceManager,
            VkDescriptorSetLayout frameSetLayout,
            VkDescriptorSetLayout textureSetLayout
        );

        ~BasicRenderSystem();

        // Non-copyable
        BasicRenderSystem(const BasicRenderSystem&) = delete;
        BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;
        BasicRenderSystem(BasicRenderSystem&&) = delete;
        BasicRenderSystem& operator=(BasicRenderSystem&&) = delete;

        void render(const RenderFrameData& frameData, const std::vector<LeActor>& actors) override;

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        void renderActors(const RenderFrameData& frameData, const std::vector<LeActor>& actors);
        void renderHitboxes(const RenderFrameData& frameData, const std::vector<LeActor>& actors);

        LeDevice& device_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;
        std::unique_ptr<LePipeline> wireframePipeline_;

        VkPipelineLayout pipelineLayout_{ VK_NULL_HANDLE };

        VkDescriptorSetLayout frameSetLayout_;
        VkDescriptorSetLayout textureSetLayout_;
    };

}

#endif