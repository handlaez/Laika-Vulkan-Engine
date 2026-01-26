#include "basic_render_system.hpp"
#include <array>
#include <stdexcept>

namespace le {

    BasicRenderSystem::BasicRenderSystem(LeDevice& device, VkRenderPass renderPass, LeResourceManager& resourceManager)
        : device_(device), resourceManager_(resourceManager) {

        createDescriptorSetLayout();   // UBO only
        createPipelineLayout();        // uses 2 set layouts
        createPipeline(renderPass);

        createUniformBuffers();
        createDescriptorPool();        // UBO pool only!
        createDescriptorSets();        // only UBO sets
    }

    BasicRenderSystem::~BasicRenderSystem() {
        for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++) {
            if (uniformBuffersMapped_[i]) {
                vkUnmapMemory(device_.device(), uniformBuffersMemory_[i]);
                uniformBuffersMapped_[i] = nullptr;
            }
            vkDestroyBuffer(device_.device(), uniformBuffers_[i], nullptr);
            vkFreeMemory(device_.device(), uniformBuffersMemory_[i], nullptr);
        }

        vkDestroyDescriptorPool(device_.device(), descriptorPool_, nullptr);
        vkDestroyDescriptorSetLayout(device_.device(), frameSetLayout_, nullptr);
        vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
    }

    void BasicRenderSystem::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;              // only the UBO
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(
            device_.device(),
            &layoutInfo,
            nullptr,
            &frameSetLayout_
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create frame descriptor set layout!");
        }
    }

    void BasicRenderSystem::createPipelineLayout() {
        std::array<VkDescriptorSetLayout, 2> setLayouts = {
        frameSetLayout_,                                    // set = 0
        resourceManager_.getTextureDescriptorSetLayout()    // set = 1
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

        pipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/frag_shader.spv",
            pipelineConfig
        );

        PipelineConfigInfo wireConfig{};
        LePipeline::defaultPipelineConfigInfo(wireConfig);

        wireConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
        wireConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        wireConfig.rasterizationInfo.lineWidth = 1.0f;

        wireConfig.renderPass = renderPass;
        wireConfig.pipelineLayout = pipelineLayout_;

        // optional, recommended for debug hitboxes
        // wireConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;

        wireframePipeline_ = std::make_unique<LePipeline>(
            device_,
            "shaders/vert_shader.spv",
            "shaders/wireframe_shader.spv",
            wireConfig
        );
    }

    void BasicRenderSystem::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        size_t frames = LeSwapchain::MAX_FRAMES_IN_FLIGHT;

        uniformBuffers_.resize(frames);
        uniformBuffersMemory_.resize(frames);
        uniformBuffersMapped_.resize(frames);

        for (size_t i = 0; i < frames; i++) {
            device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers_[i],
                uniformBuffersMemory_[i]
            );

            vkMapMemory(device_.device(), uniformBuffersMemory_[i], 0, bufferSize, 0, &uniformBuffersMapped_[i]);
        }
    }

    void BasicRenderSystem::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(device_.device(), &poolInfo, nullptr, &descriptorPool_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void BasicRenderSystem::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(LeSwapchain::MAX_FRAMES_IN_FLIGHT, frameSetLayout_);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool_;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets_.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(device_.device(), &allocInfo, descriptorSets_.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers_[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSets_[i];
            write.dstBinding = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device_.device(), 1, &write, 0, nullptr);
        }
    }

    void BasicRenderSystem::renderActors(
        VkCommandBuffer commandBuffer,
        std::vector<std::shared_ptr<LeActor>>& actors,
        bool renderHitboxes,
        const LeCamera& camera,
        size_t currentFrame
    ) {
        pipeline_->bind(commandBuffer);

        UniformBufferObject ubo{};
        ubo.view = camera.getView();
        ubo.proj = camera.getProjection();
        memcpy(uniformBuffersMapped_[currentFrame], &ubo, sizeof(ubo));

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_,
            0, 1,
            &descriptorSets_[currentFrame],
            0, nullptr
        );

        for (auto& actor : actors) {
            // actor's texture descriptor set
            VkDescriptorSet textureSet = resourceManager_.getTextureDescriptorSet(actor->textureID);
            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout_,
                1, 1,
                &textureSet,
                0, nullptr
            );

            auto model = resourceManager_.getModel(actor->modelID);
            model->bind(commandBuffer);

            SimplePushConstantData push{};
            push.color = actor->color;
            push.transform = actor->transform.mat4();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout_,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            model->draw(commandBuffer);
        }

        if (renderHitboxes)
        {
            wireframePipeline_->bind(commandBuffer);
            auto model = resourceManager_.getModel(0); // uniform cube ID
            model->bind(commandBuffer);

            for (auto& actor : actors) 
            {
                for (const auto& hitbox : actor->hitboxes)
                {
                    SimplePushConstantData push{};
                    push.color = glm::vec3(1.0f, 1.0f, 1.0f);  // very visible hitbox color 
                    push.transform = hitbox.mat4();

                    vkCmdPushConstants(
                        commandBuffer,
                        pipelineLayout_,
                        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                        0,
                        sizeof(SimplePushConstantData),
                        &push
                    );

                    model->draw(commandBuffer);
                }
            }
        }
    }

} // namespace le
