#include "le_script_system.hpp"

#include "src/scene/le_scene.hpp"

#include <iostream>

namespace le {

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

        updateFn_ =
            reinterpret_cast<UpdateFn>(
                runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"Update")
                );

        destroyAllFn_ =
            reinterpret_cast<DestroyAllFn>(
                runtime_.getFunctionPointer(managedAssembly, L"Laika.ScriptHost, Laika.Managed", L"DestroyAll")
                );

        if (!initializeFn_ || !createFn_ || !updateFn_ || !destroyAllFn_)
        {
            return false;
        }

        if (initializeFn_(gameAssembly.string().c_str()) != 0)
        {
            return false;
        }

        initialized_ = true;
        return true;
    }

    void LeScriptSystem::start(LeScene& scene)
    {
        if (!initialized_)
        {
            return;
        }

        for (const auto& actor : scene.getActors())
        {
            if (!actor.hasScript())
            {
                continue;
            }

            const int result = createFn_(actor.getId(), actor.getScriptClassName().c_str());

            if (result != 0)
            {
                std::cerr << "Failed to create script '" << actor.getScriptClassName() << "' for actor " << actor.getId() << '\n';
            }
        }
    }

    void LeScriptSystem::update(LeScene& scene, float deltaTime)
    {
        if (!initialized_)
        {
            return;
        }

        for (const auto& actor : scene.getActors())
        {
            if (!actor.hasScript())
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

            auto* mutableActor = scene.getActorById(actor.getId());

            if (!mutableActor)
            {
                continue;
            }

            mutableActor->transform.translation = {
                position[0],
                position[1],
                position[2]
            };
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
}