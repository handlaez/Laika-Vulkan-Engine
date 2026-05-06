#include "instanced_render_system.hpp"

#include "le_frame_info.hpp"

namespace le
{
    InstancedRenderSystem::InstancedRenderSystem(
        LeDevice& device,
        LeRenderer& renderer,
        LeResourceManager& resourceManager,
        VkDescriptorSetLayout frameLayout,
        VkDescriptorSetLayout textureLayout
    )
        : device_(device),
        renderer_(renderer),
        resourceManager_(resourceManager),
        frameLayout_(frameLayout),
        textureLayout_(textureLayout)
    {
        createPipelineLayout();
        createPipeline(renderer_.getSwapchainRenderPass());
        createInstanceBuffer();
    }

    void InstancedRenderSystem::createPipelineLayout()
    {
        std::array<VkDescriptorSetLayout, 2> setLayouts = {
            frameLayout_,     // set = 0 (camera)
            textureLayout_    // set = 1 (texture)
        };

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        layoutInfo.pSetLayouts = setLayouts.data();

        layoutInfo.pushConstantRangeCount = 0; // IMPORTANT: instancing uses no push constants

        if (vkCreatePipelineLayout(
            device_.device(),
            &layoutInfo,
            nullptr,
            &pipelineLayout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instanced pipeline layout");
        }
    }

    void InstancedRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        auto bindings = LeModel::Vertex::getBindingDescriptions();

        VkVertexInputBindingDescription instanceBinding{};
        instanceBinding.binding = 1;
        instanceBinding.stride = sizeof(InstanceData);
        instanceBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        bindings.push_back(instanceBinding);

        auto attributes = LeModel::Vertex::getAttributeDescriptions();

        uint32_t loc = static_cast<uint32_t>(attributes.size());

        attributes.push_back({ loc + 0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 0 });
        attributes.push_back({ loc + 1, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 1 });
        attributes.push_back({ loc + 2, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 2 });
        attributes.push_back({ loc + 3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceData, model) + sizeof(glm::vec4) * 3 });

        PipelineConfigInfo pipelineConfig{};
        LePipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout_;
        pipelineConfig.bindingDescriptions = bindings;
        pipelineConfig.attributeDescriptions = attributes;

        // IMPORTANT: we will extend vertex attributes later for instance data
        pipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/instanced_vert_shader.spv",
            "shaders/instanced_frag_shader.spv",
            pipelineConfig
        );
    }

    void InstancedRenderSystem::createInstanceBuffer()
    {
        instanceBuffer_ = std::make_unique<LeBuffer>(
            device_,
            sizeof(InstanceData),
            MAX_INSTANCES,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        instanceBuffer_->map();
    }

    void InstancedRenderSystem::setModel(uint32_t modelID)
    {
        modelID_ = modelID;
    }

    void InstancedRenderSystem::setTexture(uint32_t textureID)
    {
        textureID_ = textureID;
    }

    void InstancedRenderSystem::setInstances(const std::vector<InstanceData>& instances)
    {
        instances_ = instances;
    }

    void InstancedRenderSystem::updateInstances(const std::vector<InstanceData>& instances)
    {
        instanceCount_ = static_cast<uint32_t>(instances.size());

        instanceBuffer_->writeToBuffer(
            (void*)instances.data(),
            instances.size() * sizeof(InstanceData)
        );
    }

    void InstancedRenderSystem::render(const RenderFrameData& frameData)
    {
        pipeline_->bind(frameData.cmd);

        // set 0: camera
        vkCmdBindDescriptorSets(
            frameData.cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_,
            0,
            1,
            &frameData.frameDescriptorSet,
            0,
            nullptr
        );

        instanceBuffer_->update(
            instances_.data(),
            sizeof(InstanceData) * instances_.size()
        );

        VkDescriptorSet textureSet =
            resourceManager_.getTextureDescriptorSet(textureID_);

        vkCmdBindDescriptorSets(
            frameData.cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_,
            1,
            1,
            &textureSet,
            0,
            nullptr
        );

        auto model = resourceManager_.getModel(modelID_);

        VkBuffer buffers[] = {
            model->getVertexBuffer(),
            instanceBuffer_->getBuffer()
        };

        VkDeviceSize offsets[] = { 0, 0 };

        vkCmdBindVertexBuffers(
            frameData.cmd,
            0,
            2,
            buffers,
            offsets
        );

        model->bindIndexBuffer(frameData.cmd);

        vkCmdDrawIndexed(
            frameData.cmd,
            model->getIndexCount(),
            static_cast<uint32_t>(instances_.size()),
            0,
            0,
            0
        );
    }
}