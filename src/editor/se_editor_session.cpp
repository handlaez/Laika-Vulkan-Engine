#include "se_editor_session.hpp"

#include "src/scene/le_scene.hpp"
#include "src/core/le_core.hpp"

namespace se {

    EditorSession::EditorSession(EditorContext& context)
        : context_(context)
    {
    }

    EditorSession::~EditorSession()
    {
        closeProject();
    }

    bool EditorSession::openProject(const std::filesystem::path& projectFile)
    {
        closeProject();

        if (!context_.projects.loadProject(projectFile))
        {
            return false;
        }

        context_.resources.reset();

        editorScene_ = std::make_unique<le::LeScene>(context_.core.getDevice(), context_.resources);
        modeController_ = std::make_unique<ModeController>(*editorScene_, laikaApp_);

        context_.scene = editorScene_.get();
        context_.modeController = modeController_.get();

        context_.selection.clear();

        // Temporary until scene serialization exists.
        laikaApp_.onLoad(*editorScene_);

        return true;
    }

    bool EditorSession::createProject(const std::filesystem::path& directory, const std::string& name)
    {
        closeProject();

        if (!context_.projects.createProject(directory, name))
        {
            return false;
        }

        context_.resources.reset();

        editorScene_ = std::make_unique<le::LeScene>(context_.core.getDevice(), context_.resources);
        modeController_ = std::make_unique<ModeController>(*editorScene_, laikaApp_);

        context_.scene = editorScene_.get();
        context_.modeController = modeController_.get();

        context_.selection.clear();

        // Temporary until scene serialization exists.
        laikaApp_.onLoad(*editorScene_);

        return true;
    }

    void EditorSession::closeProject()
    {
        if (modeController_)
        {
            modeController_->stop();
        }

        context_.modeController = nullptr;
        context_.scene = nullptr;
        context_.selection.clear();

        modeController_.reset();
        editorScene_.reset();

        context_.resources.reset();
        context_.projects.unloadProject();
    }

    void EditorSession::requestOpen(const std::filesystem::path& projectFile)
    {
        pendingAction_ = PendingAction::Open;
        pendingPath_ = projectFile;
        pendingName_.clear();
    }

    void EditorSession::requestCreate(const std::filesystem::path& directory, const std::string& name)
    {
        pendingAction_ = PendingAction::Create;
        pendingPath_ = directory;
        pendingName_ = name;
    }

    void EditorSession::requestClose()
    {
        pendingAction_ = PendingAction::Close;
        pendingPath_.clear();
        pendingName_.clear();
    }

    void EditorSession::processPendingAction()
    {
        switch (pendingAction_)
        {
        case PendingAction::None:
            return;

        case PendingAction::Open:
            openProject(pendingPath_);
            break;

        case PendingAction::Create:
            createProject(pendingPath_, pendingName_);
            break;

        case PendingAction::Close:
            closeProject();
            break;
        }

        pendingAction_ = PendingAction::None;
        pendingPath_.clear();
        pendingName_.clear();
    }

    bool EditorSession::hasProject() const
    {
        return context_.projects.hasProject();
    }

    bool EditorSession::isPlaying() const
    {
        return context_.modeController != nullptr && context_.modeController->getState() != PlayState::Edit;
    }

    le::LeScene* EditorSession::getActiveScene()
    {
        return context_.modeController ? &context_.modeController->getActiveScene() : nullptr;
    }

    const le::LeScene* EditorSession::getActiveScene() const
    {
        return context_.modeController ? &context_.modeController->getActiveScene() : nullptr;
    }
}