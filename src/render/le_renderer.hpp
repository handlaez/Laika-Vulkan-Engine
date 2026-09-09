#ifndef LE_RENDERER_HPP
#define LE_RENDERER_HPP

#include "src/core/le_window.hpp"
#include "src/render/le_swapchain.hpp"
#include "src/core/le_device.hpp"
#include "src/render/le_scene_render_target.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace le {
	class LeRenderer {
	public:

		LeRenderer(LeWindow &window, LeDevice &device);
		~LeRenderer();

		LeRenderer(const LeRenderer&) = delete;
		LeRenderer& operator=(const LeRenderer&) = delete;

		VkRenderPass getSwapchainRenderPass() const { return leSwapchain->getRenderPass(); }
		float getAspectRatio() const { return leSwapchain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer, when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index, when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		LeSceneRenderTarget& getSceneRenderTarget() { return *sceneRenderTarget_; }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();

		LeWindow& leWindow;
		LeDevice& leDevice;
		std::unique_ptr<LeSwapchain> leSwapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		std::unique_ptr<LeSceneRenderTarget> sceneRenderTarget_;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}

#endif