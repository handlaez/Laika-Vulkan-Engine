#include "le_scene.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace le {

    LeScene::LeScene(LeDevice& device, LeResourceManager& resourceManager)
        : leDevice(device), leResourceManager(resourceManager) {
        createDefaultCamera();
    }

    LeDevice& LeScene::getDevice() {
        return leDevice;
    }

    void LeScene::addActor(LeActor actor) {
        actors.push_back(std::move(actor));
    }

    void LeScene::addActor(int32_t model, int32_t texture)
    {
        actors.emplace_back(model, texture);
    }

    std::vector<LeActor>& LeScene::getActors() {
        return actors;
    }

    const std::vector<LeActor>& LeScene::getActors() const {
        return actors;
    }

    LeActor& LeScene::getActor(int index) {
        return actors.at(index);
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
        cameraObject = std::make_unique<LeActor>(LeActor::createGameObject());
        cameraObject->transform.translation = { 0.f, 0.f, -100.f };
        cameraObject->transform.rotation = glm::quat(1.f, 0.f, 0.f, 0.f);

        camera.setPerspectiveProjection(glm::radians(50.f), 1.f, 0.1f, 100.f);
        camera.setView(cameraObject->transform.translation, cameraObject->transform.rotation);
    }

} // namespace le
