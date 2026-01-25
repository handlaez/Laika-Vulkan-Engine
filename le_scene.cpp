#include "le_scene.hpp"
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

    std::shared_ptr<LeActor> LeScene::addActor(int32_t model, int32_t texture)
    {
        auto actor = std::make_shared<LeActor>(model, texture);
        actors.push_back(actor);
        return actor;
    }

    void LeScene::toggleRenderHitboxes()
    {
        renderHitboxes_ = !renderHitboxes_;
    }

    bool LeScene::getRenderHitboxes() const
    {
        return renderHitboxes_;
    }

    std::shared_ptr<LeActor> LeScene::getActor(int index) {
        return actors.at(index);
    }

    std::vector<std::shared_ptr<LeActor>>& LeScene::getActors() {
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
        cameraObject->transform.translation = { 0.f, 0.f, 0.f };
        cameraObject->transform.rotation = { 0.f, 0.f, 0.f };

        camera.setPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 100.f);
        camera.setView(cameraObject->transform.translation, cameraObject->transform.rotation);
    }

} // namespace le
