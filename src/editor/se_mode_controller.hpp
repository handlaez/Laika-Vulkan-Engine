#ifndef SE_MODE_CONTROLLER_HPP
#define SE_MODE_CONTROLLER_HPP

#include "src/scene/le_scene.hpp"
#include "src/laika_app.hpp"

#include <memory>

namespace se {

    enum class PlayState {
        Edit,
        Run,
        Pause
    };

    class ModeController {
    public:
        ModeController(le::LeScene& editorScene, LaikaApp& app)
            : editorScene_(editorScene), app_(app)
        {
        }

        void run();
        void pause();
        void resume();
        void restart();
        void stop();

        void update(const le::FrameInfo& frameInfo, bool viewportActive);

        PlayState getState() const {
            return state_;
        }

        bool isEditing() const {
            return state_ == PlayState::Edit;
        }

        bool isRunning() const {
            return state_ == PlayState::Run;
        }

        bool isPaused() const {
            return state_ == PlayState::Pause;
        }

        le::LeScene& getActiveScene() {
            if (runtimeScene_) {
                return *runtimeScene_;
            }

            return editorScene_;
        }

        const le::LeScene& getActiveScene() const {
            if (runtimeScene_) {
                return *runtimeScene_;
            }

            return editorScene_;
        }

    private:
        le::LeScene& editorScene_;
        LaikaApp& app_;

        std::unique_ptr<le::LeScene> runtimeScene_;
        PlayState state_ = PlayState::Edit;
    };

}

#endif