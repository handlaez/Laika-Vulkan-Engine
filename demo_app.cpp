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
        auto& device = scene.getDevice();
        auto& window = device.getWindow();
        GLFWwindow = window.getGLFWwindow();

        // auto cubeModel = le::LeModel::createCube(scene.getDevice());
        auto cubeModel = le::LeModel::createModelFromFile(scene.getDevice(), "models/cottage_obj.obj", "textures/cottage_diffuse.png");
        le::LeActor cube = LeActor::createGameObject();
        cube.model = std::move(cubeModel);
        cube.transform.translation = { 0.f, 0.f, 5.f };
        cube.transform.scale = { 1.f, 1.f, 1.f };
        scene.addActor(std::move(cube));
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override {
        // movement
        controller.moveInPlaneXZ(GLFWwindow, fi.deltaTime, scene.getCameraObject());

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
    GLFWwindow* GLFWwindow;
};