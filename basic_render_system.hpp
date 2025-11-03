#ifndef BASIC_RENDER_SYSTEM_HPP
#define BASIC_RENDER_SYSTEM_HPP

#include "le_camera.hpp"
#include "le_pipeline.hpp"
#include "le_device.hpp"
#include "le_actor.hpp"

// std
#include <memory>
#include <vector>

namespace le {

	struct UniformBufferObject {
		glm::mat4 view;
		glm::mat4 proj;
	};

	class BasicRenderSystem {
	public:

		BasicRenderSystem(LeDevice &device, VkRenderPass renderPass, VkImageView imageView);
		~BasicRenderSystem();

		BasicRenderSystem(const BasicRenderSystem&) = delete;
		BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;

		void renderActors(VkCommandBuffer commandBuffer, std::vector<LeActor>& actors, const LeCamera& camera, size_t currentFrame);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		void createUniformBuffers();
		void createDescriptorSetLayout(LeDevice& device);
		void createDescriptorPool();
		void createDescriptorSets(VkImageView imageView);
		void createTextureSampler();

		LeDevice& leDevice;

		VkDescriptorSetLayout descriptorSetLayout{};
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkBuffer> uniformBuffers; // one per swapchain image
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		std::unique_ptr<LePipeline> lePipeline;
		VkPipelineLayout pipelineLayout;

		VkSampler textureSampler;
	};
}

#endif