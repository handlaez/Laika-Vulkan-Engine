#ifndef LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP
#define LE_RENDER_SYSTEM_MANAGER_RENDERER_HPP

#include "le_renderer.hpp"
#include "basic_render_system.hpp"
#include "le_scene.hpp"
#include "le_texture.hpp"

namespace le {
	class LeRenderSystemManager {
	public:
		LeRenderSystemManager(LeDevice& device, LeRenderer& renderer, LeResourceManager& resourceManager);
		void render(LeScene& scene);

	private:
		LeDevice& leDevice;
		LeRenderer& leRenderer;
		std::unique_ptr<BasicRenderSystem> basicRenderSystem;
	};
}

#endif