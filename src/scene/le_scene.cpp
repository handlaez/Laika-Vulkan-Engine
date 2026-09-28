#include "src/scene/le_scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace le {

    LeScene::LeScene(LeDevice& device, LeResourceManager& resourceManager)
        : leDevice(device), leResourceManager(resourceManager) {
        renderHitboxes_ = false;
        createDefaultCamera();
    }

    const std::string& LeScene::getName() const
    {
        return name_;
    }

    void LeScene::setName(std::string name)
    {
        name_ = std::move(name);
    }

    LeDevice& LeScene::getDevice() {
        return leDevice;
    }

    uint32_t LeScene::addActor(LeActor actor) {
        const uint32_t id = actor.getId();
        actors.push_back(std::move(actor));
        return id;
    }

    uint32_t LeScene::addActor(int32_t model, int32_t texture) {
        LeActor actor(model, texture);
        const uint32_t id = actor.getId();
        actors.push_back(std::move(actor));
        return id;
    }

    void LeScene::toggleRenderHitboxes() {
        renderHitboxes_ = !renderHitboxes_;
    }

    bool LeScene::getRenderHitboxes() const {
        return renderHitboxes_;
    }

    LeActor& LeScene::getActor(size_t index) {
        return actors.at(index);
    }

    const LeActor& LeScene::getActor(size_t index) const {
        return actors.at(index);
    }

    const std::vector<LeActor>& LeScene::getActors() const {
        return actors;
    }

    LeActor* LeScene::getActorById(LeActor::id_t id)
    {
        for (auto& actor : actors)
        {
            if (actor.getId() == id)
            {
                return &actor;
            }
        }

        return nullptr;
    }

    const LeActor* LeScene::getActorById(LeActor::id_t id) const
    {
        for (const auto& actor : actors)
        {
            if (actor.getId() == id)
            {
                return &actor;
            }
        }

        return nullptr;
    }

    bool LeScene::removeActorById(LeActor::id_t id)
    {
        auto it = std::find_if(actors.begin(), actors.end(), [id](const LeActor& actor)
            {
                return actor.getId() == id;
            }
        );

        if (it == actors.end())
        {
            return false;
        }

        for (auto& actor : actors)
        {
            if (actor.getParentId() == id)
            {
                actor.clearParent();
            }
        }

        actors.erase(it);
        return true;
    }

    LeCamera& LeScene::getCamera() {
        return camera;
    }

    const LeCamera& LeScene::getCamera() const {
        return camera;
    }

    LeActor& LeScene::getCameraObject() {
        return *cameraObject;
    }

    std::unique_ptr<LeScene> LeScene::clone() const
    {
        auto result = std::make_unique<LeScene>(leDevice, leResourceManager);

        result->renderHitboxes_ = renderHitboxes_;
        result->actors.reserve(actors.size());

        for (const auto& actor : actors)
        {
            result->actors.push_back(actor.clone());
        }

        result->camera = camera;

        if (cameraObject)
        {
            result->cameraObject = std::make_unique<LeActor>(cameraObject->clone());
        }

        // This is external state and must not be blindly copied.
        result->instanceDataPtr = nullptr;
        result->terrain_ = nullptr;

        return result;
    }

    bool LeScene::setParent(LeActor::id_t child, LeActor::id_t parent)
    {
        auto* childActor = getActorById(child);
        auto* parentActor = getActorById(parent);

        if (!childActor || !parentActor)
        {
            return false;
        }

        if (child == parent)
        {
            return false;
        }

        if (isDescendant(parent, child))
        {
            return false;
        }

        childActor->setParentId(parent);
        return true;
    }

    void LeScene::clearParent(LeActor::id_t child)
    {
        auto* actor = getActorById(child);

        if (!actor)
        {
            return;
        }

        actor->clearParent();
    }

    std::vector<LeActor::id_t> LeScene::getRootActors() const
    {
        std::vector<LeActor::id_t> result;
        result.reserve(actors.size());

        for (const auto& actor : actors)
        {
            if (!actor.getParentId().has_value())
            {
                result.push_back(actor.getId());
            }
        }

        return result;
    }

    std::vector<LeActor::id_t> LeScene::getChildren(LeActor::id_t parent) const
    {
        std::vector<LeActor::id_t> result;

        for (const auto& actor : actors)
        {
            const auto actorParent = actor.getParentId();

            if (actorParent.has_value() && *actorParent == parent)
            {
                result.push_back(actor.getId());
            }
        }

        return result;
    }

    bool LeScene::isDescendant(LeActor::id_t actor, LeActor::id_t potentialAncestor) const {
        const auto* current = getActorById(actor);

        if (!current)
        {
            return false;
        }

        while (current->getParentId().has_value())
        {
            const auto parentId = *current->getParentId();

            if (parentId == potentialAncestor)
            {
                return true;
            }

            current = getActorById(parentId);

            if (!current)
            {
                return false;
            }
        }

        return false;
    }

    void LeScene::createDefaultCamera() {
        cameraObject = std::make_unique<LeActor>();
        cameraObject->transform.translation = { 0.f, 128.f, 0.f };
        cameraObject->transform.rotation = glm::quat(1.f, 0.f, 0.f, 0.f);

        camera.setPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 100.f);
        camera.setView(cameraObject->transform.translation, cameraObject->transform.rotation);
    }

} // namespace le