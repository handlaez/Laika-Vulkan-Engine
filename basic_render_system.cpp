#include "basic_render_system.hpp"
#include <array>
#include <stdexcept>

namespace le {

    BasicRenderSystem::BasicRenderSystem(LeDevice& device,
        VkRenderPass renderPass,
        LeResourceManager& resourceManager,
        VkDescriptorSetLayout frameSetLayout,
        VkDescriptorSetLayout textureSetLayout
    )
        : device_(device),
        resourceManager_(resourceManager),
        frameSetLayout_(frameSetLayout),
        textureSetLayout_(textureSetLayout)
    {
        createPipelineLayout();        // uses 2 set layouts
        createPipeline(renderPass);
    }

    BasicRenderSystem::~BasicRenderSystem() {
        vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
    }

    void BasicRenderSystem::createPipelineLayout() {
        std::array<VkDescriptorSetLayout, 2> setLayouts = {
        frameSetLayout_,                                    // set = 0
        textureSetLayout_    // set = 1
        };

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void BasicRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout_ != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout_;
        pipelineConfig.bindingDescriptions = LeModel::Vertex::getBindingDescriptions();
        pipelineConfig.attributeDescriptions = LeModel::Vertex::getAttributeDescriptions();

        pipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/frag_shader.spv",
            pipelineConfig
        );
    }

    void BasicRenderSystem::render(const RenderFrameData& frameData, const std::vector<LeActor>& actors)
    {
        pipeline_->bind(frameData.cmd);

        // set 0: global frame
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

        for (const auto& actor : actors)
        {
            auto model = resourceManager_.getModel(actor.modelID);
            VkDescriptorSet textureSet =
                resourceManager_.getTextureDescriptorSet(actor.textureID);

            // set 1: tex
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

            model->bind(frameData.cmd);

            SimplePushConstantData push{};
            push.color = actor.color;
            push.transform = actor.transform.mat4();

            vkCmdPushConstants(
                frameData.cmd,
                pipelineLayout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            model->draw(frameData.cmd);
        }
    }

} // namespace le
