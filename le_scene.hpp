#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "le_actor.hpp"
#include "le_camera.hpp"
#include "le_device.hpp"
#include "le_resource_manager.hpp"

// std
#include <vector>
#include <memory>

namespace le {

    class LeScene {
    public:
        explicit LeScene(LeDevice& device, LeResourceManager& resourceManager);

        LeDevice& getDevice();
        LeResourceManager& leResourceManager;

        LeActor& addActor(LeActor actor);
        LeActor& addActor(int32_t model, int32_t texture);

        std::vector<LeActor>& getActors();
        const std::vector<LeActor>& getActors() const;
        LeActor& getActor(int index);

        LeCamera& getCamera();
        const LeCamera& getCamera() const;

        LeActor& getCameraObject();

    private:
        void createDefaultCamera();

        LeDevice& leDevice;
        std::vector<LeActor> actors;

        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif
