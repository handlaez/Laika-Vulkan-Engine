#ifndef LE_CORE_HPP
#define LE_CORE_HPP

// these ones should not (most probably) be here, but in corresponding manager classes (and im working on it)
#include "src/core/le_window.hpp"
#include "src/objects/le_camera.hpp"
#include "src/core/le_device.hpp"
#include "src/objects/le_actor.hpp"
#include "src/render/le_renderer.hpp"
#include "src/scene/le_scene.hpp"

#include "src/render/le_render_system_manager.hpp"
#include "src/scene/le_resource_manager.hpp"

#include "src/i_laika_engine_app.hpp"

// std
#include <memory>
#include <chrono>

namespace le {
	class LeCore {
		// when core is created a window is created -> when core is destroyed, any window will be destroyed too.
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 800;

		LeCore();
		~LeCore();

		LeCore(const LeCore&) = delete;
		LeCore& operator=(const LeCore&) = delete;

		void run(ILaikaEngineApp& gameApp);

	private:
		void updateFrameInfo(FrameInfo& fi);

		LeWindow leWindow{ WIDTH, HEIGHT, "Le_Core_Window" };
		LeDevice leDevice{ leWindow };
		LeRenderer leRenderer{ leWindow, leDevice };

		std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
	};
}

#endif