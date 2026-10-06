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
        std::filesystem::path getAssetDirectory() const;
        std::filesystem::path getSceneDirectory() const;
        std::filesystem::path getScriptDirectory() const;

        std::filesystem::path getScriptProjectFile() const;
        std::filesystem::path getScriptBuildDirectory() const;
        std::filesystem::path getScriptAssembly() const;
        std::filesystem::path getScriptRuntimeConfig() const;
        
        const std::filesystem::path& getStartupScene() const;
        void setStartupScene(std::filesystem::path path);

    private:
        std::filesystem::path root_;
        std::string name_;
        std::filesystem::path startupScene_;
    };
}

#endif