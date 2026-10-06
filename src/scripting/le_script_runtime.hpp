#ifndef LE_SCRIPT_RUNTIME_HPP
#define LE_SCRIPT_RUNTIME_HPP

#include <filesystem>
#include <string>

#include <coreclr_delegates.h>
#include <hostfxr.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace le {

    class LeScriptRuntime
    {
    public:
        LeScriptRuntime() = default;
        ~LeScriptRuntime();

        LeScriptRuntime(const LeScriptRuntime&) = delete;
        LeScriptRuntime& operator=(const LeScriptRuntime&) = delete;

        bool initialize(const std::filesystem::path& runtimeConfigPath);
        int invoke(const std::filesystem::path& assemblyPath, const std::wstring& typeName, const std::wstring& methodName);

        void* getFunctionPointer(const std::filesystem::path& assemblyPath, const std::wstring& typeName, const std::wstring& methodName);

    private:
        bool loadHostFxr();

    private:
#ifdef _WIN32
        HMODULE hostFxrLibrary_{ nullptr };
#endif
        hostfxr_initialize_for_runtime_config_fn initializeForRuntimeConfig_{ nullptr };
        hostfxr_get_runtime_delegate_fn getRuntimeDelegate_{ nullptr };
        hostfxr_close_fn closeHostContext_{ nullptr };
        load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPointer_{ nullptr };
    };

}

#endif //LE_SCRIPT_RUNTIME_HPP