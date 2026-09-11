#include "basic_render_system.hpp"
#include "src/core/le_utils.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

namespace le {

    BasicRenderSystem::BasicRenderSystem(
        LeDevice& device,
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
        createPipelineLayout();
        createPipeline(renderPass);
    }

    BasicRenderSystem::~BasicRenderSystem()
    {
        vkDestroyPipelineLayout(
            device_.device(),
            pipelineLayout_,
            nullptr
        );
    }

    void BasicRenderSystem::createPipelineLayout()
    {
        std::array<VkDescriptorSetLayout, 2> setLayouts = {
            frameSetLayout_,    // set = 0
            textureSetLayout_   // set = 1
        };

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount =
            static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(
            device_.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void BasicRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout_ != VK_NULL_HANDLE && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LePipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout_;
        pipelineConfig.bindingDescriptions =
            Vertex::getBindingDescriptions();
        pipelineConfig.attributeDescriptions =
            Vertex::getAttributeDescriptions();

        pipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/frag_shader.spv",
            pipelineConfig
        );

        PipelineConfigInfo wireframeConfig{};
        LePipeline::defaultPipelineConfigInfo(wireframeConfig);

        wireframeConfig.renderPass = renderPass;
        wireframeConfig.pipelineLayout = pipelineLayout_;
        wireframeConfig.bindingDescriptions = Vertex::getBindingDescriptions();
        wireframeConfig.attributeDescriptions = Vertex::getAttributeDescriptions();
        wireframeConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
        wireframeConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        wireframeConfig.rasterizationInfo.lineWidth = 1.0f;

        wireframePipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/wireframe_shader.spv",
            wireframeConfig
        );
    }

    void BasicRenderSystem::render(const RenderFrameData& frameData, const std::vector<LeActor>& actors)
    {
        if (Utils::wireframeEnabled) {
            wireframePipeline_->bind(frameData.cmd);
        }
        else {
            pipeline_->bind(frameData.cmd);
        }

        // set 0: global frame descriptor set
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

        renderActors(frameData, actors);

        if (Utils::hitboxesEnabled) {
            renderHitboxes(frameData, actors);
        }
    }

    void BasicRenderSystem::renderActors(
        const RenderFrameData& frameData,
        const std::vector<LeActor>& actors)
    {
        for (const auto& actor : actors) {
            auto model = resourceManager_.getModel(actor.modelID);

            VkDescriptorSet textureSet =
                resourceManager_.getTextureDescriptorSet(
                    actor.textureID
                );

            // set 1: actor texture
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
            push.model = actor.transform.mat4();
            push.color = glm::vec4(actor.color, 1.f);

            vkCmdPushConstants(
                frameData.cmd,
                pipelineLayout_,
                VK_SHADER_STAGE_VERTEX_BIT |
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            model->draw(frameData.cmd);
        }
    }

    void BasicRenderSystem::renderHitboxes(
        const RenderFrameData& frameData,
        const std::vector<LeActor>& actors)
    {
        wireframePipeline_->bind(frameData.cmd);

        // Model 0 is the unit cube used to visualize hitboxes.
        auto model = resourceManager_.getModel(0);

        model->bind(frameData.cmd);

        for (const auto& actor : actors) {
            for (const auto& hitbox : actor.hitboxes) {
                SimplePushConstantData push{};
                push.color = glm::vec4(1.f);

                push.model =
                    hitbox.mat4(actor.transform.translation);

                vkCmdPushConstants(
                    frameData.cmd,
                    pipelineLayout_,
                    VK_SHADER_STAGE_VERTEX_BIT |
                    VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push
                );

                model->draw(frameData.cmd);
            }
        }
    }

} // namespace le