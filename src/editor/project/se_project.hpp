#ifndef SE_PROJECT_HPP
#define SE_PROJECT_HPP

#include <filesystem>

namespace se {
    class Project
    {
    public:
        const std::filesystem::path& getRoot() const;
        const std::filesystem::path& getAssetDirectory() const;
        const std::filesystem::path& getSceneDirectory() const;
        const std::filesystem::path& getScriptDirectory() const;

    private:
        std::filesystem::path root_;
    };
}

#endif