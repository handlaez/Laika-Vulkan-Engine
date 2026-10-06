#include "se_editor_session.hpp"

#include "src/scene/le_scene.hpp"
#include "src/core/le_core.hpp"

namespace se {

    EditorSession::EditorSession(EditorContext& context, le::log::Logger& logger)
        : context_(context), laikaApp_(logger)
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

        auto* project = context_.projects.getProject();

        if (!project)
        {
            closeProject();
            return false;
        }

        laikaApp_.configureScriptProject(
            project->getScriptProjectFile(),
            "managed/Laika.Managed.dll",
            project->getScriptAssembly(),
            project->getScriptRuntimeConfig()
        );

        const auto scenePath = project->getRoot() / project->getStartupScene();

        if (!openScene(scenePath))
        {
            closeProject();
            return false;
        }

        return true;
    }

    bool EditorSession::createProject(const std::filesystem::path& directory, const std::string& name)
    {
        closeProject();

        if (!context_.projects.createProject(directory, name))
        {
            return false;
        }

        auto* project = context_.projects.getProject();

        if (!project)
        {
            closeProject();
            return false;
        }

        laikaApp_.configureScriptProject(
            project->getScriptProjectFile(), "managed/Laika.Managed.dll", project->getScriptAssembly(), project->getScriptRuntimeConfig());

        return createScene("Main");
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

        scenePath_.clear();
        sceneModified_ = false;
    }

    bool EditorSession::openScene(const std::filesystem::path& path)
    {
        const auto* project = context_.projects.getProject();

        if (!project)
        {
            return false;
        }

        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        {
            return false;
        }

        if (modeController_)
        {
            modeController_->stop();
        }

        context_.modeController = nullptr;
        context_.scene = nullptr;
        context_.selection.clear();

        modeController_.reset();
        editorScene_.reset();

        // rebuild resources for the newly opened scene.
        context_.resources.reset();

        auto newScene = std::make_unique<le::LeScene>(context_.core.getDevice(), context_.resources);

        if (!SceneSerializer::load(*newScene, *project, context_.resources, path))
        {
            return false;
        }

        editorScene_ = std::move(newScene);
        modeController_ = std::make_unique<ModeController>(*editorScene_, laikaApp_);

        context_.scene = editorScene_.get();
        context_.modeController = modeController_.get();
        context_.selection.clear();

        scenePath_ = path;
        sceneModified_ = false;

        return true;
    }

    bool EditorSession::createScene(const std::string& name)
    {
        const auto* project = context_.projects.getProject();

        if (!project || name.empty())
        {
            return false;
        }

        const std::filesystem::path path = project->getSceneDirectory() / (name + ".scene");

        if (std::filesystem::exists(path))
        {
            return false;
        }

        auto newScene = std::make_unique<le::LeScene>(context_.core.getDevice(), context_.resources);

        newScene->setName(name);

        if (!SceneSerializer::save(*newScene, *project, context_.resources, path))
        {
            return false;
        }

        return openScene(path);
    }

    bool EditorSession::saveScene()
    {
        const auto* project = context_.projects.getProject();

        if (!project || !editorScene_ || scenePath_.empty())
        {
            return false;
        }

        if (!SceneSerializer::save(*editorScene_, *project, context_.resources, scenePath_))
        {
            return false;
        }

        sceneModified_ = false;
        return true;
    }

    bool EditorSession::saveSceneAs(const std::filesystem::path& path)
    {
        const auto* project = context_.projects.getProject();

        if (!project || !editorScene_)
        {
            return false;
        }

        if (path.empty())
        {
            return false;
        }

        if (!SceneSerializer::save(*editorScene_, *project, context_.resources, path))
        {
            return false;
        }

        scenePath_ = path;
        sceneModified_ = false;

        return true;
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

    void EditorSession::requestCreateScene(const std::string& name)
    {
        pendingAction_ = PendingAction::CreateScene;
        pendingPath_.clear();
        pendingName_ = name;
    }

    void EditorSession::requestOpenScene(const std::filesystem::path& path)
    {
        pendingAction_ = PendingAction::OpenScene;
        pendingPath_ = path;
        pendingName_.clear();
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

        case PendingAction::Create:
            createProject(pendingPath_, pendingName_);
            break;

        case PendingAction::Open:
            openProject(pendingPath_);
            break;

        case PendingAction::CreateScene:
            createScene(pendingName_);
            break;

        case PendingAction::OpenScene:
            openScene(pendingPath_);
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

    bool EditorSession::isSceneModified() const
    {
        return sceneModified_;
    }

    void EditorSession::setSceneModified()
    {
        sceneModified_ = true;
    }

    const std::filesystem::path& EditorSession::getScenePath() const
    {
        return scenePath_;
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