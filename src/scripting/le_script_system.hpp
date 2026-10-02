#ifndef LE_SCRIPT_SYSTEM_HPP
#define LE_SCRIPT_SYSTEM_HPP

#include "le_script_runtime.hpp"

#include <filesystem>
#include <cstdint>
#include <unordered_set>

namespace le {

    class LeScene;

    class LeScriptSystem
    {
    public:
        using InitializeFn = int (*)(const char* assemblyPath);
        using CreateFn = int (*)(uint32_t actorId, const char* typeName);
        using UpdateFn = int (*)(uint32_t actorId, float deltaTime, float* position);
        using DestroyAllFn = void (*)();

    public:
        bool initialize(const std::filesystem::path& runtimeConfig, const std::filesystem::path& managedAssembly, const std::filesystem::path& gameAssembly);

        void start(LeScene& scene);
        void update(LeScene& scene, float deltaTime);
        void shutdown();

    private:
        LeScriptRuntime runtime_;

        InitializeFn initializeFn_{ nullptr };
        CreateFn createFn_{ nullptr };
        UpdateFn updateFn_{ nullptr };
        DestroyAllFn destroyAllFn_{ nullptr };

        bool initialized_{ false };
    };

}

#endif