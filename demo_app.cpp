#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_scene.hpp"
#include "le_frame_info.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// boid includes
#include "BoidSystem.hpp"
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

        boidActorIndices.reserve(boidCount);
        scene.getActors().reserve(scene.getActors().size() + boidCount);

        auto model = scene.leResourceManager.loadModel("models/ugly_fish/ugly_fish.obj");
        auto texture = scene.leResourceManager.loadTexture("models/ugly_fish/fish.jpg");

        boidModelID = model;
        boidTextureID = texture;

        for (int i = 0; i < boidCount; i++)
        {
            glm::vec3 pos(
                le::randf() * 50.f,
                le::randf() * 20.f,
                le::randf() * 50.f
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

        camera.setPerspectiveProjection(glm::radians(50.f), fi.aspect, 1.f, 5000.f);
        camera.setView(
            camObj.transform.translation,
            camObj.transform.rotation
        );

        boidSystem.Update(deltaTime);

        auto& positions = boidSystem.GetPositions();
        auto& velocities = boidSystem.GetVelocities();

        instances_.clear();
        instances_.resize(positions.size());

        const int boidCount = static_cast<int>(positions.size());

        #pragma omp parallel for
        for (int i = 0; i < boidCount; i++) // int for MSVC OpenMP
        {
            glm::vec3 pos = positions[i];
            glm::vec3 vel = velocities[i];

            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);

            if (glm::length(vel) > 0.001f)
            {
                glm::vec3 dir = glm::normalize(vel);
                glm::quat rot = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
                model *= glm::mat4_cast(rot);
            }

            // direct assignment, NOT push_back
            instances_[i].model = model;
        }

        scene.instanceData = instances_;
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};