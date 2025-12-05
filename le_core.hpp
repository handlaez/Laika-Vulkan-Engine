#ifndef LE_CORE_HPP
#define LE_CORE_HPP

// these ones should not (most probably) be here, but in corresponding manager classes (and im working on it)
#include "le_window.hpp"
#include "le_camera.hpp"
#include "le_device.hpp"
#include "le_actor.hpp"
#include "le_renderer.hpp"
#include "le_scene.hpp"

#include "le_render_system_manager.hpp"
#include "le_resource_manager.hpp"

#include "i_laika_engine_app.hpp"

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