#ifndef DEMO_APP_HPP
#define DEMO_APP_HPP


#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_frame_info.hpp"
#include "le_scene.hpp"

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