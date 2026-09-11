#ifndef LE_SKYBOX_RENDER_SYSTEM_HPP
#define LE_SKYBOX_RENDER_SYSTEM_HPP

#include "src/render/i_render_system.hpp"

#include "src/core/le_device.hpp"
#include "src/render/le_renderer.hpp"
#include "src/render/le_pipeline.hpp"
#include "src/objects/le_camera.hpp"
#include "src/objects/le_buffer.hpp"
#include "src/scene/skybox/le_cubemap.hpp"

#include <memory>
#include <array>
#include <vector>

namespace le {
    struct SkyboxVertex
    {
        glm::vec3 position;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
        {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(SkyboxVertex);
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescriptions;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(SkyboxVertex, position);

            return attributeDescriptions;
        }
    };

    class SkyboxRenderSystem
    {
    public:
        SkyboxRenderSystem(
            LeDevice& device,
            LeRenderer& renderer,
            LeResourceManager& resourceManager,
            VkDescriptorSetLayout frameLayout
        );

        ~SkyboxRenderSystem();

        void render(const RenderFrameData& frameData);

    private:
        void createPipeline(VkRenderPass renderPass);
        void createPipelineLayout();
        void createCubeBuffers();
        void createCubemapDescriptorResources();

        LeDevice& device_;
        LeRenderer& renderer_;
        LeResourceManager& resourceManager_;

        std::unique_ptr<LePipeline> pipeline_;
        VkPipelineLayout pipelineLayout_;
        VkDescriptorSetLayout frameLayout_;

        std::unique_ptr<LeBuffer> vertexBuffer_;
        uint32_t vertexCount_ = 0;

        std::unique_ptr<LeCubemap> cubemap_;
        VkDescriptorSet cubemapDescriptorSet_;
        VkDescriptorPool cubemapDescriptorPool_;
        VkDescriptorSetLayout cubemapSetLayout_;
    };
}
#endif