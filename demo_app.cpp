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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <omp.h>
#include <iostream>

using namespace le;

class DemoApp : public ILaikaEngineApp {

public:
    void onStart(le::LeScene& scene) override
    {
        int max_threads = omp_get_max_threads();
        std::cout << "OpenMP is using " << max_threads << " threads." << std::endl;

        MeshData chunk = TerrainGenerator::generateChunk(-1, -1);
        uint32_t terrainModelID = scene.leResourceManager.addModel(chunk);
        LeActor terrainActor = LeActor::createGameObject();
        terrainActor.modelID = terrainModelID;
        terrainActor.textureID = 0;
        scene.addActor(std::move(terrainActor));
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override
    {
        // movement
        controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());

        // camera
        auto& camera = scene.getCamera();
        auto& camObj = scene.getCameraObject();

        camera.setPerspectiveProjection(glm::radians(45.f), fi.aspect, 1.f, 5000.f);
        camera.setView(camObj.transform.translation, camObj.transform.rotation);

        Utils::checkKeys(fi.window);
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};