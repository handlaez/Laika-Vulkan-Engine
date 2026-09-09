#ifndef LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP
#define LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP

#include "src/render/le_renderer.hpp"
#include "basic_render_system.hpp"
#include "src/scene/le_scene.hpp"
#include "src/objects/le_texture.hpp"

#include <memory>
#include <functional>

namespace le {
	class LeRenderSystemManager {
	public:
		using RenderOverlay = std::function<void(VkCommandBuffer)>;

		LeRenderSystemManager(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager);
		
		void render(LeScene& scene);

		void setRenderOverlay(RenderOverlay overlay);
	private:
		LeDevice& leDevice;
		LeRenderer& leRenderer;

		std::unique_ptr<BasicRenderSystem> basicRenderSystem;

		RenderOverlay renderOverlay_{};
	};
}

#endif