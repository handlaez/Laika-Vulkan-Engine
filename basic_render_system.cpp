#include "basic_render_system.hpp"
#include "le_utils.hpp"

#include <omp.h>
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

        createThreadLocalCommandBuffers();
    }

    BasicRenderSystem::~BasicRenderSystem() {
        vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);

        for (auto pool : threadCommandPools) {
            vkDestroyCommandPool(device_.device(), pool, nullptr);
        }
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

        PipelineConfigInfo wireframeConfig{};
        LePipeline::defaultPipelineConfigInfo(wireframeConfig);
        wireframeConfig.renderPass = renderPass;
        wireframeConfig.pipelineLayout = pipelineLayout_;
        wireframeConfig.bindingDescriptions = LeModel::Vertex::getBindingDescriptions();
        wireframeConfig.attributeDescriptions = LeModel::Vertex::getAttributeDescriptions();

        wireframeConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

        wireframePipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/frag_shader.spv",
            wireframeConfig
        );
    }

    void BasicRenderSystem::createThreadLocalCommandBuffers()
    {
        int maxThreads = omp_get_max_threads();
        threadCommandPools.resize(maxThreads);
        threadCommandBuffers.resize(maxThreads);

        for (int i = 0; i < maxThreads; i++) {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            // flag is absolutely critical: it allows us to reset the secondary command buffer every frame
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            poolInfo.queueFamilyIndex = device_.findPhysicalQueueFamilies().graphicsFamily;

            if (vkCreateCommandPool(device_.device(), &poolInfo, nullptr, &threadCommandPools[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create thread-local command pool!");
            }

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = threadCommandPools[i];
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY; // must be secondary to execute inside the main pass
            allocInfo.commandBufferCount = 1;

            if (vkAllocateCommandBuffers(device_.device(), &allocInfo, &threadCommandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate thread-local command buffer!");
            }
        }
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
            push.color = glm::vec4(actor.color, 1.f);
            push.position = glm::vec4(actor.transform.translation, 1.f);
            push.forward = glm::vec4(actor.transform.rotation.x, actor.transform.rotation.y, actor.transform.rotation.z, 1.f);

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

    void BasicRenderSystem::render(const RenderFrameData& frameData, const std::vector<InstanceData>& instances)
    {
        if (Utils::wireframeEnabled) {
            wireframePipeline_->bind(frameData.cmd);
        }
        else {
            pipeline_->bind(frameData.cmd);
        }

        vkCmdBindDescriptorSets(
            frameData.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_, 0, 1, &frameData.frameDescriptorSet, 0, nullptr
        );

        vkCmdBindDescriptorSets(
            frameData.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_, 1, 1, &cachedTextureSet_, 0, nullptr
        );

        cachedModel_->bind(frameData.cmd);

        for (const auto& instance : instances)
        {
            SimplePushConstantData push{};
            push.position = instance.position;
            push.forward = instance.forward;
            push.color = glm::vec4(1.0f);

            vkCmdPushConstants(
                frameData.cmd, pipelineLayout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0, sizeof(SimplePushConstantData), &push
            );

            cachedModel_->draw(frameData.cmd);
        }
    }

    void BasicRenderSystem::renderParallel(const RenderFrameData& frameData, const std::vector<InstanceData>& instances, VkRenderPass renderPass)
    {
        std::vector<VkCommandBuffer> activeSecondaryBuffers;

        #pragma omp parallel if(Utils::parallelEnabled.load())
        {
            int threadId = omp_get_thread_num();
            VkCommandBuffer secCmd = threadCommandBuffers[threadId];

            vkResetCommandBuffer(secCmd, 0);

            VkCommandBufferInheritanceInfo inheritanceInfo{};
            inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            inheritanceInfo.renderPass = renderPass;
            inheritanceInfo.subpass = 0;

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            beginInfo.pInheritanceInfo = &inheritanceInfo;

            vkBeginCommandBuffer(secCmd, &beginInfo);

            
            if (Utils::wireframeEnabled) {
                wireframePipeline_->bind(secCmd);
            }
            else {
                pipeline_->bind(secCmd);
            }

            vkCmdBindDescriptorSets(secCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &frameData.frameDescriptorSet, 0, nullptr);
            vkCmdBindDescriptorSets(secCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 1, 1, &cachedTextureSet_, 0, nullptr);
            cachedModel_->bind(secCmd);


            #pragma omp for nowait
            for (int i = 0; i < instances.size(); i++)
            {
                SimplePushConstantData push{};
                push.position = instances[i].position;
                push.forward = instances[i].forward;
                push.color = glm::vec4(1.0f);

                vkCmdPushConstants(
                    secCmd, pipelineLayout_,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData), &push
                );

                cachedModel_->draw(secCmd);
            }

            vkEndCommandBuffer(secCmd);

            // safely add this thread's buffer to the execution list
            #pragma omp critical
            {
                activeSecondaryBuffers.push_back(secCmd);
            }
        }

        // MAIN THREAD: execute all recorded secondary buffers into the primary buffer
        if (!activeSecondaryBuffers.empty()) {
            vkCmdExecuteCommands(frameData.cmd, static_cast<uint32_t>(activeSecondaryBuffers.size()), activeSecondaryBuffers.data());
        }
    }


    void BasicRenderSystem::setModel(uint32_t modelID) {
        auto modelPtr = resourceManager_.getModel(modelID);
        if (modelPtr) cachedModel_ = modelPtr.get();
    }

    void BasicRenderSystem::setTexture(uint32_t textureID) {
        if (resourceManager_.getTexture(textureID)) {
            cachedTextureSet_ = resourceManager_.getTextureDescriptorSet(textureID);
        }
    }

} // namespace le
