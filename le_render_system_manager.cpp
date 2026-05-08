#include "le_render_system_manager.hpp"

namespace le {
	LeRenderSystemManager::LeRenderSystemManager(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager) : device_(device), renderer_(renderer), resourceManager_(resourceManager)
	{
        createDescriptorSetLayouts();

        createFrameResources();

        basicRenderSystem = std::make_unique<BasicRenderSystem>(
            device,
            renderer.getSwapchainRenderPass(),
            resourceManager,
            frameSetLayout_,
            textureSetLayout_
        );

        instancedRenderSystem = std::make_unique<InstancedRenderSystem>(
            device,
            renderer,
            resourceManager,
            frameSetLayout_,
            textureSetLayout_
        );

        instancedRenderSystem->setModel(0);   //just so it's not empty later on
        instancedRenderSystem->setTexture(0); //exactly the same here
	}

    LeRenderSystemManager::~LeRenderSystemManager()
    {
        for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (frameUniformBuffersMapped_[i])
            {
                vkUnmapMemory(device_.device(), frameUniformBuffersMemory_[i]);
            }

            vkDestroyBuffer(device_.device(), frameUniformBuffers_[i], nullptr);
            vkFreeMemory(device_.device(), frameUniformBuffersMemory_[i], nullptr);
        }

        vkDestroyDescriptorPool(device_.device(), frameDescriptorPool_, nullptr);
    }

    // monstrously huge func
    void LeRenderSystemManager::createFrameResources() 
    {
        size_t frames = LeSwapchain::MAX_FRAMES_IN_FLIGHT;

        frameUniformBuffers_.resize(frames);
        frameUniformBuffersMemory_.resize(frames);
        frameUniformBuffersMapped_.resize(frames);
        frameDescriptorSets_.resize(frames);

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        // create buffers
        for (size_t i = 0; i < frames; i++)
        {
            device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                frameUniformBuffers_[i],
                frameUniformBuffersMemory_[i]
            );

            vkMapMemory(
                device_.device(),
                frameUniformBuffersMemory_[i],
                0,
                bufferSize,
                0,
                &frameUniformBuffersMapped_[i]
            );
        }

        // create descriptor pool
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(frames);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(frames);

        if (vkCreateDescriptorPool(device_.device(), &poolInfo, nullptr, &frameDescriptorPool_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create frame descriptor pool!");
        }

        // allocate descriptor sets
        std::vector<VkDescriptorSetLayout> layouts(frames, frameSetLayout_);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = frameDescriptorPool_;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(frames);
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(
            device_.device(),
            &allocInfo,
            frameDescriptorSets_.data()
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate frame descriptor sets!");
        }

        // bind buffers to descriptor sets
        for (size_t i = 0; i < frames; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = frameUniformBuffers_[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = frameDescriptorSets_[i];
            write.dstBinding = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device_.device(), 1, &write, 0, nullptr);
        }
    }

    void LeRenderSystemManager::updateFrameUBO(uint32_t frameIndex, const LeCamera& camera)
    {
        UniformBufferObject ubo{};
        ubo.view = camera.getView();
        ubo.proj = camera.getProjection();

        memcpy(
            frameUniformBuffersMapped_[frameIndex],
            &ubo,
            sizeof(UniformBufferObject)
        );
    }

    void LeRenderSystemManager::sync(LeScene& scene)
    {
        instancedRenderSystem->setModel(1);
        instancedRenderSystem->setTexture(1);
    }

    void LeRenderSystemManager::render(LeScene& scene)
    {
        if (auto commandBuffer = renderer_.beginFrame()) {

            renderer_.beginSwapChainRenderPass(commandBuffer);

            uint32_t currentFrame = renderer_.getFrameIndex();

            // camera update now lives here
            updateFrameUBO(currentFrame, scene.getCamera());

            VkDescriptorSet frameSet = getFrameDescriptorSet(currentFrame);

            RenderFrameData frameData{
                commandBuffer,
                scene.getCamera(),
                currentFrame,
                frameSet
            };

            if (scene.instanceDataPtr != nullptr) {
                instancedRenderSystem->updateInstances(*scene.instanceDataPtr, frameData);
            }

            //basicRenderSystem->render(frameData, scene.getActors());
            instancedRenderSystem->render(frameData);

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }

    void LeRenderSystemManager::createDescriptorSetLayouts()
    {
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

        textureSetLayout_ = resourceManager_.getTextureDescriptorSetLayout();
    }

    VkDescriptorSet LeRenderSystemManager::getFrameDescriptorSet(uint32_t frameIndex) const
    {
        return frameDescriptorSets_.at(frameIndex);
    }
}