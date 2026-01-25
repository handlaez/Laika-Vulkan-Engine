#include "demo_app.hpp"

#include "le_model.hpp"
#include "le_camera.hpp"

using namespace le;

void DemoApp::onStart(LeScene& scene) {
    uint32_t model1 = scene.leResourceManager.loadModel("models/viking_room.obj");
    uint32_t texture1 = scene.leResourceManager.loadTexture("textures/viking_room.png");

    scene.addActor(model1, texture1); // loaded model
    scene.addActor(0, 0); // missing texture cube
    scene.addActor(0, 0); // missing texture cube (2)

    auto modelActor = scene.getActor(0);
    auto cubeActor1 = scene.getActor(1);
    auto cubeActor2 = scene.getActor(2);

    cubeActor1->transform.translation = glm::vec3{ -2.0f, 0.f, 5.f };
    cubeActor1->transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };

    modelActor->transform.translation = glm::vec3{ 0.0f, 0.f, 5.f };
    modelActor->transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };

    cubeActor2->transform.translation = glm::vec3{ +2.0f, 0.f, 5.f };
    cubeActor2->transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };

    // testing grounds
    scene.toggleRenderHitboxes();
    modelActor->addHitbox(glm::vec3(0.f, -0.25f, 0.0f), glm::vec3(0.5f)); //vulkan Y points down for dicks sake
    cubeActor1->addHitbox(glm::vec3(0.f), glm::vec3(0.5f));
    cubeActor2->addHitbox(glm::vec3(0.f), glm::vec3(0.5f));
}

void DemoApp::onUpdate(le::LeScene& scene, FrameInfo fi) {
    // movement
    controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());

    // hitbox render
    if (controller.getToggleHitboxPressed(fi.window)) {
        scene.toggleRenderHitboxes();
    }

    // camera update
    scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 0.1f, 100.f);
    scene.getCamera().setView(scene.getCameraObject().transform.translation, scene.getCameraObject().transform.rotation);

    // cubamid rotate
    if (!scene.getActors().empty()) {
        scene.getActors()[0]->transform.rotation.z += 0.6f * fi.deltaTime;
        scene.getActors()[0]->transform.translation.z += 0.1f * fi.deltaTime;

        scene.getActors()[1]->transform.rotation.z += 0.5f * fi.deltaTime;
    }
}

void DemoApp::onShutdown() {
    // Cleanup if necessary
}