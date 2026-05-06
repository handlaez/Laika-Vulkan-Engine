#pragma once

#include "le_frame_info.hpp"
#include "le_scene.hpp"

namespace le {
	class ILaikaEngineApp {
	public:
		virtual ~ILaikaEngineApp() = default;

		virtual void onStart(LeScene& scene) = 0;
		virtual void onUpdate(LeScene& scene, FrameInfo fi) = 0;
		virtual void onShutdown() {};
	};
}