#include "demo_app.hpp"

#include "src/objects/le_model.hpp"
#include "src/objects/le_camera.hpp"
#include "src/objects/le_BVH.hpp"

#include <iostream>

using namespace le;

float sinfunc = 0.0f;

void DemoApp::onStart(LeScene& scene) {
    // loading models
    uint32_t model1 = scene.leResourceManager.loadModel("models/viking_room.obj");
    uint32_t texture1 = scene.leResourceManager.loadTexture("textures/viking_room.png");

    scene.getCameraObject().transform.translation.z = -3.0f;
    scene.getCameraObject().transform.translation.y = 2.5f;
    scene.getCameraObject().transform.rotation.x = 0.3736f;

    scene.addActor(model1, texture1); // loaded model
    scene.addActor(0, 0); // missing texture cube
    scene.addActor(model1, texture1); // second loaded model
    scene.addActor(0, 0); // missing texture cube

    auto& modelActor1 = scene.getActor(0);
    auto& cubeActor1 = scene.getActor(1);
    auto& modelActor2 = scene.getActor(2);
    auto& cubeActor2 = scene.getActor(3);

    cubeActor1.transform.translation = glm::vec3{-1.f, 0.6f, 0.f };
    modelActor2.transform.translation = glm::vec3{ 1.f,  0.f,  2.f };
    cubeActor1.transform.scale = glm::vec3(0.5f);
    cubeActor2.transform.scale = glm::vec3(0.5f);

    // testing grounds
    scene.toggleRenderHitboxes();
    cubeActor1.addHitbox(glm::vec3(0.f), glm::vec3(0.25f));
    cubeActor2.addHitbox(glm::vec3(0.f), glm::vec3(0.25f));

    std::shared_ptr<LeModel> m = scene.leResourceManager.getStaticModel(model1);
    std::shared_ptr<BVH> b = m->getBVH();
    b->build(m->getPositions(), m->getIndices());
    modelActor1.setBVH(b);
    modelActor2.setBVH(b);
}

void DemoApp::onUpdate(le::LeScene& scene, FrameInfo fi) {
    // movement
    controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());
    sinfunc += 0.8f * fi.deltaTime;

    // camera update
    scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 0.1f, 100.f);
    scene.getCamera().setView(scene.getCameraObject().transform.translation, scene.getCameraObject().transform.rotation);

    // BVH colision test
    auto& modelActor1 = scene.getActor(0);
    auto& cubeActor1 = scene.getActor(1);
    auto& modelActor2 = scene.getActor(2);
    auto& cubeActor2 = scene.getActor(3);

    modelActor1.transform.translation.x = -2 + 1.5f * glm::sin(sinfunc);
    modelActor1.transform.translation.z = 0.5f * glm::cos(sinfunc);

    cubeActor2.transform.translation.x = 1.5f * glm::cos(sinfunc);
    cubeActor2.transform.translation.z = 2 + 0.5f * glm::sin(sinfunc);

    modelActor1.checkCollision(cubeActor1);
    modelActor2.checkCollision(cubeActor2);
}

void DemoApp::onShutdown() {
    // Cleanup if necessary
}

/*
#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_scene.hpp"
#include "le_frame_info.hpp"
#include "le_utils.hpp"
#include "profiler.hpp"

//test
#include "terrain_generation_system.hpp"
#include "procedural_terrain.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

using namespace le;

class DemoApp : public ILaikaEngineApp {

public:
    void onStart(le::LeScene& scene) override
    {
        auto terrain = std::make_unique<ProceduralTerrain>(scene.getDevice(), 21, Utils::randf()+1.f*10000, 64, 1.f);

        // terrain init, using one-time cmdBuffer
        VkCommandBuffer cmd = scene.getDevice().beginSingleTimeCommands();
        terrain->init(scene, scene.getCameraObject().transform.translation, cmd);
        scene.getDevice().endSingleTimeCommands(cmd);

        scene.setTerrain(std::move(terrain));
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override
    {
        // movement
        controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());

        // camera
        auto& camera = scene.getCamera();
        auto& camObj = scene.getCameraObject();

        camera.setPerspectiveProjection(glm::radians(45.f), fi.aspect, 1.f, 2048.f);
        camera.setView(camObj.transform.translation, camObj.transform.rotation);

        Utils::checkKeys(fi.window);
    }

    void onShutdown() override {
    }

private:
    KeyboardMovementController controller{};
};
*/
