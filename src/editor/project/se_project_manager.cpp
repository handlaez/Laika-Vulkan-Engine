#include "se_project_manager.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

namespace se {

    bool ProjectManager::createProject(const std::filesystem::path& directory, const std::string& name)
    {
        if (name.empty())
        {
            return false;
        }

        const std::filesystem::path projectRoot = directory / name;

        if (std::filesystem::exists(projectRoot))
        {
            return false;
        }

        try
        {
            std::filesystem::create_directories(projectRoot);

            Project project(projectRoot, name);

            std::filesystem::create_directories(project.getAssetDirectory());
            std::filesystem::create_directories(project.getSceneDirectory());
            std::filesystem::create_directories(project.getScriptDirectory());

            nlohmann::json data;

            data["name"] = name;
            data["version"] = 1;
            data["startupScene"] = "Scenes/Main.scene";

            std::ofstream projectFile(project.getProjectFile());

            if (!projectFile)
            {
                return false;
            }

            projectFile << data.dump(4);

            if (!projectFile.good())
            {
                return false;
            }

            project_ = std::make_unique<Project>(std::move(project));

            return true;
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return false;
        }
    }

    bool ProjectManager::loadProject(const std::filesystem::path& projectFile)
    {
        if (!std::filesystem::exists(projectFile) || !std::filesystem::is_regular_file(projectFile))
        {
            return false;
        }

        std::ifstream file(projectFile);

        if (!file.is_open())
        {
            return false;
        }

        nlohmann::json data;

        try
        {
            file >> data;
        }
        catch (const nlohmann::json::exception&)
        {
            return false;
        }

        const auto root = projectFile.parent_path();
        const std::string name = data.value("name", projectFile.stem().string());

        auto project = std::make_unique<Project>(root, name);

        if (data.contains("startupScene") && data["startupScene"].is_string())
        {
            project->setStartupScene(data["startupScene"].get<std::string>());
        }

        project_ = std::move(project);

        return true;
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