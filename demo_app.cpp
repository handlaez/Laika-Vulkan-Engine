#include "demo_app.hpp"

#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_BVH.hpp"

#include <iostream>

using namespace le;

float sinfunc = 0.0f;

void DemoApp::onStart(LeScene& scene) {
    // loading models
    uint32_t model1 = scene.leResourceManager.loadModel("models/viking_room.obj");
    uint32_t texture1 = scene.leResourceManager.loadTexture("textures/viking_room.png");

    scene.getCameraObject().transform.translation.z = -3.0f;
    scene.getCameraObject().transform.translation.y = -2.5f;
    scene.getCameraObject().transform.rotation.x = -0.5236f;

    scene.addActor(model1, texture1); // loaded model
    scene.addActor(0, 0); // missing texture cube
    scene.addActor(model1, texture1); // second loaded model
    scene.addActor(0, 0); // missing texture cube

    auto modelActor1 = scene.getActor(0);
    auto cubeActor1 = scene.getActor(1);
    auto modelActor2 = scene.getActor(2);
    auto cubeActor2 = scene.getActor(3);

    cubeActor1->transform.translation = glm::vec3{ -1.f, -0.6f, 0.f };
    modelActor2->transform.translation = glm::vec3{ 1.f,  0.f,  2.f };
    cubeActor1->transform.scale = glm::vec3(0.5f);
    cubeActor2->transform.scale = glm::vec3(0.5f);

    // testing grounds
    scene.toggleRenderHitboxes();
    cubeActor1->addHitbox(glm::vec3(0.f), glm::vec3(0.25f));
    cubeActor2->addHitbox(glm::vec3(0.f), glm::vec3(0.25f));

    std::shared_ptr<LeModel> m = scene.leResourceManager.getModel(model1);
    std::shared_ptr<BVH> b = m->getBVH();
    b->build(m->getPositions(), m->getIndices());
    modelActor1->setBVH(b);
    modelActor2->setBVH(b);
}

void DemoApp::onUpdate(le::LeScene& scene, FrameInfo fi) {
    // movement
    controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());
    sinfunc += 0.8f * fi.deltaTime;

    // hitbox render
    if (controller.getToggleHitboxPressed(fi.window)) {
        scene.toggleRenderHitboxes();
    }

    // camera update
    scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 0.1f, 100.f);
    scene.getCamera().setView(scene.getCameraObject().transform.translation, scene.getCameraObject().transform.rotation);

    // BVH colision test
    auto modelActor1 = scene.getActor(0);
    auto cubeActor1 = scene.getActor(1);
    auto modelActor2 = scene.getActor(2);
    auto cubeActor2 = scene.getActor(3);

    modelActor1->transform.translation.x = -2 + 1.5f * glm::sin(sinfunc);
    modelActor1->transform.translation.z = 0.5f * glm::cos(sinfunc);

    cubeActor2->transform.translation.x = 1.5f * glm::cos(sinfunc);
    cubeActor2->transform.translation.z = 2 + 0.5f * glm::sin(sinfunc);

    modelActor1->checkCollision(*cubeActor1);
    modelActor2->checkCollision(*cubeActor2);
}

void DemoApp::onShutdown() {
    // Cleanup if necessary
}