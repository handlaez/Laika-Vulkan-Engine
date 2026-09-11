#include "src/scene/le_scene.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace le {

    LeScene::LeScene(LeDevice& device, LeResourceManager& resourceManager)
        : leDevice(device), leResourceManager(resourceManager) {
        renderHitboxes_ = false;
        createDefaultCamera();
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

    LeCamera& LeScene::getCamera() {
        return camera;
    }

    const LeCamera& LeScene::getCamera() const {
        return camera;
    }

    LeActor& LeScene::getCameraObject() {
        return *cameraObject;
    }

    void LeScene::createDefaultCamera() {
        cameraObject = std::make_unique<LeActor>();
        cameraObject->transform.translation = { 0.f, 128.f, 0.f };
        cameraObject->transform.rotation = glm::quat(1.f, 0.f, 0.f, 0.f);

        camera.setPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 100.f);
        camera.setView(cameraObject->transform.translation, cameraObject->transform.rotation);
    }

} // namespace le