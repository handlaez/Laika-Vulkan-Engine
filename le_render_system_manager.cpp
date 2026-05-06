#include "le_render_system_manager.hpp"

namespace le {
	LeRenderSystemManager::LeRenderSystemManager(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager) : device_(device), renderer_(renderer), resourceManager_(resourceManager)
	{
        createDescriptorSetLayouts();

        basicRenderSystem = std::make_unique<BasicRenderSystem>(
            device,
            renderer.getSwapchainRenderPass(),
            resourceManager,
            frameSetLayout_,
            textureSetLayout_
        );
	}

    void LeRenderSystemManager::render(LeScene& scene)
    {
        if (auto commandBuffer = renderer_.beginFrame()) {
            renderer_.beginSwapChainRenderPass(commandBuffer);

            uint32_t currentFrame = renderer_.getFrameIndex();
            auto frameDescriptorSet = basicRenderSystem->getFrameDescriptorSet(currentFrame);

            RenderFrameData frameData
            {
                commandBuffer,
                scene.getCamera(),
                currentFrame,
                frameDescriptorSet
            };

            basicRenderSystem->render(frameData, scene.getActors());

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
}