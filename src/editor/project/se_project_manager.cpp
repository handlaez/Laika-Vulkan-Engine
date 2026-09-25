#include "se_project_manager.hpp"

#include <fstream>

namespace se {

    bool ProjectManager::createProject(const std::filesystem::path& directory, const std::string& name)
    {
        if (name.empty()) {
            return false;
        }

        if (std::filesystem::exists(directory) && !std::filesystem::is_directory(directory))
        {
            return false;
        }

        try {
            std::filesystem::create_directories(directory);

            Project project(directory, name);

            std::filesystem::create_directories(project.getAssetDirectory());
            std::filesystem::create_directories(project.getSceneDirectory());
            std::filesystem::create_directories(project.getScriptDirectory());

            std::ofstream projectFile(project.getProjectFile());

            if (!projectFile) {
                return false;
            }

            projectFile << "{\n";
            projectFile << "  \"name\": \"" << name << "\",\n";
            projectFile << "  \"version\": 1,\n";
            projectFile << "  \"startupScene\": \"Scenes/Main.scene\"\n";
            projectFile << "}\n";

            project_ = std::make_unique<Project>(std::move(project));

            return true;
        }
        catch (const std::filesystem::filesystem_error&) {
            return false;
        }
    }

    bool ProjectManager::loadProject(const std::filesystem::path& projectFile)
    {
        if (!std::filesystem::exists(projectFile)) {
            return false;
        }

        if (!std::filesystem::is_regular_file(projectFile)) {
            return false;
        }

        const auto root = projectFile.parent_path();
        const auto name = projectFile.stem().string();

        try {
            project_ = std::make_unique<Project>(root, name);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    void ProjectManager::unloadProject()
    {
        project_.reset();
    }

    bool ProjectManager::hasProject() const
    {
        return project_ != nullptr;
    }

    Project* ProjectManager::getProject()
    {
        return project_.get();
    }

    const Project* ProjectManager::getProject() const
    {
        return project_.get();
    }
}