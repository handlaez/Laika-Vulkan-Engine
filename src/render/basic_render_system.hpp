#ifndef BASIC_RENDER_SYSTEM_HPP
#define BASIC_RENDER_SYSTEM_HPP

#include "src/core/le_device.hpp"
#include "src/render/le_pipeline.hpp"
#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/objects/le_texture.hpp"
#include "src/render/le_swapchain.hpp"
#include "src/scene/le_resource_manager.hpp"

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

    class BasicRenderSystem {
    public:
        BasicRenderSystem(LeDevice& device, VkRenderPass renderPass, LeResourceManager& resourceManager);
        ~BasicRenderSystem();

        // Non-copyable
        BasicRenderSystem(const BasicRenderSystem&) = delete;
        BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;

        void renderActors(
            VkCommandBuffer commandBuffer,
            std::vector<std::shared_ptr<LeActor>>& actors,
            bool renderHitboxes,
            const LeCamera& camera,
            size_t currentFrame
        );

    private:
        void createDescriptorSetLayout();
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();

        LeDevice& device_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;           // the intended way of hot-swapping used shaders
        std::unique_ptr<LePipeline> wireframePipeline_;  // this one is for rendering hitboxes (primarly)
        VkPipelineLayout pipelineLayout_{ VK_NULL_HANDLE };

        std::vector<VkBuffer> uniformBuffers_;
        std::vector<VkDeviceMemory> uniformBuffersMemory_;
        std::vector<void*> uniformBuffersMapped_;

        VkDescriptorPool descriptorPool_{ VK_NULL_HANDLE };
        VkDescriptorSetLayout frameSetLayout_{ VK_NULL_HANDLE };    // UBO
        VkDescriptorSetLayout materialSetLayout_{ VK_NULL_HANDLE }; // Textures from le_resource_manager
        std::vector<VkDescriptorSet> descriptorSets_;
    };

}

#endif