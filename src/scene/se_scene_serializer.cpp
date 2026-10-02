#include "se_scene_serializer.hpp"

#include "src/editor/project/se_project.hpp"
#include "src/scene/le_scene.hpp"
#include "src/objects/le_actor.hpp"
#include "src/scene/le_resource_manager.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_map>

namespace se {

    using json = nlohmann::json;

    namespace {

        json serializeVec3(const glm::vec3& value)
        {
            return { value.x, value.y, value.z };
        }

        glm::vec3 deserializeVec3(const json& value)
        {
            return { value[0].get<float>(), value[1].get<float>(), value[2].get<float>() };
        }

        json serializeQuat(const glm::quat& value)
        {
            return { value.x, value.y, value.z, value.w };
        }

        glm::quat deserializeQuat(const json& value)
        {
            return { value[3].get<float>(),value[0].get<float>(),value[1].get<float>(),value[2].get<float>() };
        }

        std::filesystem::path makeRelativeAssetPath(const Project& project, const std::filesystem::path& assetPath)
        {
            if (assetPath.empty())
            {
                return {};
            }

            return std::filesystem::relative(assetPath, project.getRoot());
        }

        std::filesystem::path makeAbsoluteAssetPath(const Project& project, const std::filesystem::path& assetPath)
        {
            if (assetPath.empty())
            {
                return {};
            }

            return project.getRoot() / assetPath;
        }

    }

    bool SceneSerializer::save(const le::LeScene& scene, const Project& project, const le::LeResourceManager& resources, const std::filesystem::path& scenePath)
    {
        json root;

        root["version"] = 1;
        root["name"] = scene.getName();

        const auto& cameraObject = scene.getCameraObject();

        json cameraJson;
        cameraJson["position"] = serializeVec3(cameraObject.transform.translation);
        cameraJson["rotation"] = serializeQuat(cameraObject.transform.rotation);
        root["camera"] = cameraJson;


        root["actors"] = json::array();

        for (const auto& actor : scene.getActors())
        {
            json actorJson;

            actorJson["id"] = actor.getId();

            if (actor.getParentId().has_value())
            {
                actorJson["parent"] = *actor.getParentId();
            }
            else
            {
                actorJson["parent"] = nullptr;
            }

            auto model = resources.findModel(actor.modelID);

            if (model)
            {
                auto staticModel = std::dynamic_pointer_cast<le::LeModel>(model);

                if (staticModel && !staticModel->getPath().empty())
                {
                    actorJson["model"] = makeRelativeAssetPath(project, staticModel->getPath()).generic_string();
                }
                else
                {
                    actorJson["model"] = "";
                }
            }
            else
            {
                actorJson["model"] = "";
            }

            auto texture = resources.findTexture(actor.textureID);

            if (texture && !texture->getPath().empty())
            {
                actorJson["texture"] = makeRelativeAssetPath(project, texture->getPath()).generic_string();
            }
            else
            {
                actorJson["texture"] = "";
            }

            actorJson["color"] = serializeVec3(actor.color);
            actorJson["position"] = serializeVec3(actor.transform.translation);
            actorJson["rotation"] = serializeQuat(actor.transform.rotation);
            actorJson["scale"] = serializeVec3(actor.transform.scale);
            actorJson["script"] = actor.getScriptClassName();

            root["actors"].push_back(actorJson);
        }

        std::ofstream file(scenePath);

        if (!file.is_open())
        {
            return false;
        }

        file << root.dump(4);

        return file.good();
    }

    bool SceneSerializer::load(le::LeScene& scene, const Project& project, le::LeResourceManager& resources, const std::filesystem::path& scenePath)
    {
        std::ifstream file(scenePath);

        if (!file.is_open())
        {
            return false;
        }

        json root;

        try
        {
            file >> root;
        }
        catch (const json::exception&)
        {
            return false;
        }

        if (!root.contains("version") || root["version"].get<int>() != 1)
        {
            return false;
        }

        scene.setName(root.value("name", scenePath.stem().string()));

        if (root.contains("camera") && root["camera"].is_object())
        {
            const auto& cameraJson = root["camera"];
            auto& cameraObject = scene.getCameraObject();

            if (cameraJson.contains("position"))
            {
                cameraObject.transform.translation = deserializeVec3(cameraJson["position"]);
            }

            if (cameraJson.contains("rotation"))
            {
                cameraObject.transform.rotation = deserializeQuat(cameraJson["rotation"]);
            }
        }

        std::unordered_map<le::LeActor::id_t, le::LeActor::id_t> idRemap;

        struct PendingParent { le::LeActor::id_t newId; le::LeActor::id_t oldParentId; };
        std::vector<PendingParent> pendingParents;

        if (!root.contains("actors") || !root["actors"].is_array())
        {
            return true;
        }

        for (const auto& actorJson : root["actors"])
        {
            uint32_t modelId = 0;
            uint32_t textureId = 0;

            const std::string modelPath = actorJson.value("model", "");
            const std::string texturePath = actorJson.value("texture", "");

            if (!modelPath.empty())
            {
                modelId = resources.loadModel(makeAbsoluteAssetPath(project, modelPath).string());
            }

            if (!texturePath.empty())
            {
                textureId = resources.loadTexture(makeAbsoluteAssetPath(project, texturePath).string());
            }

            const auto newId = scene.addActor(modelId, textureId);

            auto* actor = scene.getActorById(newId);

            if (!actor)
            {
                return false;
            }

            actor->color = deserializeVec3(actorJson["color"]);
            actor->transform.translation = deserializeVec3(actorJson["position"]);
            actor->transform.rotation = deserializeQuat(actorJson["rotation"]);
            actor->transform.scale = deserializeVec3(actorJson["scale"]);

            const auto oldId = actorJson.value<le::LeActor::id_t>("id", newId);

            idRemap[oldId] = newId;

            if (actorJson.contains("script") && actorJson["script"].is_string())
            {
                actor->setScriptClassName(actorJson["script"].get<std::string>());
            }

            if (actorJson.contains("parent") && !actorJson["parent"].is_null())
            {
                pendingParents.push_back({newId, actorJson["parent"].get<le::LeActor::id_t>()});
            }
        }

        for (const auto& pending : pendingParents)
        {
            auto parentIt = idRemap.find(pending.oldParentId);

            if (parentIt == idRemap.end())
            {
                continue;
            }

            scene.setParent(pending.newId, parentIt->second);
        }

        return true;
    }

}