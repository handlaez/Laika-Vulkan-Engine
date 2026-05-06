#ifndef LE_INSTANCED_RENDER_SYSTEM_HPP
#define LE_INSTANCED_RENDER_SYSTEM_HPP

#include "i_render_system.hpp"
#include "instance_data.hpp"

#include "le_device.hpp"
#include "le_renderer.hpp"
#include "le_pipeline.hpp"
#include "le_model.hpp"
#include "le_resource_manager.hpp"
#include "le_camera.hpp"
#include "le_buffer.hpp"

#include <memory>
#include <array>
#include <vector>

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

        void setModel(uint32_t modelID);
        void setTexture(uint32_t textureID);
        void setInstances(const std::vector<InstanceData>& instances);

        void updateInstances(const std::vector<InstanceData>& instances);

        void render(const RenderFrameData& frameData);

    private:
        void createPipeline(VkRenderPass renderPass);
        void createPipelineLayout();
        void createInstanceBuffer();

    private:
        LeDevice& device_;
        LeRenderer& renderer_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;
        VkPipelineLayout pipelineLayout_;

        VkDescriptorSetLayout frameLayout_;
        VkDescriptorSetLayout textureLayout_;

        std::unique_ptr<LeBuffer> instanceBuffer_;
        std::vector<InstanceData> instances_;

        uint32_t modelID_;
        uint32_t textureID_;

        uint32_t instanceCount_ = 0;
    };
}
#endif