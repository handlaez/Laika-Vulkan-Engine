#ifndef DEMO_APP_HPP
#define DEMO_APP_HPP


#include "src/i_laika_engine_app.hpp"
#include "src/systems/keyboard_movement_controller.hpp"
#include "src/core/le_frame_info.hpp"
#include "src/scene/le_scene.hpp"

using namespace le;

class DemoApp : public ILaikaEngineApp {
public:
    void onStart(LeScene& scene) override;
    void onUpdate(LeScene& scene, FrameInfo fi) override;
    void onShutdown() override;

private:
    KeyboardMovementController controller{};
};

#endif