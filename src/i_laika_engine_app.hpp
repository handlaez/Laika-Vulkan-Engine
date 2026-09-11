#pragma once

#include "src/core/le_frame_info.hpp"
#include "src/scene/le_scene.hpp"

namespace le {
	class ILaikaEngineApp {
	public:
		virtual ~ILaikaEngineApp() = default;

		virtual void onStart(LeScene& scene) = 0;
		virtual void onUpdate(LeScene& scene, FrameInfo fi, bool viewportActive) = 0;
		virtual void onShutdown() {};
	};
}