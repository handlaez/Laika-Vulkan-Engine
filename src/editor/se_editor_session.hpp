#ifndef SE_EDITOR_SESSION_HPP
#define SE_EDITOR_SESSION_HPP

#include "src/editor/se_editor_context.hpp"
#include "src/editor/se_mode_controller.hpp"
#include "src/scene/se_scene_serializer.hpp"
#include "src/laika_app.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace se {

    class EditorSession {
    public:
        explicit EditorSession(EditorContext& context);
        ~EditorSession();

        bool openProject(const std::filesystem::path& projectFile);
        bool createProject(const std::filesystem::path& directory, const std::string& name);
        void closeProject();

        bool openScene(const std::filesystem::path& path);
        bool createScene(const std::string& name);
        bool saveScene();
        bool saveSceneAs(const std::filesystem::path& path);

        void requestOpen(const std::filesystem::path& projectFile);
        void requestCreate(const std::filesystem::path& directory, const std::string& name);
        void requestClose();

        void processPendingAction();

        bool hasProject() const;
        bool isPlaying() const;

        bool isSceneModified() const;
        void setSceneModified();

        const std::filesystem::path& getScenePath() const;
        le::LeScene* getActiveScene();
        const le::LeScene* getActiveScene() const;

    private:
        enum class PendingAction {
            None,
            Open,
            Create,
            Close
        };

        std::filesystem::path scenePath_;
        bool sceneModified_{ false };

        EditorContext& context_;

        LaikaApp laikaApp_;
        std::unique_ptr<le::LeScene> editorScene_;
        std::unique_ptr<ModeController> modeController_;

        PendingAction pendingAction_ = PendingAction::None;
        std::filesystem::path pendingPath_;
        std::string pendingName_;
    };
}

#endif