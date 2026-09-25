#ifndef SE_EDITOR_SESSION_HPP
#define SE_EDITOR_SESSION_HPP

#include "src/editor/se_editor_context.hpp"
#include "src/editor/se_mode_controller.hpp"
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

        void requestOpen(const std::filesystem::path& projectFile);
        void requestCreate(const std::filesystem::path& directory, const std::string& name);
        void requestClose();

        void processPendingAction();

        bool hasProject() const;
        bool isPlaying() const;

        le::LeScene* getActiveScene();
        const le::LeScene* getActiveScene() const;

    private:
        enum class PendingAction {
            None,
            Open,
            Create,
            Close
        };

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