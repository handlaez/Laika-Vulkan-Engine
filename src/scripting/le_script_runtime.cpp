#include "le_script_runtime.hpp"

#include <nethost.h>

#include <iostream>
#include <vector>

namespace le {

    LeScriptRuntime::~LeScriptRuntime()
    {
        // Do not FreeLibrary(hostFxrLibrary_). -- undefined
    }

    bool LeScriptRuntime::loadHostFxr()
    {
#ifdef _WIN32

        char_t buffer[MAX_PATH];
        size_t bufferSize = sizeof(buffer) / sizeof(char_t);

        const int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);

        if (rc != 0)
        {
            std::cerr << "get_hostfxr_path failed: 0x" << std::hex << rc << '\n';
            return false;
        }

        hostFxrLibrary_ = LoadLibraryW(buffer);

        if (!hostFxrLibrary_)
        {
            std::cerr << "Failed to load hostfxr.dll\n";
            return false;
        }

        initializeForRuntimeConfig_ = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(GetProcAddress(hostFxrLibrary_, "hostfxr_initialize_for_runtime_config"));
        getRuntimeDelegate_ = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(GetProcAddress(hostFxrLibrary_, "hostfxr_get_runtime_delegate"));
        closeHostContext_ = reinterpret_cast<hostfxr_close_fn>(GetProcAddress(hostFxrLibrary_, "hostfxr_close"));

        if (!initializeForRuntimeConfig_ || !getRuntimeDelegate_ || !closeHostContext_)
        {
            std::cerr << "Failed to load hostfxr functions\n";
            return false;
        }

        return true;
#else
        return false;
#endif
    }

    bool LeScriptRuntime::initialize(const std::filesystem::path& runtimeConfigPath)
    {
        if (!std::filesystem::exists(runtimeConfigPath))
        {
            std::cerr << "Runtime config does not exist: " << runtimeConfigPath.string() << '\n';
            return false;
        }

        if (!loadHostFxr())
        {
            return false;
        }

        hostfxr_handle context = nullptr;

        const int rc = initializeForRuntimeConfig_(runtimeConfigPath.c_str(), nullptr, &context);

        // hostfxr can return successful "already initialized" states in addition to zero.
        if (rc < 0 || context == nullptr)
        {
            std::cerr << "hostfxr_initialize_for_runtime_config failed: 0x" << std::hex << rc << '\n';
            return false;
        }

        void* delegate = nullptr;
        const int delegateRc = getRuntimeDelegate_(context, hdt_load_assembly_and_get_function_pointer, &delegate);

        if (delegateRc != 0 || delegate == nullptr)
        {
            std::cerr << "hostfxr_get_runtime_delegate failed: 0x" << std::hex << delegateRc << '\n';

            closeHostContext_(context);
            return false;
        }

        loadAssemblyAndGetFunctionPointer_ = reinterpret_cast<load_assembly_and_get_function_pointer_fn>(delegate);
        closeHostContext_(context);

        return true;
    }

    int LeScriptRuntime::invoke(const std::filesystem::path& assemblyPath, const std::wstring& typeName, const std::wstring& methodName)
    {
        if (!loadAssemblyAndGetFunctionPointer_)
        {
            std::cerr << "Script runtime has not been initialized\n";
            return -1;
        }

        if (!std::filesystem::exists(assemblyPath))
        {
            std::cerr << "Assembly does not exist: " << assemblyPath.string() << '\n';
            return -1;
        }

        void* functionPointer = nullptr;

        const int rc = loadAssemblyAndGetFunctionPointer_(
                assemblyPath.c_str(),
                typeName.c_str(),
                methodName.c_str(),
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                &functionPointer
            );

        if (rc != 0 || functionPointer == nullptr)
        {
            std::cerr << "Failed to get managed function pointer: 0x" << std::hex << rc << '\n';
            return -1;
        }

        auto entryPoint = reinterpret_cast<component_entry_point_fn>(functionPointer);

        return entryPoint(nullptr, 0);
    }

}