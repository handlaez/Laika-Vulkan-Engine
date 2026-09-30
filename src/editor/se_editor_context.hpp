#ifndef SE_EDITOR_CONTEXT_HPP
#define SE_EDITOR_CONTEXT_HPP

#include "src/editor/project/se_project_manager.hpp"
#include "src/editor/se_editor_selection.hpp"

namespace le {
    class LeCore;
    class LeScene;
    class LeResourceManager;
}

namespace se {
    class ModeController;
    class EditorSession;

    class EditorContext
    {
    public:
        le::LeCore& core;
        ProjectManager& projects;
        le::LeResourceManager& resources;
        EditorSelection& selection;

        EditorSession* session = nullptr;
        le::LeScene* scene = nullptr;
        ModeController* modeController = nullptr;
    };
} 

#endif