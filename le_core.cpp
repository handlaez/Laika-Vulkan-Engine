#include "le_core.hpp"
#include "basic_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "le_texture.hpp"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

namespace le {

	LeCore::LeCore() {
		loadActors();
	}

	LeCore::~LeCore() {}

	void LeCore::run() {
        LeTexture tex{ leDevice, "textures/texture.jpg" };
		BasicRenderSystem basicRenderSystem{ leDevice, leRenderer.getSwapchainRenderPass(), tex.getImageView() };
        LeCamera camera{};

        auto cameraObject = LeActor::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!leWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(leWindow.getGLFWwindow(), frameTime, cameraObject);
            camera.setView(cameraObject.transform.translation, cameraObject.transform.rotation);

            float aspect = leRenderer.getAspectRatio();
            //camera.setOrtographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f); //has to be updated in case of window being stretched

            // first actor is always spinning
            actors.at(0).transform.rotation.x = actors.at(0).transform.rotation.x + (1.f * frameTime);
            actors.at(0).transform.rotation.y = actors.at(0).transform.rotation.y + (3.f * frameTime);
            // this is just for testing purposes

			if (auto commandBuffer = leRenderer.beginFrame()) { // returns nullptr if swapchain needs to be recreated
				leRenderer.beginSwapChainRenderPass(commandBuffer);

				basicRenderSystem.renderActors(commandBuffer, actors, camera, leRenderer.getFrameIndex());

				leRenderer.endSwapChainRenderPass(commandBuffer);
				leRenderer.endFrame();
			}
		}
	}

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<LeModel> createCubeModel(LeDevice& device, glm::vec3 offset) {
        LeModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}, {1.f, 0.f}},
            {{-.5f,  .5f,  .5f}, {.9f, .9f, .9f}, {0.f, 1.f}},
            {{-.5f, -.5f,  .5f}, {.9f, .9f, .9f}, {0.f, 0.f}},
            {{-.5f,  .5f, -.5f}, {.9f, .9f, .9f}, {1.f, 1.f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}, {0.f, 0.f}},
            {{.5f,  .5f,  .5f}, {.8f, .8f, .1f}, {1.f, 1.f}},
            {{.5f, -.5f,  .5f}, {.8f, .8f, .1f}, {1.f, 0.f}},
            {{.5f,  .5f, -.5f}, {.8f, .8f, .1f}, {0.f, 1.f}},

            // top face (violet)
            {{-.5f, -.5f, -.5f}, {.4f, .1f, .8f}, {0.f, 1.f}},
            {{ .5f, -.5f,  .5f}, {.4f, .1f, .8f}, {1.f, 0.f}},
            {{-.5f, -.5f,  .5f}, {.4f, .1f, .8f}, {0.f, 0.f}},
            {{ .5f, -.5f, -.5f}, {.4f, .1f, .8f}, {1.f, 1.f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}, {1.f, 0.f}},
            {{ .5f, .5f,  .5f}, {.8f, .1f, .1f}, {0.f, 1.f}},
            {{-.5f, .5f,  .5f}, {.8f, .1f, .1f}, {1.f, 1.f}},
            {{ .5f, .5f, -.5f}, {.8f, .1f, .1f}, {0.f, 0.f}},

            // back face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}, {1.f, 0.f}},
            {{ .5f,  .5f, 0.5f}, {.1f, .1f, .8f}, {0.f, 1.f}},
            {{-.5f,  .5f, 0.5f}, {.1f, .1f, .8f}, {1.f, 1.f}},
            {{ .5f, -.5f, 0.5f}, {.1f, .1f, .8f}, {0.f, 0.f}},

            // front face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}, {0.f, 0.f}},
            {{ .5f,  .5f, -0.5f}, {.1f, .8f, .1f}, {1.f, 1.f}},
            {{-.5f,  .5f, -0.5f}, {.1f, .8f, .1f}, {0.f, 1.f}},
            {{ .5f, -.5f, -0.5f}, {.1f, .8f, .1f}, {1.f, 0.f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

        return std::make_unique<LeModel>(device, modelBuilder);
    }

	void LeCore::loadActors()
	{
        std::shared_ptr<LeModel> leModel = createCubeModel(leDevice, { .0f, .0f, .0f });

        auto cube = LeActor::createGameObject();
        cube.model = leModel;
        cube.transform.translation = { .0f, .0f, 5.f };
        cube.transform.scale = { 1.f, 1.f, 1.f };
        actors.push_back(std::move(cube));
	}

} // le