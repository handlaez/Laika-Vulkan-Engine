#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_scene.hpp"
#include "le_frame_info.hpp"
#include "le_utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// boid includes
#include "boid_system.hpp"
#include "instanced_render_system.hpp"
#include "instance_data.hpp"

#include <omp.h>
#include <iostream>

using namespace le;

class DemoApp : public ILaikaEngineApp {
    // boids
    BoidSystem boidSystem;
    std::vector<size_t> boidActorIndices;
    std::vector<InstanceData> instances_;
    uint32_t boidModelID;
    uint32_t boidTextureID;

    // fps counter
    double prevTime = 0.0;
    double lastTime = 0.0;
    unsigned int counter = 0;

public:
    void onStart(le::LeScene& scene) override
    {
        const int boidCount = 5000;
        instances_.resize(boidCount);

        boidActorIndices.reserve(boidCount);

        auto model = scene.leResourceManager.loadModel("models/trout/Mesh_Trout.obj");
        auto texture = scene.leResourceManager.loadTexture("models/trout/Tex_Trout.png");

        boidModelID = model;
        boidTextureID = texture;

        for (int i = 0; i < boidCount; i++)
        {
            glm::vec3 pos(
                Utils::randf() * 100.f,
                Utils::randf() * 20.f,
                Utils::randf() * 100.f
            );

            boidSystem.AddBoid(pos);
        }

        int max_threads = omp_get_max_threads();
        std::cout << "OpenMP is using " << max_threads << " threads." << std::endl;
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override
    {
        // FPS counter
        counter++;

        double crntTime = glfwGetTime();
        float deltaTime = fi.deltaTime;
        lastTime = crntTime;

        double timeDiff = crntTime - prevTime;

        if (timeDiff >= 1.0) {
            double fps = counter / timeDiff;
            double msPerFrame = (timeDiff / counter) * 1000.0;

            std::string title =
                "Vulkan Engine - " +
                std::to_string(fps) + " FPS (" +
                std::to_string(msPerFrame) + " ms)";

            glfwSetWindowTitle(fi.window, title.c_str());

            prevTime = crntTime;
            counter = 0;
        }

        // movement
        controller.moveInPlaneXZ(fi.window, fi.deltaTime, scene.getCameraObject());

        // camera
        auto& camera = scene.getCamera();
        auto& camObj = scene.getCameraObject();

        camera.setPerspectiveProjection(glm::radians(45.f), fi.aspect, 1.f, 5000.f);
        camera.setView(
            camObj.transform.translation,
            camObj.transform.rotation
        );

        boidSystem.Update(deltaTime);

        auto& positions = boidSystem.GetPositions();
        auto& velocities = boidSystem.GetVelocities();

        const int boidCount = static_cast<int>(positions.size());

        #pragma omp parallel for if(Utils::parallelEnabled.load())
        for (int i = 0; i < boidCount; i++) // int for MSVC OpenMP
        {
            instances_[i].position = glm::vec4(positions[i], 1.f);

            if (glm::length(velocities[i]) > 0.001f)
            {
                instances_[i].forward = glm::vec4(glm::normalize(velocities[i]), 0.f);
            }
            else
            {
                instances_[i].forward = glm::vec4(0.f, 0.f, 1.f, 0.f);
            }
        }

        scene.setInstanceData(instances_);

        Utils::checkKeys(fi.window);
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};