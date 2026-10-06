#include "le_script_system.hpp"

#include "src/scene/le_scene.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>

namespace le {

    LeScriptSystem::LeScriptSystem(le::log::Logger& logger) : logger_(logger)
    {
    }

    bool LeScriptSystem::initialize(const std::filesystem::path& runtimeConfig, const std::filesystem::path& managedAssembly, const std::filesystem::path& gameAssembly)
    {
        if (!runtime_.initialize(runtimeConfig))
        {
            return false;
        }

        initializeFn_ = reinterpret_cast<InitializeFn>(
            runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"Initialize")
                );

        createFn_ = reinterpret_cast<CreateFn>(
            runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"Create")
                );

        updateFn_ = reinterpret_cast<UpdateFn>(
            runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"Update")
                );

        destroyAllFn_ = reinterpret_cast<DestroyAllFn>(
            runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"DestroyAll")
                );

        if (!initializeFn_ || !createFn_ || !updateFn_ || !destroyAllFn_)
        {
            std::cerr << "[Script] Failed to acquire managed entry points\n";
            return false;
        }

        initialized_ = true;
        return true;
    }

    void LeScriptSystem::start(LeScene& scene)
    {
        if (!configured_)
        {
            logger_.write(le::log::Level::error, le::log::Category::script, "No script project configured.");
            return;
        }

        if (!buildScriptProject())
        {
            return;
        }

        if (!initializeRuntime())
        {
            return;
        }

        const std::string gameAssemblyPath = std::filesystem::absolute(gameAssemblyPath_).string();

        logger_.write(le::log::Level::info, le::log::Category::script,
            "Loading game assembly: " + std::filesystem::absolute(gameAssemblyPath_).string()
        );

        const int initializeResult = initializeFn_(gameAssemblyPath.c_str());

        logger_.write(le::log::Level::info, le::log::Category::script,
            "Managed Initialize returned " + std::to_string(initializeResult)
        );

        if (initializeResult != 0)
        {
            logger_.write(le::log::Level::error, le::log::Category::script, "Failed to load game assembly.");
            return;
        }

        for (const auto& actor : scene.getActors())
        {
            for (const auto& script : actor.getScripts())
            {
                if (!script.enabled || script.className.empty())
                {
                    continue;
                }

                const int result = createFn_(actor.getId(), script.className.c_str());

                if (result != 0)
                {
                    std::cout << "[Script] Failed to create script '" << script.className << "' for actor " << actor.getId() << '\n';
                }
            }
        }
    }

    void LeScriptSystem::update(LeScene& scene, float deltaTime)
    {
        if (!initialized_)
        {
            return;
        }

        for (auto& actor : scene.getActors())
        {
            if (!actor.hasScripts())
            {
                continue;
            }

            float position[3] = {
                actor.transform.translation.x,
                actor.transform.translation.y,
                actor.transform.translation.z
            };

            if (updateFn_(actor.getId(), deltaTime, position) != 0)
            {
                continue;
            }

            actor.transform.translation = { position[0], position[1], position[2] };
        }
    }

    void LeScriptSystem::shutdown()
    {
        if (!initialized_)
        {
            return;
        }

        destroyAllFn_();
        initialized_ = false;
    }

    void LeScriptSystem::configureProject(
        const std::filesystem::path& scriptProject, const std::filesystem::path& managedAssembly,
        const std::filesystem::path& gameAssembly, const std::filesystem::path& runtimeConfig)
    {
        scriptProjectPath_ = scriptProject;
        managedAssemblyPath_ = managedAssembly;
        gameAssemblyPath_ = gameAssembly;
        runtimeConfigPath_ = runtimeConfig;

        configured_ = true;

        logger_.write(le::log::Level::info, le::log::Category::script, "Configured script project: " + scriptProjectPath_.string());
    }
    
    void LeScriptSystem::clearProject()
    {

    }

    bool LeScriptSystem::ensureScriptProject()
    {
        if (scriptProjectPath_.empty() || managedAssemblyPath_.empty())
        {
            return false;
        }

        const auto scriptDirectory = scriptProjectPath_.parent_path();

        std::error_code error;
        std::filesystem::create_directories(scriptDirectory, error);

        if (error)
        {
            std::cout << "[Script] Failed to create script directory: " << error.message() << '\n';
            return false;
        }

        const auto localManagedAssembly = scriptDirectory / "Laika.Managed.dll";

        std::filesystem::copy_file(managedAssemblyPath_, localManagedAssembly, std::filesystem::copy_options::overwrite_existing, error);

        if (error)
        {
            std::cout << "[Script] Failed to copy Laika.Managed.dll: " << error.message() << '\n';
            return false;
        }

        if (std::filesystem::exists(scriptProjectPath_))
        {
            return true;
        }

        std::ofstream file(scriptProjectPath_);

        if (!file)
        {
            return false;
        }

        file
            << "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
            << "  <PropertyGroup>\n"
            << "    <TargetFramework>net10.0</TargetFramework>\n"
            << "    <OutputType>Library</OutputType>\n"
            << "    <ImplicitUsings>enable</ImplicitUsings>\n"
            << "    <Nullable>enable</Nullable>\n"
            << "    <GenerateRuntimeConfigurationFiles>true</GenerateRuntimeConfigurationFiles>\n"
            << "  </PropertyGroup>\n"
            << "\n"
            << "  <ItemGroup>\n"
            << "    <Reference Include=\"Laika.Managed\">\n"
            << "      <HintPath>Laika.Managed.dll</HintPath>\n"
            << "      <Private>true</Private>\n"
            << "    </Reference>\n"
            << "  </ItemGroup>\n"
            << "</Project>\n";

        return true;
    }

    bool LeScriptSystem::buildScriptProject()
    {
        if (!ensureScriptProject())
        {
            return false;
        }

        const auto command = "dotnet build \"" + scriptProjectPath_.string() + "\" --configuration Debug --nologo";
        std::cout << "[Script] Building " << scriptProjectPath_ << '\n';

        const int result = std::system(command.c_str());

        if (result != 0)
        {
            std::cout << "[Script] Build failed with code " << result << '\n';
            return false;
        }

        std::cout << "[Script] Build succeeded\n";
        return true;
    }

    bool LeScriptSystem::initializeRuntime()
    {
        if (initialized_)
        {
            return true;
        }

        if (!runtime_.initialize(runtimeConfigPath_))
        {
            logger_.write(
                le::log::Level::error,
                le::log::Category::script,
                "Failed to initialize .NET runtime."
            );

            return false;
        }

        initializeFn_ = reinterpret_cast<InitializeFn>(
            runtime_.getFunctionPointer(
                managedAssemblyPath_,
                L"Laika.ScriptHost, Laika.Managed",
                L"Initialize"
            )
            );

        createFn_ = reinterpret_cast<CreateFn>(
            runtime_.getFunctionPointer(
                managedAssemblyPath_,
                L"Laika.ScriptHost, Laika.Managed",
                L"Create"
            )
            );

        updateFn_ = reinterpret_cast<UpdateFn>(
            runtime_.getFunctionPointer(
                managedAssemblyPath_,
                L"Laika.ScriptHost, Laika.Managed",
                L"Update"
            )
            );

        destroyAllFn_ = reinterpret_cast<DestroyAllFn>(
            runtime_.getFunctionPointer(
                managedAssemblyPath_,
                L"Laika.ScriptHost, Laika.Managed",
                L"DestroyAll"
            )
            );

        if (!initializeFn_ ||
            !createFn_ ||
            !updateFn_ ||
            !destroyAllFn_)
        {
            logger_.write(
                le::log::Level::error,
                le::log::Category::script,
                "Failed to acquire managed entry points."
            );

            return false;
        }

        initialized_ = true;

        logger_.write(
            le::log::Level::info,
            le::log::Category::script,
            ".NET scripting runtime initialized."
        );

        return true;
    }
}