#include "src/core/le_core.hpp"
#include "src/core/le_frame_info.hpp"

namespace le {

	LeCore::LeCore() : currentTime_(std::chrono::high_resolution_clock::now()) {
        resourceManager_.startUp();
        frameInfo_.window = leWindow_.getGLFWwindow();
    }

	LeCore::~LeCore() {
        resourceManager_.shutDown();
    }

    void LeCore::beginFrame()
    {
        glfwPollEvents();
        updateFrameInfo(frameInfo_);
    }

    void LeCore::update()
    {
    }

    void LeCore::render(LeScene& scene)
    {
        renderManager_.render(scene);
    }

    void LeCore::endFrame()
    {
    }

    void LeCore::updateFrameInfo(FrameInfo& fi) {
        auto newTime = std::chrono::high_resolution_clock::now();
        fi.deltaTime = std::chrono::duration<float>(newTime - currentTime_).count();
        currentTime_ = newTime;

        fi.aspect = leRenderer_.getAspectRatio();
    }

    LeWindow& LeCore::getWindow()
    {
        return leWindow_;
    }

    LeDevice& LeCore::getDevice()
    {
        return leDevice_;
    }

    LeRenderer& LeCore::getRenderer()
    {
        return leRenderer_;
    }

    LeResourceManager& LeCore::getResources()
    {
        return resourceManager_;
    }
} // le