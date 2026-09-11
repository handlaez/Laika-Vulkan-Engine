#ifndef LE_SCENE_HPP
#define LE_SCENE_HPP

#include "src/objects/le_actor.hpp"
#include "src/objects/le_camera.hpp"
#include "src/core/le_device.hpp"
#include "src/scene/le_resource_manager.hpp"
#include "src/systems/boids/instance_data.hpp"
#include "src/systems/terrain-gen/procedural_terrain.hpp"

// std
#include <vector>
#include <memory>
#include <utility>

namespace le {

    class LeScene {
    public:
        explicit LeScene(LeDevice& device, LeResourceManager& resourceManager);

        LeDevice& getDevice();
        LeResourceManager& leResourceManager;

        uint32_t addActor(LeActor actor);
        uint32_t addActor(int32_t model, int32_t texture);

        void toggleRenderHitboxes();
        bool getRenderHitboxes() const;

        LeActor& getActor(size_t index);
        const LeActor& getActor(size_t index) const;
        const std::vector<LeActor>& getActors() const;

        LeCamera& getCamera();
        const LeCamera& getCamera() const;

        LeActor& getCameraObject();

        const std::vector<InstanceData>* instanceDataPtr = nullptr;

        void setInstanceData(const std::vector<InstanceData>& data) {
            instanceDataPtr = &data;
        }

        void setTerrain(std::unique_ptr<ProceduralTerrain> terrain) {
            terrain_ = std::move(terrain);
        }

        ProceduralTerrain* getTerrain() const {
            return terrain_.get();
        }

        bool hasTerrain() const {
            return terrain_ != nullptr;
        }

    private:
        void createDefaultCamera();

        bool renderHitboxes_ = false;
        std::unique_ptr<ProceduralTerrain> terrain_ = nullptr;

        LeDevice& leDevice;
        std::vector<LeActor> actors;
        std::unique_ptr<LeActor> cameraObject;
        LeCamera camera;
    };

} // namespace le

#endif