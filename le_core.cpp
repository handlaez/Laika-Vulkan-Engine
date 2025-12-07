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

        LeResourceManager resourceManager{ leDevice }; // holds all the textures and models
        resourceManager.startUp();
        LeRenderSystemManager renderManager{ leDevice, leRenderer, resourceManager }; // actualy renders all the textures and models
        LeScene scene{ leDevice, resourceManager }; // is an environment where models can be rendered
        FrameInfo fi{};
        fi.window = leWindow.getGLFWwindow();

        application.onStart(scene);

        currentTime = std::chrono::high_resolution_clock::now();

        while (!leWindow.shouldClose()) {

            glfwPollEvents();

            updateFrameInfo(fi);

            application.onUpdate(scene, fi);

            renderManager.render(scene);
        }

        application.onShutdown();
        resourceManager.shutDown();
    }
} // le