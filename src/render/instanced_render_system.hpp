#ifndef LE_INSTANCED_RENDER_SYSTEM_HPP
#define LE_INSTANCED_RENDER_SYSTEM_HPP

#include "src/render/i_render_system.hpp"
#include "src/systems/boids/instance_data.hpp"

#include "src/core/le_device.hpp"
#include "src/render/le_renderer.hpp"
#include "src/render/le_pipeline.hpp"
#include "src/objects/le_model.hpp"
#include "src/scene/le_resource_manager.hpp"
#include "src/objects/le_camera.hpp"
#include "src/objects/le_buffer.hpp"

#include <array>
#include <vector>
#include <memory>

namespace le {
    const int MAX_INSTANCES = 10000;

    class InstancedRenderSystem {
    public:
        InstancedRenderSystem(
            LeDevice& device,
            LeRenderer& renderer,
            LeResourceManager& resourceManager,
            VkDescriptorSetLayout frameLayout,
            VkDescriptorSetLayout textureLayout
        );
        ~InstancedRenderSystem();

        void setModel(uint32_t modelID);
        void setTexture(uint32_t textureID);

        void updateInstances(const std::vector<InstanceData>& instances, const RenderFrameData& frameData);

        void render(const RenderFrameData& frameData) const;

    private:
        void createPipeline(VkRenderPass renderPass);
        void createPipelineLayout();
        void createInstanceBuffer();

    private:
        LeDevice& device_;
        LeRenderer& renderer_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;
        std::unique_ptr<LePipeline> wireframePipeline_;
        VkPipelineLayout pipelineLayout_;

        VkDescriptorSetLayout frameLayout_;
        VkDescriptorSetLayout textureLayout_;

        std::array <std::unique_ptr<LeBuffer>, LeSwapchain::MAX_FRAMES_IN_FLIGHT> instanceBuffer_;

        uint32_t modelID_;
        uint32_t textureID_;

        //boid helpers
        LeModel* cachedModel_;
        VkDescriptorSet cachedTextureSet_ = VK_NULL_HANDLE;

        uint32_t instanceCount_ = 0;
    };
}
#endif