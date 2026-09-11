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