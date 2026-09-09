#include "src/render/le_render_system_manager.hpp"

namespace le {
	LeRenderSystemManager::LeRenderSystemManager(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager) : leDevice(device), leRenderer(renderer)
	{
        basicRenderSystem = std::make_unique<BasicRenderSystem>(
            device,
            renderer.getSwapchainRenderPass(),
            resourceManager
        );
	}

    void LeRenderSystemManager::render(LeScene& scene)
    {
        if (auto commandBuffer = leRenderer.beginFrame()) {
            leRenderer.beginSwapChainRenderPass(commandBuffer);

            basicRenderSystem->renderActors(
                commandBuffer,
                scene.getActors(),
                scene.getRenderHitboxes(),
                scene.getCamera(),
                leRenderer.getFrameIndex()
            );

            if (renderOverlay_) {
                renderOverlay_(commandBuffer);
            }

            leRenderer.endSwapChainRenderPass(commandBuffer);
            leRenderer.endFrame();
        }
    }

    void LeRenderSystemManager::setRenderOverlay(RenderOverlay overlay)
    {
        renderOverlay_ = std::move(overlay);
    }
}