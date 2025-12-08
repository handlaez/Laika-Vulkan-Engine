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
        auto model1 = scene.leResourceManager.loadModel("models/viking_room.obj");
        auto texture1 = scene.leResourceManager.loadTexture("textures/viking_room.png");
        scene.addActor(0, 0); // missing texture cube
        scene.addActor(model1, texture1); // obama cube
        scene.getActor(0).transform.translation.x += 5.00f;
        scene.getActor(1).transform.translation.x -= 0.75f;
        scene.getActor(0).transform.translation.z += 5.00f;
        scene.getActor(1).transform.translation.z += 5.00f;
        scene.getActor(1).transform.rotation.x =  1.5707f;
        scene.getActor(1).transform.rotation.z = -1.5707f;
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