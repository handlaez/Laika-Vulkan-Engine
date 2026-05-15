#include "skybox_render_system.hpp"

namespace le
{
	SkyboxRenderSystem::SkyboxRenderSystem(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager, VkDescriptorSetLayout frameLayout)
		: device_(device), renderer_(renderer), resourceManager_(resourceManager), frameLayout_(frameLayout)
	{
        createCubemapDescriptorResources();
		createPipelineLayout();
		createPipeline(renderer_.getSwapchainRenderPass());
		createCubeBuffers();
	}

	SkyboxRenderSystem::~SkyboxRenderSystem()
	{
        vkDestroyDescriptorPool(
            device_.device(),
            cubemapDescriptorPool_,
            nullptr
        );

        vkDestroyDescriptorSetLayout(
            device_.device(),
            cubemapSetLayout_,
            nullptr
        );

		vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
	}

	void SkyboxRenderSystem::render(const RenderFrameData& frameData)
	{
        pipeline_->bind(frameData.cmd);

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

        vkCmdBindDescriptorSets(
            frameData.cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout_,
            1,
            1,
            &cubemapDescriptorSet_,
            0,
            nullptr
        );

        VkBuffer vertexBuffers[] =
        {
            vertexBuffer_->getBuffer()
        };

        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(
            frameData.cmd,
            0,
            1,
            vertexBuffers,
            offsets
        );

        vkCmdDraw(
            frameData.cmd,
            vertexCount_,
            1,
            0,
            0
        );
	}

	void SkyboxRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfig{};

		LePipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout_;
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
		pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

		pipelineConfig.bindingDescriptions = SkyboxVertex::getBindingDescriptions();
		pipelineConfig.attributeDescriptions = SkyboxVertex::getAttributeDescriptions();

		pipeline_ = std::make_unique<LePipeline>(
			device_,
			"shaders/skybox_vert_shader.spv",
			"shaders/skybox_frag_shader.spv",
			pipelineConfig
		);
	}

	void SkyboxRenderSystem::createPipelineLayout()
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts
		{
			frameLayout_,
            cubemapSetLayout_
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount =	static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(
			device_.device(),
			&pipelineLayoutInfo,
			nullptr,
			&pipelineLayout_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create skybox pipeline layout!");
		}
	}

	void SkyboxRenderSystem::createCubeBuffers()
	{
        std::vector<SkyboxVertex> vertices =
        {
            // back face
            {{-1.f, -1.f, -1.f}},
            {{ 1.f,  1.f, -1.f}},
            {{ 1.f, -1.f, -1.f}},

            {{ 1.f,  1.f, -1.f}},
            {{-1.f, -1.f, -1.f}},
            {{-1.f,  1.f, -1.f}},

            // front face
            {{-1.f, -1.f,  1.f}},
            {{ 1.f, -1.f,  1.f}},
            {{ 1.f,  1.f,  1.f}},

            {{ 1.f,  1.f,  1.f}},
            {{-1.f,  1.f,  1.f}},
            {{-1.f, -1.f,  1.f}},

            // left face
            {{-1.f,  1.f,  1.f}},
            {{-1.f,  1.f, -1.f}},
            {{-1.f, -1.f, -1.f}},

            {{-1.f, -1.f, -1.f}},
            {{-1.f, -1.f,  1.f}},
            {{-1.f,  1.f,  1.f}},

            // right face
            {{ 1.f,  1.f,  1.f}},
            {{ 1.f, -1.f, -1.f}},
            {{ 1.f,  1.f, -1.f}},

            {{ 1.f, -1.f, -1.f}},
            {{ 1.f,  1.f,  1.f}},
            {{ 1.f, -1.f,  1.f}},

            // bottom face
            {{-1.f, -1.f, -1.f}},
            {{ 1.f, -1.f, -1.f}},
            {{ 1.f, -1.f,  1.f}},

            {{ 1.f, -1.f,  1.f}},
            {{-1.f, -1.f,  1.f}},
            {{-1.f, -1.f, -1.f}},

            // top face
            {{-1.f,  1.f, -1.f}},
            {{ 1.f,  1.f,  1.f}},
            {{ 1.f,  1.f, -1.f}},

            {{ 1.f,  1.f,  1.f}},
            {{-1.f,  1.f, -1.f}},
            {{-1.f,  1.f,  1.f}}
        };

        vertexCount_ = static_cast<uint32_t>(vertices.size());

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vertexBuffer_ = std::make_unique<LeBuffer>(
            device_,
            sizeof(SkyboxVertex),
            vertexCount_,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vertexBuffer_->map();
        vertexBuffer_->writeToBuffer(vertices.data());
	}

    void SkyboxRenderSystem::createCubemapDescriptorResources()
    {
        cubemap_ = std::make_unique<LeCubemap>(
            device_,
            std::array<std::string, 6>
            {
                "skybox/uw_rt.jpg",
                "skybox/uw_lf.jpg",
                "skybox/uw_up.jpg",
                "skybox/uw_dn.jpg",
                "skybox/uw_bk.jpg",
                "skybox/uw_ft.jpg"
            }   
        );

        VkDescriptorSetLayoutBinding samplerBinding{};

        samplerBinding.binding = 0;
        samplerBinding.descriptorCount = 1;
        samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &samplerBinding;

        if (vkCreateDescriptorSetLayout(
            device_.device(),
            &layoutInfo,
            nullptr,
            &cubemapSetLayout_
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create cubemap descriptor set layout!");
        }

        VkDescriptorPoolSize poolSize{};

        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo{};

        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(
            device_.device(),
            &poolInfo,
            nullptr,
            &cubemapDescriptorPool_
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create cubemap descriptor pool!");
        }

        VkDescriptorSetAllocateInfo allocInfo{};

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = cubemapDescriptorPool_;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &cubemapSetLayout_;

        if (vkAllocateDescriptorSets(
            device_.device(),
            &allocInfo,
            &cubemapDescriptorSet_
        ) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate cubemap descriptor set!");
        }

        VkDescriptorImageInfo imageInfo{};

        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = cubemap_->getImageView();
        imageInfo.sampler = cubemap_->getSampler();

        VkWriteDescriptorSet write{};

        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = cubemapDescriptorSet_;
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(
            device_.device(),
            1,
            &write,
            0,
            nullptr
        );
    }
}