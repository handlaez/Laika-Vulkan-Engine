#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "le_actor.hpp"
#include "le_camera.hpp"
#include "le_device.hpp"

// std
#include <vector>
#include <memory>

namespace le {

    class LeScene {
    public:
        explicit LeScene(LeDevice& device);

        LeDevice& getDevice();

        void addActor(LeActor actor);

        std::vector<LeActor>& getActors();
        const std::vector<LeActor>& getActors() const;

        LeCamera& getCamera();
        const LeCamera& getCamera() const;

        LeActor& getCameraObject();

    private:
        void createDefaultCamera();

    private:
        LeDevice& leDevice;
        std::vector<LeActor> actors;

        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif
