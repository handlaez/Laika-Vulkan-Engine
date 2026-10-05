#ifndef LAIKA_APP_HPP
#define LAIKA_APP_HPP

#include "src/i_laika_engine_app.hpp"
#include "src/systems/keyboard_movement_controller.hpp"
#include "src/core/le_frame_info.hpp"
#include "src/scene/le_scene.hpp"
#include "src/scripting/le_script_system.hpp"

class LaikaApp : public le::ILaikaEngineApp {
public:
    explicit LaikaApp(le::log::Logger& logger);

    void onStart(le::LeScene& scene) override;
    void onUpdate(le::LeScene& scene, le::FrameInfo fi, bool viewportActive) override;
    void onShutdown(le::LeScene&) override;

    void configureScriptProject(
        const std::filesystem::path& scriptProject, const std::filesystem::path& managedAssembly,
        const std::filesystem::path& gameAssembly, const std::filesystem::path& runtimeConfig);
    void clearScriptProject();

private:
    le::KeyboardMovementController controller_{};
    le::LeScriptSystem scriptSystem_;
};

#endif