#ifndef LE_SCRIPT_SYSTEM_HPP
#define LE_SCRIPT_SYSTEM_HPP

#include "le_script_runtime.hpp"

#include "src/logger/le_logger.hpp"

#include <filesystem>
#include <cstdint>
#include <unordered_set>

namespace le {

    class LeScene;

    class LeScriptSystem
    {
    public:
        LeScriptSystem(le::log::Logger& logger);

        using InitializeFn = int (*)(const char* assemblyPath);
        using CreateFn = int (*)(uint32_t actorId, const char* typeName);
        using UpdateFn = int (*)(uint32_t actorId, float deltaTime, float* position);
        using DestroyAllFn = void (*)();

        bool initialize(const std::filesystem::path& runtimeConfig, const std::filesystem::path& managedAssembly, const std::filesystem::path& gameAssembly);
        void start(LeScene& scene);
        void update(LeScene& scene, float deltaTime);
        void shutdown();

        void configureProject(
            const std::filesystem::path& scriptProject, const std::filesystem::path& managedAssembly,
            const std::filesystem::path& gameAssembly, const std::filesystem::path& runtimeConfig);
        void clearProject();

    private:
        bool ensureScriptProject();
        bool buildScriptProject();
        bool initializeRuntime();

        le::log::Logger& logger_;
        LeScriptRuntime runtime_;

        InitializeFn initializeFn_{ nullptr };
        CreateFn createFn_{ nullptr };
        UpdateFn updateFn_{ nullptr };
        DestroyAllFn destroyAllFn_{ nullptr };

        std::filesystem::path scriptProjectPath_;
        std::filesystem::path managedAssemblyPath_;
        std::filesystem::path gameAssemblyPath_;
        std::filesystem::path runtimeConfigPath_;

        bool configured_{ false };
        bool initialized_{ false };
    };

}

#endif