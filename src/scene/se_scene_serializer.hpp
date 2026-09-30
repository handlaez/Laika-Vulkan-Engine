#ifndef LE_SCENE_SERIALIZER_HPP
#define LE_SCENE_SERIALIZER_HPP

#include <filesystem>

namespace le {
    class LeScene;
    class LeResourceManager;
}

namespace se {

    class Project;

    class SceneSerializer {
    public:
        static bool save(const le::LeScene& scene, const Project& project, const le::LeResourceManager& resources, const std::filesystem::path& scenePath);
        static bool load(le::LeScene& scene, const Project& project, le::LeResourceManager& resources, const std::filesystem::path& scenePath);
    };

}

#endif