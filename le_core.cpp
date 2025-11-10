#include "le_core.hpp"
#include "le_frame_info.hpp"

namespace le {

	LeCore::LeCore() : currentTime(std::chrono::high_resolution_clock::now()) {}

	LeCore::~LeCore() {}

    void LeCore::updateFrameInfo(FrameInfo& fi) {
        auto newTime = std::chrono::high_resolution_clock::now();
        fi.deltaTime = std::chrono::duration<float>(newTime - currentTime).count();
        currentTime = newTime;

        fi.aspect = leRenderer.getAspectRatio();
    }

    void LeCore::run(ILaikaEngineApp& application) {

        LeScene scene{ leDevice };
        LeRenderSystemManager renderManager{ leDevice, leRenderer };
        FrameInfo fi{};

        application.onStart(scene);

        currentTime = std::chrono::high_resolution_clock::now();

        while (!leWindow.shouldClose()) {

            glfwPollEvents();

            updateFrameInfo(fi);

            application.onUpdate(scene, fi);

            renderManager.render(scene);
        }

        application.onShutdown();
    }
} // le