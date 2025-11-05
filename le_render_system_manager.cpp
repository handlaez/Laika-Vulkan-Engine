#include "le_render_system_manager.hpp"

namespace le {
	LeRenderSystemManager::LeRenderSystemManager(LeDevice& device, LeRenderer& renderer) : leDevice(device), leRenderer(renderer)
	{
        // temp! loading a texture here until a proper TextureManager is implemented
        texture = std::make_unique<LeTexture>(device, "textures/texture.jpg");
        textureImageView = texture->getImageView();

        basicRenderSystem = std::make_unique<BasicRenderSystem>(
            device,
            renderer.getSwapchainRenderPass(),
            textureImageView
        );
	}

    void LeRenderSystemManager::render(LeScene& scene)
    {
        if (auto commandBuffer = leRenderer.beginFrame()) {
            leRenderer.beginSwapChainRenderPass(commandBuffer);

            basicRenderSystem->renderActors(
                commandBuffer,
                scene.getActors(),
                scene.getCamera(),
                leRenderer.getFrameIndex()
            );

            leRenderer.endSwapChainRenderPass(commandBuffer);
            leRenderer.endFrame();
        }
    }
}