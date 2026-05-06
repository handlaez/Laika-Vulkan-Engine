#include "i_laika_engine_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_model.hpp"
#include "le_camera.hpp"
#include "le_scene.hpp"
#include "le_frame_info.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BoidSystem.hpp"

using namespace le;

class DemoApp : public ILaikaEngineApp {
    BoidSystem boidSystem;
    std::vector<size_t> boidActorIndices;

    // fps counter
    double prevTime = 0.0;
    double lastTime = 0.0;
    unsigned int counter = 0;

public:
    float randf()
    {
        return -1.0f + (rand() / (RAND_MAX / 2.0f));
    }

    void onStart(le::LeScene& scene) override
    {
        const int boidCount = 5000;

        boidActorIndices.reserve(boidCount);
        scene.getActors().reserve(scene.getActors().size() + boidCount);

        auto model = scene.leResourceManager.loadModel("models/ugly_fish/ugly_fish.obj");
        auto texture = scene.leResourceManager.loadTexture("models/ugly_fish/fish.jpg");

        auto& actors = scene.getActors();

        for (int i = 0; i < boidCount; i++)
        {
            glm::vec3 pos(
                randf() * 50.f,
                randf() * 20.f,
                randf() * 50.f
            );

            boidSystem.AddBoid(pos);

            scene.addActor(model, texture);

            size_t index = actors.size() - 1;

            boidActorIndices.push_back(index);

            actors[index].transform.translation = pos;
        }
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
        auto& actors = scene.getActors();

        const size_t count = boidActorIndices.size();

        for (size_t i = 0; i < count; i++)
        {
            const size_t actorIndex = boidActorIndices[i];

            auto& actor = actors[actorIndex];

            // position sync
            glm::vec3 pos = glm::vec3(positions[i]);
            glm::vec3 vel = glm::vec3(velocities[i]);

            actor.transform.translation = pos;

            // orientation from velocity
            if (glm::length(vel) > 0.001f)
            {
                glm::vec3 dir = glm::normalize(vel);

                glm::quat rot = glm::quatLookAt(dir, glm::vec3(0, 1, 0));

                actor.transform.rotation = rot;
                // OR:
                // actor.transform.rotation = glm::eulerAngles(rot);
            }
        }
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};