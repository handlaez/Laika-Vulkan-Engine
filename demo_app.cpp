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
#include "Boid.hpp"

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

    void onStart(le::LeScene& scene) override {
        // reserving the space so it doesn't change the address (hopefully)
        const int boidCount = 200;
        boidActorIndices.reserve(boidCount);
        scene.getActors().reserve(scene.getActors().size() + boidCount);

        // those are handles! (integers i think)
        auto model1 = scene.leResourceManager.loadModel("models/ugly_fish/ugly_fish.obj");
        auto texture1 = scene.leResourceManager.loadTexture("models/ugly_fish/fish.jpg");

        auto& actors = scene.getActors();

        for (int i = 0; i < boidCount; i++)
        {
            glm::vec3 pos(
                randf() * 50.f,
                randf() * 20.f,
                randf() * 50.f
            );

            boidSystem.AddBoid(Boid(pos));

            scene.addActor(model1, texture1);

            size_t newIndex = scene.getActors().size() - 1;

            boidActorIndices.push_back(newIndex);

            scene.getActors()[newIndex].transform.translation = pos;
        }
    }

    void onUpdate(le::LeScene& scene, FrameInfo fi) override {
        // FPS counter
        counter++;

        double crntTime = glfwGetTime();
        float deltaTime = crntTime - lastTime;
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

        // camera update
        scene.getCamera().setPerspectiveProjection(glm::radians(50.f), fi.aspect, 1.f, 5000.f);
        scene.getCamera().setView(
            scene.getCameraObject().transform.translation,
            scene.getCameraObject().transform.rotation
        );

        // boids
        boidSystem.Update(fi.deltaTime);

        auto& boids = boidSystem.GetBoids();
        auto& actors = scene.getActors(); // <-- critical change

        for (size_t i = 0; i < boids.size(); i++)
        {
            auto& actor = actors[boidActorIndices[i]];

            actor.transform.translation = boids[i].GetPosition();

            glm::vec3 vel = boids[i].GetVelocity();
            if (glm::length(vel) > 0.001f)
            {
                glm::vec3 dir = glm::normalize(vel);

                // handle Euler vs quat
                actor.transform.rotation = glm::eulerAngles(
                    glm::quatLookAt(dir, glm::vec3(0, 1, 0))
                );
            }
        }
    }

    void onShutdown() override {
    }

private:
    le::KeyboardMovementController controller{};
};