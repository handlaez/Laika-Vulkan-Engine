#include "basic_render_system.hpp"
#include "le_swapchain.hpp"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <iostream>

namespace le {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};

	BasicRenderSystem::BasicRenderSystem(LeDevice& device, VkRenderPass renderPass) : leDevice{ device } {
		createDescriptorSetLayout(device);
		createPipelineLayout();
		createPipeline(renderPass);
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
	}

	BasicRenderSystem::~BasicRenderSystem() {
		vkDestroyPipelineLayout(leDevice.device(), pipelineLayout, nullptr);

		for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(leDevice.device(), uniformBuffers[i], nullptr);
			vkFreeMemory(leDevice.device(), uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(leDevice.device(), descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(leDevice.device(), descriptorSetLayout, nullptr);
	}

	void BasicRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(leDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	void BasicRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lePipeline = std::make_unique<LePipeline>(
			leDevice,
			"vert_shader.vert.spv",
			"frag_shader.frag.spv",
			pipelineConfig);
	}

	void BasicRenderSystem::createDescriptorSetLayout(LeDevice& device)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout");
		}
	}

	void BasicRenderSystem::createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			leDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(leDevice.device(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void BasicRenderSystem::createDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(leDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create descriptor pool!");
		}
	}

	void BasicRenderSystem::createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(LeSwapchain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(LeSwapchain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(leDevice.device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < LeSwapchain::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject); //VK_WHOLE_SIZE in this case

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(leDevice.device(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	void BasicRenderSystem::renderActors(VkCommandBuffer commandBuffer, std::vector<LeActor>& actors, const LeCamera& camera, size_t currentFrame)
	{
		lePipeline->bind(commandBuffer);

		UniformBufferObject ubo{};
		ubo.view = camera.getView();
		ubo.proj = camera.getProjection();
		memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,	0, 1, &descriptorSets[currentFrame], 0, nullptr);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : actors) {
			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

} // le