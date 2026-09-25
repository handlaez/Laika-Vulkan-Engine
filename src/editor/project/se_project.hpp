#ifndef SE_PROJECT_HPP
#define SE_PROJECT_HPP

#include <filesystem>
#include <string>

namespace se {
    class Project
    {
    public:
        Project(std::filesystem::path root, std::string name);

        const std::string& getName() const;
        const std::filesystem::path& getRoot() const;
        std::filesystem::path getProjectFile() const;
        const std::filesystem::path getAssetDirectory() const;
        const std::filesystem::path getSceneDirectory() const;
        const std::filesystem::path getScriptDirectory() const;

    private:
        std::filesystem::path root_;
        std::string name_;
    };
}

#endif