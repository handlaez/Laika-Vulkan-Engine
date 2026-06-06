#include "le_render_system_manager.hpp"
#include "le_utils.hpp"
#include "profiler.hpp"

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

        skyboxRenderSystem = std::make_unique<SkyboxRenderSystem>(
            device,
            renderer,
            resourceManager,
            frameSetLayout_
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

        lightingUniformBuffers_.resize(frames);
        lightingUniformBuffersMemory_.resize(frames);
        lightingUniformBuffersMapped_.resize(frames);

        VkDeviceSize lightingBufferSize = sizeof(LightingUBO);

        // create buffers (frame)
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

        // (lighting)
        for (size_t i = 0; i < frames; i++)
        {
            device_.createBuffer(
                lightingBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                lightingUniformBuffers_[i],
                lightingUniformBuffersMemory_[i]
            );

            vkMapMemory(
                device_.device(),
                lightingUniformBuffersMemory_[i],
                0,
                lightingBufferSize,
                0,
                &lightingUniformBuffersMapped_[i]
            );
        }

        // create descriptor pool
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(frames * 2);

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
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = frameUniformBuffers_[i];
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorBufferInfo lightingBufferInfo{};
            lightingBufferInfo.buffer = lightingUniformBuffers_[i];
            lightingBufferInfo.offset = 0;
            lightingBufferInfo.range = sizeof(LightingUBO);

            std::array<VkWriteDescriptorSet, 2> writes{};

            writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet = frameDescriptorSets_[i];
            writes[0].dstBinding = 0;
            writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[0].descriptorCount = 1;
            writes[0].pBufferInfo = &cameraBufferInfo;

            writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[1].dstSet = frameDescriptorSets_[i];
            writes[1].dstBinding = 1;
            writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[1].descriptorCount = 1;
            writes[1].pBufferInfo = &lightingBufferInfo;

            vkUpdateDescriptorSets(
                device_.device(),
                static_cast<uint32_t>(writes.size()),
                writes.data(),
                0,
                nullptr
            );
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

        basicRenderSystem->setModel(1);
        basicRenderSystem->setTexture(1);
    }

    void LeRenderSystemManager::render(LeScene& scene)
    {
        if (auto commandBuffer = renderer_.beginFrame()) {

            renderer_.beginSwapChainRenderPass(commandBuffer);

            uint32_t currentFrame = renderer_.getFrameIndex();

            updateFrameUBO(currentFrame, scene.getCamera());
            updateLightingUBO(currentFrame);

            VkDescriptorSet frameSet = getFrameDescriptorSet(currentFrame);

            RenderFrameData frameData{
                commandBuffer,
                scene.getCamera(),
                currentFrame,
                frameSet
            };

            // actors
            auto& actors = scene.getActors();
            if (!actors.empty())
            {
                basicRenderSystem->render(frameData, actors);
            }

            /*
            if (scene.instanceDataPtr != nullptr && !scene.instanceDataPtr->empty())
            {
                if (Utils::instancingEnabled)
                {
                    instancedRenderSystem->updateInstances(*scene.instanceDataPtr, frameData);
                    instancedRenderSystem->render(frameData);
                }
                else
                {
                    if (Utils::parallelEnabled.load())
                    {
                        basicRenderSystem->renderParallel(frameData, *scene.instanceDataPtr, renderer_.getSwapchainRenderPass());
                    }
                    else
                    {
                        basicRenderSystem->render(frameData, *scene.instanceDataPtr);
                    }
                }
            }
            */

            //skybox
            if (Utils::skyboxEnabled)
            {
                skyboxRenderSystem->render(frameData);
            }

            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame();
        }
    }

    void LeRenderSystemManager::updateLightingUBO(uint32_t frameIndex)
    {
        LightingUBO lighting{};

        lighting.lightColor = glm::vec4(1.0f);
        lighting.lightDir = glm::vec4(1.f, 3.f, 2.f, 0.f);

        lighting.lightingEnabled = Utils::lightingEnabled;
        lighting.texturesEnabled = Utils::texturesEnabled;

        memcpy(
            lightingUniformBuffersMapped_[frameIndex],
            &lighting,
            sizeof(LightingUBO)
        );
    }

    void LeRenderSystemManager::createDescriptorSetLayouts()
    {
        VkDescriptorSetLayoutBinding cameraBinding{};
        cameraBinding.binding = 0;
        cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraBinding.descriptorCount = 1;
        cameraBinding.stageFlags =
            VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding lightingBinding{};
        lightingBinding.binding = 1;
        lightingBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightingBinding.descriptorCount = 1;
        lightingBinding.stageFlags =
            VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
            cameraBinding,
            lightingBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

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
