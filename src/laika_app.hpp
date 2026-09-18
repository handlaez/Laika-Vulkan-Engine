#ifndef LAIKA_APP_HPP
#define LAIKA_APP_HPP

#include "src/i_laika_engine_app.hpp"
#include "src/systems/keyboard_movement_controller.hpp"
#include "src/core/le_frame_info.hpp"
#include "src/scene/le_scene.hpp"

class LaikaApp : public le::ILaikaEngineApp {
public:
    void onStart(le::LeScene& scene) override;
    void onUpdate(le::LeScene& scene, le::FrameInfo fi, bool viewportActive) override;
    void onShutdown(le::LeScene&) override;

private:
    le::KeyboardMovementController controller{};
};

#endif