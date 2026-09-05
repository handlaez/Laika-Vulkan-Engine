#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/core/le_device.hpp"
#include "src/scene/le_resource_manager.hpp"

// std
#include <vector>
#include <memory>

namespace le {

    class LeScene {
    public:
        explicit LeScene(LeDevice& device, LeResourceManager& resourceManager);

        LeDevice& getDevice();
        LeResourceManager& leResourceManager;

        std::shared_ptr<LeActor> addActor(int32_t model, int32_t texture);

        void toggleRenderHitboxes();
        bool getRenderHitboxes() const;

        std::vector<std::shared_ptr<LeActor>>& getActors();
        std::shared_ptr<LeActor> getActor(int index);

        LeCamera& getCamera();
        const LeCamera& getCamera() const;

        LeActor& getCameraObject();

    private:
        void createDefaultCamera();

        bool renderHitboxes_;

        LeDevice& leDevice;
        std::vector<std::shared_ptr<LeActor>> actors;

        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif
