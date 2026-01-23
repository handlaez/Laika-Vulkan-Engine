#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_scene.hpp"
#include "le_frame_info.hpp"

using namespace le;

class DemoApp : public ILaikaEngineApp {
public:
    void onStart(le::LeScene& scene) override {
        uint32_t model1 = scene.leResourceManager.loadModel("models/viking_room.obj");
        uint32_t texture1 = scene.leResourceManager.loadTexture("textures/viking_room.png");

        scene.addActor(model1, texture1); // loaded model
        scene.addActor(0, 0); // missing texture cube
        scene.addActor(0, 0); // missing texture cube

        LeActor& modelActor = scene.getActor(0);
        LeActor& cubeActor = scene.getActor(1);
        LeActor& cubeActor2 = scene.getActor(2);
        
        cubeActor.transform.translation = glm::vec3{ -1.25f, 0.f, 5.f };
        cubeActor.transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };

        modelActor.transform.translation = glm::vec3{ +0.75f, 0.f, 5.f };
        modelActor.transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };

        cubeActor2.transform.translation = glm::vec3{ +2.75f, 0.f, 5.f };
        cubeActor2.transform.rotation = glm::vec3{ 1.5707f, 0.f, -1.5707f };
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override {
        // movement
        controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());

        // camera update
        scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 0.1f, 100.f);
        scene.getCamera().setView(scene.getCameraObject().transform.translation, scene.getCameraObject().transform.rotation);

        // cubamid rotate
        if (!scene.getActors().empty()) {
            scene.getActors()[0].transform.rotation.y += 1.1f * fi.deltaTime;
            scene.getActors()[0].transform.rotation.x += 0.6f * fi.deltaTime;
        }
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};