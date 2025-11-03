#include "le_renderer.hpp"

// std
#include <stdexcept>
#include <array>
#include <iostream>

namespace le {

	LeRenderer::LeRenderer(LeWindow& window, LeDevice& device) : leWindow{ window }, leDevice{ device } {
		recreateSwapchain();
		createCommandBuffers();
	}

	LeRenderer::~LeRenderer() {	freeCommandBuffers(); }

	void LeRenderer::recreateSwapchain()
	{
		auto extent = leWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = leWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(leDevice.device()); //waiting until current swapchain is no longer being used

		if (leSwapchain == nullptr)
		{
			leSwapchain = std::make_unique<LeSwapchain>(leDevice, extent);
		}
		else
		{
			std::shared_ptr<LeSwapchain> oldSwpachain = std::move(leSwapchain);
			leSwapchain = std::make_unique<LeSwapchain>(leDevice, extent, oldSwpachain);

			if(!oldSwpachain->compareSwapFormats(*leSwapchain.get())) {
				throw std::runtime_error("Swapchain image(and/or depth) format has changed!");
			}
		}
	}

	void LeRenderer::createCommandBuffers()
	{
		commandBuffers.resize(LeSwapchain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = leDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(leDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers.");
		}
	}

	void LeRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(leDevice.device(), leDevice.getCommandPool(), static_cast<float>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer LeRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame; frame is already in progress");
		auto result = leSwapchain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image."); //TODO: handle suboptimal KHR (window resize)
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording for command buffer ");
		}

		return commandBuffer;
	}

	void LeRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame; frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record commands in Command Buffer");
		}

		auto result = leSwapchain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || leWindow.wasWindowResized())
		{
			leWindow.resetWindowResizedFlag();
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image.");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LeSwapchain::MAX_FRAMES_IN_FLIGHT;
	}


	void LeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a diffrent frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = leSwapchain->getRenderPass();
		renderPassInfo.framebuffer = leSwapchain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = leSwapchain->getSwapchainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(leSwapchain->getSwapchainExtent().width);
		viewport.height = static_cast<float>(leSwapchain->getSwapchainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, leSwapchain->getSwapchainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a diffrent frame");

		vkCmdEndRenderPass(commandBuffer);
	}

} // le