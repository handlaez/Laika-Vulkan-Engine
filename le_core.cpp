#include "le_core.hpp"
#include "le_frame_info.hpp"

#include "profiler.hpp"

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

        //profiler pt.1
        constexpr uint64_t maxMeasuredFrames = 5001;
        Profiler::Initialize(leDevice.device(), leDevice.getPhysicalDevice(), maxMeasuredFrames);
        uint64_t measuredFrames = 0;

        application.onStart(scene);

        renderManager.sync(scene);

        currentTime = std::chrono::high_resolution_clock::now();

        while (!leWindow.shouldClose()) {

            glfwPollEvents();

            updateFrameInfo(fi);

            Profiler::StartFrame();
            application.onUpdate(scene, fi);
            
            renderManager.render(scene);

            // profiler pt.2
            measuredFrames++;
            /*if (measuredFrames > maxMeasuredFrames)
            {
                break;
            }*/
        }

        // profiler pt.3
        vkDeviceWaitIdle(leDevice.device());
        
        Profiler::ExportToCSV("vulkan_profile.csv");
        Profiler::Shutdown();

        application.onShutdown();
        resourceManager.shutDown();
    }
} // le