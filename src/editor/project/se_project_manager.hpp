#ifndef SE_PROJECT_MANAGER_HPP
#define SE_PROJECT_MANAGER_HPP

#include "src/editor/project/se_project.hpp"

#include <filesystem>
#include <string>
#include <memory>

namespace se {
    class ProjectManager
    {
    public:
        bool createProject(const std::filesystem::path& directory, const std::string& name);
        bool loadProject(const std::filesystem::path& projectFile);
        void unloadProject();

        bool hasProject() const;
        Project* getProject();
        const Project* getProject() const;

    private:
        std::unique_ptr<Project> project_;
    };
}

#endif