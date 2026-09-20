#include "se_mode_controller.hpp"

namespace se {

	void ModeController::run()
	{
		if (state_ != PlayState::Edit) {
			return;
		}

		runtimeScene_ = editorScene_.clone();

		app_.onStart(*runtimeScene_);

		state_ = PlayState::Run;
	}

	void ModeController::pause()
	{
		if (state_ == PlayState::Run) {
			state_ = PlayState::Pause;
		}
	}

	void ModeController::resume()
	{
		if (state_ == PlayState::Pause) {
			state_ = PlayState::Run;
		}
	}

	void ModeController::restart()
	{
		if (state_ == PlayState::Edit) {
			run();
			return;
		}

		stop();
		run();
	}

	void ModeController::stop()
	{
		if (!runtimeScene_) {
			state_ = PlayState::Edit;
			return;
		}

		app_.onShutdown(*runtimeScene_);

		runtimeScene_.reset();
		state_ = PlayState::Edit;
	}

	void ModeController::update(const le::FrameInfo& frameInfo, bool viewportActive)
	{
		if (state_ != PlayState::Run) {
			return;
		}

		app_.onUpdate(*runtimeScene_, frameInfo, viewportActive);
	}
}